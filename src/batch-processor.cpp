/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include "batch-processor.hpp"
#include "ostreamlock.hpp"

#include <mpi.h>
#include <iostream>
#include <boost/regex.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;

using namespace std;

#define BP_HEAD_NODE 0
#define BP_WORKER_READY 1

#define BP_WORK_TAG 1
#define BP_WORKER_READY_TAG 0
#define BP_RESULT_TAG 1337
#define BP_WORKER_EXIT_TAG 42

BatchProcessor::BatchProcessor(int* argcp, char*** argvp, boost::threadpool::pool& pool) :
  output_stream(nullptr), pool(pool), scheduling_complete(false) {

  int provided;
  int error = MPI_Init_thread(argcp, argvp, MPI_THREAD_MULTIPLE, &provided);
  if (error) {
    BOOST_LOG_SEV(log, logging::trivial::fatal) << "Error: " << error << " initalizing MPI." << endl;
    exit(error);
  }

  BOOST_LOG_SEV(log, logging::trivial::info) << "MPI initialized.";

  if (provided < MPI_THREAD_MULTIPLE) {
    BOOST_LOG_SEV(log, logging::trivial::fatal)
      << "MPI multi-threading level:  MPI_THREAD_MULTIPLE not supported. Provided: " << provided << endl;
    exit(error);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  char tmp_proc_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(tmp_proc_name, &name_length);
  processor_name.assign(tmp_proc_name); // store processor name for later

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Batch processor initialized.";
}

void BatchProcessor::process_keys(function<void()> schedule_keys, function<void(const string &)> process_key) {
  if (world_rank == BP_HEAD_NODE) master_routine(schedule_keys);
  else worker_routine(process_key);
}

void BatchProcessor::wait() {
  if (world_rank != BP_HEAD_NODE) return;

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Main thread waiting on batch processing...";
  pool.wait();
  unique_lock<mutex> lock(worker_done_mutex);
  worker_done_cv.wait(lock, [this](){
    return work_completed();
  });
  lock.unlock();
  BOOST_LOG_SEV(log, logging::trivial::debug) << "Tasks completed.";
}

/**
 * Private method: master_routine
 * ------------------------------
 * Method used by the "head" node to coordinate work on all of the worker nodes.
 * This method will first call the get keys method which was passed, and fill the queue
 * of keys in the batch processor with
 */
void BatchProcessor::master_routine(function<void()> schedule_keys) {

  BOOST_LOG_SEV(log, logging::trivial::info) << "Starting master routine...";

  // Mark all workers as unavailable. (no need to lock, no threads yet)
  for (int i = 0; i < world_size; i++) {
    worker_ready_list.push_back(false);
    worker_mutex_list.push_back(make_shared<mutex>());
  }
  BOOST_LOG_SEV(log, logging::trivial::debug) << "Created worker readiness list.";

  // Spin up a thread to schedule all of the keys to be processed
  pool.schedule([&](){
    BOOST_LOG_SEV(log, logging::trivial::info) << "Scheduling keys...";
    schedule_keys(); // Schedule keys asynchronously
    lock_guard<mutex> lg(scheduling_complete_mutex);
    scheduling_complete = true; // Indicate done signaling
    BOOST_LOG_SEV(log, logging::trivial::info) << "Key scheduling complete...";
  });

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Created key scheduling thread task.";

  pool.schedule([&](){
    MPI_Status status;
    char worker_ready;

    while (!work_completed()) {

      // Find out what a worker wants to do next
      BOOST_LOG_SEV(log, logging::trivial::debug) << "Waiting for ready worker...";
      MPI_Probe(MPI_ANY_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR) continue;
      BOOST_LOG_SEV(log, logging::trivial::debug) << "Found ready worker rank: " << status.MPI_SOURCE;

      MPI_Recv(&worker_ready, sizeof(char), MPI_BYTE, status.MPI_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (!worker_ready || status.MPI_ERROR) continue; // Error or incorrect signal?

      if (queue_empty()) { // no more keys empty
        if (scheduling_completed()) break;
        else { // Wait until something has been put on the queue
          BOOST_LOG_SEV(log, logging::trivial::debug) << "Waiting for queue to be filled...";
          unique_lock<mutex> lock(schedule_mutex);
          schedule_cv.wait(lock, [this]() {
            return !queue_empty();
          });
          lock.unlock();
        }
      }

      // There is some work to do in the queue
      BOOST_LOG_SEV(log, logging::trivial::debug) << "Delegating work from queue...";

      unique_lock<mutex> lock(*worker_mutex_list[status.MPI_SOURCE]);
      worker_ready_list[status.MPI_SOURCE] = false; // Mark worker as busy
      lock.unlock();

      // Send the next work to the worker
      queue_mutex.lock();
      string next_key = keys.front();
      BOOST_LOG_SEV(log, logging::trivial::info) << "Sending: \"" << next_key << "\" to worker: " << status.MPI_SOURCE;
      MPI_Send(next_key.c_str(), (int) next_key.size() + 1, MPI_CHAR, status.MPI_SOURCE, BP_WORK_TAG, MPI_COMM_WORLD);
      keys.pop(); // Remove the key
      queue_mutex.unlock();
    }

    BOOST_LOG_SEV(log, logging::trivial::debug) << "Instructing workers to exit.";
    for (int i = 0; i < world_size; i++) {
      if (i == BP_HEAD_NODE) continue;
      send_exit_signal(i);
    }

    BOOST_LOG_SEV(log, logging::trivial::debug) << "Work delegation thread exiting.";
  });

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Main thread exiting master routine.";
}

/**
 * Private method: worker_routine
 * ------------------------------
 * Routine for workers to process keys sent from the master and return the result
 * @param processData: The function that the worker should use to process a key. It should take
 * as a parameter the key that will be sent over the network and return the result which will be
 * send back over the network to the master node.
 */
void BatchProcessor::worker_routine(function<void(const string &)> processKey) {
  BOOST_LOG_SEV(log, logging::trivial::info) << "Beginning worker routine...";

  size_t numProcessed = 0; // Worker keeps track of how many it has processed

  MPI_Status status;
  char ready = BP_WORKER_READY;

  int messageSize;
  string nextKey; // Stores the next key

  while (true) {

    // Tell the head node we are ready to process something
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Sending ready for work signal...";
    MPI_Send(&ready, sizeof(char), MPI_BYTE, BP_HEAD_NODE, BP_WORKER_READY_TAG, MPI_COMM_WORLD);
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Sent ready for work signal...";

    // Get information about the next key to come
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Probing incoming work key...";
    MPI_Probe(BP_HEAD_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    if (status.MPI_TAG == BP_WORKER_EXIT_TAG) break; // time to be done

    // Get the next key
    MPI_Get_count(&status, MPI_CHAR, &messageSize);
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Receiving key of size: " << messageSize;

    auto key_cstr = (char*) malloc((size_t) messageSize);
    MPI_Recv(key_cstr, messageSize, MPI_CHAR, BP_HEAD_NODE, BP_WORK_TAG, MPI_COMM_WORLD, &status);
    nextKey = key_cstr;
    free(key_cstr);

    if (status.MPI_ERROR) continue;

    BOOST_LOG_SEV(log, logging::trivial::info) << "Processing: " << nextKey << " ...";
    processKey(nextKey); // <-- work done here
    numProcessed++;
    BOOST_LOG_SEV(log, logging::trivial::info) << "Completed: " << nextKey << ". Total: " << numProcessed;
  }

  BOOST_LOG_SEV(log, logging::trivial::info) << "Worker exiting having processed: " << numProcessed << " keys.";
}

void BatchProcessor::schedule_key(const string &key) {
  if (world_rank != BP_HEAD_NODE) return;

  BOOST_LOG_SEV(log, logging::trivial::info) << "Queueing: " << key;
  lock_guard<mutex> lg(queue_mutex); // Lock the queue
  keys.push(key);
  schedule_cv.notify_one(); // Notify potentially waiting thread of scheduling
}

bool BatchProcessor::scheduling_completed() {
  lock_guard<mutex> lg(scheduling_complete_mutex);
  return scheduling_complete;
}

bool BatchProcessor::queue_empty() {
  lock_guard<mutex> lg(queue_mutex);
  return keys.empty();
}

// Sends the exit signal to a specified worker
void BatchProcessor::send_exit_signal(int worker) {
  BOOST_LOG_SEV(log, logging::trivial::debug) << "Sending exit signal to: " << worker;
  MPI_Send(&worker, 1, MPI_BYTE, worker, BP_WORKER_EXIT_TAG, MPI_COMM_WORLD);
}

bool BatchProcessor::work_completed() {
  return scheduling_completed() && queue_empty();
}

/**
 * Private method: init_logger
 * ---------------------------
 * Initializes the batch processor's logger
 */
void BatchProcessor::init_logger(bool verbose, bool debug) {
  boost::log::add_common_attributes();

  if (debug) boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
  else if (verbose) boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
  else boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);

  // log format: [processor rank/world_size] [TimeStamp] [Severity Level] Log message
  auto fmtTimeStamp = boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S");
  auto fmtSeverity = boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity");

  boost::log::formatter logFmt =
    boost::log::expressions::format("[%1% %2%/%3%] [%4%] [%5%] %6%")
    % processor_name
    % world_rank
    % world_size
    % fmtTimeStamp
    % fmtSeverity
    % boost::log::expressions::smessage;

  // console sink
  auto consoleSink = boost::log::add_console_log(std::clog);
  consoleSink->set_formatter(logFmt);
}


// Destructor
BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}