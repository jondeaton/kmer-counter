/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include "batch-processor.h"
#include "ostreamlock.h"

#include <mpi.h>
#include <iostream>
#include <boost/regex.hpp>

#define BP_HEAD_NODE 0
#define BP_WORKER_READY 1

#define BP_WORK_TAG 1
#define BP_WORKER_READY_TAG 0
#define BP_RESULT_TAG 1337
#define BP_WORKER_EXIT_TAG 42

using namespace std;

BatchProcessor::BatchProcessor(int* argcp, char*** argvp, boost::threadpool::pool& pool) :
  output_stream(nullptr), pool(pool), schedulingComplete(false) {

  int provided;
  int error = MPI_Init_thread(argcp, argvp, MPI_THREAD_MULTIPLE, &provided);
  if (error) {
    cerr << "Error: " << error << " initalizing MPI." << endl;
    exit(error);
  }

  if (provided < MPI_THREAD_MULTIPLE) {
    cerr << "MPI multi-threading level:  MPI_THREAD_MULTIPLE not supported. Provided: " << provided << endl;
    exit(error);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  char tmp_proc_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(tmp_proc_name, &name_length);
  processor_name.assign(tmp_proc_name); // store processor name for later
}

void BatchProcessor::process_keys(function<void()> schedule_keys,
                                  function<string(const string &)> process_key,
                                  function<shared_ptr<ostream>()> get_ostream) {

  if (world_rank == BP_HEAD_NODE) master_routine(schedule_keys, get_ostream);
  else worker_routine(process_key);
}

void BatchProcessor::wait() {
  if (world_rank != BP_HEAD_NODE) return;
  pool.wait();
}

/**
 * Private method: masterRoutine
 * -----------------------------
 * Method used by the "head" node to coordinate work on all of the worker nodes.
 * This method will first call the get keys method which was passed, and fill the queue
 * of keys in the batch processor with
 */
void BatchProcessor::master_routine(function<void()> schedule_keys,
                                    function<shared_ptr<ostream>()> get_ostream) {

  output_stream = get_ostream(); // Get the output stream ready to write to
  // Mark all workers as unavailable. (no need to lock, no threads yet)
  for (int i = 0; i < world_size; i++) worker_ready_list[i] = false;

  // Spin up a thread to schedule all of the keys to be processed
  pool.schedule([&](){
    schedule_keys(); // Schedule keys asynchronously
    lock_guard<mutex> lg(scheduling_complete_mutex);
    schedulingComplete = true; // Indicate done signaling
  });

  // Schedule thread to write answers to disk
  pool.schedule([&](){
    MPI_Status status;
    while (true) {
      MPI_Probe(MPI_ANY_SOURCE, BP_RESULT_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR) continue;
      receive_and_process_result(status.MPI_SOURCE);
    }
  });

  pool.schedule([&](){
    MPI_Status status;
    char worker_ready;
    while (!work_completed()) {

      // Find out what a worker wants to do next
      MPI_Probe(MPI_ANY_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (status.MPI_ERROR) continue;

      MPI_Recv(&worker_ready, sizeof(char), MPI_BYTE, status.MPI_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (!worker_ready || status.MPI_ERROR) continue; // Worker didn't send correct signal

      if (queue_empty()) { // no more keys empty
        if (scheduling_completed()) send_exit_signal(status.MPI_SOURCE); // No more work to do: worker may exit
        else { // Wait until something has been put on the queue
          unique_lock<mutex> lock(schedule_mutex);
          schedule_cv.wait(lock, [this]() {
            return !queue_empty();
          });
        }
      }

      worker_ready_list[status.MPI_SOURCE] = false;

      // There is some work to do in the queue

      // Send the next work to the worker
      queue_mutex.lock();
      string next_key = keys.front();
      MPI_Send(next_key.c_str(), (int) next_key.size(), MPI_CHAR, status.MPI_SOURCE, BP_WORK_TAG, MPI_COMM_WORLD);
      keys.pop(); // Remove the key
      queue_mutex.unlock();
    }
  });
}

/**
 * Private method: workerRoutine
 * -----------------------------
 * Routine for workers to process keys sent from the master and return the result
 * @param processData: The function that the worker should use to process a key. It should take
 * as a parameter the key that will be sent over the network and return the result which will be
 * send back over the network to the master node.
 */
void BatchProcessor::worker_routine(function<string(const string &)> processKey) {
  size_t numProcessed = 0; // Worker keeps track of how many it has processed

  MPI_Status status;
  int ready = BP_WORKER_READY;

  size_t messageSize;
  string nextKey; // Stores the next key

  while (true) {

    // Tell the head node we are ready to process something
    MPI_Send(&ready, sizeof(int), MPI_BYTE, BP_HEAD_NODE, BP_WORKER_READY_TAG, MPI_COMM_WORLD);

    // Get information about the next key to come
    MPI_Probe(BP_HEAD_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == BP_WORKER_EXIT_TAG) { // Instructed by master to exit
      cout << "Worker \"" << processor_name << "\" exiting (rank " << world_rank << "/" << world_size << ") ";
      cout << "Processed: " << numProcessed << endl;
      break;
    }

    // Get the next key
    MPI_Get_count(&status, MPI_CHAR, (int*) &messageSize);
    auto key_cstr = (char*) malloc(messageSize);
    MPI_Recv(key_cstr, (int) messageSize, MPI_CHAR, BP_HEAD_NODE, BP_WORK_TAG, MPI_COMM_WORLD, &status);
    nextKey = key_cstr;
    free(key_cstr);

    if (status.MPI_ERROR) continue;

    // Process the key and send it back
    string result = processKey(nextKey); // <-- work gets done here
    MPI_Send(result.c_str(), (int) result.size(), MPI_CHAR, BP_HEAD_NODE, BP_RESULT_TAG, MPI_COMM_WORLD);

    numProcessed++;
  }
}

void BatchProcessor::schedule_key(const string &key) {
  if (world_rank != BP_HEAD_NODE) return;

  lock_guard<mutex> lg1(queue_mutex);
  keys.push(key);
  schedule_cv.notify_one(); // Notify potentially waiting node that something has been scheduled
}

/**
 * Private method: receiveAndProcessResult
 * ---------------------------------------
 * Receives an answer from a worker node and write it to the output stream asynchronously
 */
void BatchProcessor::receive_and_process_result(int worker) {
  MPI_Status status;
  size_t message_size;
  MPI_Get_count(&status, MPI_CHAR, (int*) &message_size);
  auto answer = (char*) malloc(message_size);
  MPI_Recv(answer, (int) message_size, MPI_CHAR, worker, BP_RESULT_TAG, MPI_COMM_WORLD, &status);

  // Write answer to output file
  pool.schedule([this, answer](){
    *output_stream << oslock << answer << endl << osunlock;
    free(answer);
  });
}

bool BatchProcessor::scheduling_completed() {
  lock_guard<mutex> lg(scheduling_complete_mutex);
  return schedulingComplete;
}

bool BatchProcessor::queue_empty() {
  lock_guard<mutex> lg(queue_mutex);
  return keys.empty();
}

// Sends the exit signal to a specified worker
void BatchProcessor::send_exit_signal(int worker) {
  MPI_Send(&worker, 1, MPI_BYTE, worker, BP_WORKER_EXIT_TAG, MPI_COMM_WORLD);
}

bool BatchProcessor::work_completed() {
  // todo: check that scheduling is completed, queue is empty and every worker is finished
  return false;
}

// Destructor
BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}