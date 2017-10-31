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

void BatchProcessor::process_keys(function<void()> schedule_keys,
                                  function<string(const string &)> process_key,
                                  function<shared_ptr<ostream>()> get_ostream) {

  if (world_rank == BP_HEAD_NODE) master_routine(schedule_keys, get_ostream);
  else worker_routine(process_key);
}

void BatchProcessor::wait() {
  if (world_rank != BP_HEAD_NODE) return;

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Waiting for batch processing tasks to complete...";
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
void BatchProcessor::master_routine(function<void()> schedule_keys,
                                    function<shared_ptr<ostream>()> get_ostream) {

  BOOST_LOG_SEV(log, logging::trivial::info) << "Starting master routine...";

  output_stream = get_ostream(); // Get the output stream ready to write to
  BOOST_LOG_SEV(log, logging::trivial::debug) << "Acquired output stream.";

  // Mark all workers as unavailable. (no need to lock, no threads yet)
  for (int i = 0; i < world_size; i++) worker_ready_list.push_back(false);
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

  // I/O thread task
  pool.schedule([&](){
    MPI_Status status;
    while (true) {
      MPI_Probe(MPI_ANY_SOURCE, BP_RESULT_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR) continue;
      BOOST_LOG_SEV(log, logging::trivial::debug) << "Recieved result from worker..";
      unique_lock<mutex> lock(*worker_mutex_list[status.MPI_SOURCE]);
      receive_and_process_result(status.MPI_SOURCE);
      worker_ready_list[status.MPI_SOURCE] = true;
      lock.unlock();
    }
  });

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Created key I/O thread task.";

  pool.schedule([&](){
    MPI_Status status;
    char worker_ready;

    while (!work_completed()) {
      BOOST_LOG_SEV(log, logging::trivial::debug) << "Waiting for ready worker...";
      // Find out what a worker wants to do next
      MPI_Probe(MPI_ANY_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR) continue;

      BOOST_LOG_SEV(log, logging::trivial::debug) << "Found ready worker: " << status.MPI_SOURCE;

      MPI_Recv(&worker_ready, sizeof(char), MPI_BYTE, status.MPI_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (!worker_ready || status.MPI_ERROR) continue; // Worker didn't send correct signal

      if (queue_empty()) { // no more keys empty
        if (scheduling_completed()) send_exit_signal(status.MPI_SOURCE); // No more work to do: worker may exit
        else { // Wait until something has been put on the queue
          BOOST_LOG_SEV(log, logging::trivial::info) << "Waiting for queue to be filled...";
          unique_lock<mutex> lock(schedule_mutex);
          schedule_cv.wait(lock, [this]() {
            return !queue_empty();
          });
          lock.unlock();
        }
      }

      // There is some work to do in the queue

      unique_lock<mutex> lock(*worker_mutex_list[status.MPI_SOURCE]);
      worker_ready_list[status.MPI_SOURCE] = false; // Mark worker as busy
      lock.unlock();

      // Send the next work to the worker
      queue_mutex.lock();
      string next_key = keys.front();
      BOOST_LOG_SEV(log, logging::trivial::info) << "Sending: " << next_key << " to wokrer: " << status.MPI_SOURCE;
      MPI_Send(next_key.c_str(), (int) next_key.size(), MPI_CHAR, status.MPI_SOURCE, BP_WORK_TAG, MPI_COMM_WORLD);
      keys.pop(); // Remove the key
      queue_mutex.unlock();
    }
  });

  BOOST_LOG_SEV(log, logging::trivial::info) << "Exiting master routine...";
}

/**
 * Private method: worker_routine
 * ------------------------------
 * Routine for workers to process keys sent from the master and return the result
 * @param processData: The function that the worker should use to process a key. It should take
 * as a parameter the key that will be sent over the network and return the result which will be
 * send back over the network to the master node.
 */
void BatchProcessor::worker_routine(function<string(const string &)> processKey) {
  BOOST_LOG_SEV(log, logging::trivial::info) << "Beginning worker routine...";

  size_t numProcessed = 0; // Worker keeps track of how many it has processed

  MPI_Status status;
  char ready = BP_WORKER_READY;

  size_t messageSize;
  string nextKey; // Stores the next key

  while (true) {

    // Tell the head node we are ready to process something
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Sending ready for work signal...";
    MPI_Send(&ready, sizeof(int), MPI_BYTE, BP_HEAD_NODE, BP_WORKER_READY_TAG, MPI_COMM_WORLD);
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Sent ready for work signal...";

    // Get information about the next key to come
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Probing for incoming work key...";
    MPI_Probe(BP_HEAD_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == BP_WORKER_EXIT_TAG) break;

    // Get the next key
    MPI_Get_count(&status, MPI_CHAR, (int *) &messageSize);
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Receiving key of size: " << messageSize;
    auto key_cstr = (char *) malloc(messageSize);
    MPI_Recv(key_cstr, (int) messageSize, MPI_CHAR, BP_HEAD_NODE, BP_WORK_TAG, MPI_COMM_WORLD, &status);
    nextKey = key_cstr;
    free(key_cstr);

    if (status.MPI_ERROR) continue;
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Received key: " << nextKey;

    // Process the key and send it back
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Processing: " << nextKey << " ...";
    string result = processKey(nextKey); // <-- work gets done here

    BOOST_LOG_SEV(log, logging::trivial::debug) << "Processing complete. Returning result from: " << nextKey;
    MPI_Send(result.c_str(), (int) result.size(), MPI_CHAR, BP_HEAD_NODE, BP_RESULT_TAG, MPI_COMM_WORLD);
    numProcessed++;
    BOOST_LOG_SEV(log, logging::trivial::debug) << "Result returned. Keys processed: " << numProcessed;
  }

  BOOST_LOG_SEV(log, logging::trivial::info) << "Worker exiting having processed: " << numProcessed << " keys.";
}

void BatchProcessor::schedule_key(const string &key) {
  if (world_rank != BP_HEAD_NODE) return;

  BOOST_LOG_SEV(log, logging::trivial::debug) << "Scheduling: " << key;
  lock_guard<mutex> lg(queue_mutex); // Lock the queue
  keys.push(key);
  schedule_cv.notify_one(); // Notify potentially waiting thread of scheduling
}

/**
 * Private method: receive_and_process_result
 * ------------------------------------------
 * Receives a single answer from a specified worker node and writes it to the output stream asynchronously
 */
void BatchProcessor::receive_and_process_result(int worker) {
  MPI_Status status;
  size_t message_size;
  MPI_Get_count(&status, MPI_CHAR, (int*) &message_size);

  auto answer = new char[message_size];
  MPI_Recv(answer, (int) message_size, MPI_CHAR, worker, BP_RESULT_TAG, MPI_COMM_WORLD, &status);
  BOOST_LOG_SEV(log, logging::trivial::debug) << "Received result from: " << worker;

  if (status.MPI_ERROR) {
    delete[] answer;
    return;
  }

  // Write answer to output file
  pool.schedule([this, answer](){
    *output_stream << oslock << answer << endl << osunlock;
    delete[] answer;
  });
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
  if (!scheduling_completed() || !queue_empty()) return false;

  for (int i = 0; i < world_size; i++) {
    if (i == BP_HEAD_NODE) continue;
    worker_mutex_list[i]->lock();
    if (!worker_ready_list[i]) {
      worker_mutex_list[i]->unlock();
      return false;
    }
    worker_mutex_list[i]->unlock();
  }
  return true;
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