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

using namespace std;

#define BP_HEAD_NODE 0
#define BP_WORKER_READY 1

#define BP_WORK_TAG 1
#define BP_WORKER_READY_TAG 0
#define BP_RESULT_TAG 1337
#define BP_WORKER_EXIT_TAG 42

BatchProcessor::BatchProcessor(int* argcp, char*** argvp, ThreadPool& pool) : pool(pool) {
  int error = MPI_Init(argcp, argvp);
  if (error) {
    cerr << "Error: " << error << " initalizing MPI." << endl;
    exit(error);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(procName, &nameLength);
  processorName.assign(procName); // store processor name for later
  if (worldRank == BP_HEAD_NODE) masterRoutine();
  else workerRoutine();
}

void BatchProcessor::schedule(std::string key) {
  if (worldRank != BP_HEAD_NODE) return;
  keys.push(key);
}

/**
 * Private method: masterRoutine
 * -----------------------------
 * Method used by the "head" node to coordinate work on all of the worker nodes.
 * This method will first call the get keys method which was passed, and fill the queue
 * of keys in the batch processor with
 */
void BatchProcessor::masterRoutine() {

  MPI_Status status;
  char workerReady;

  // Dequeue keys until the work is all done
  while (true) {

    // Find out what a worker wants to do next
    MPI_Probe(BP_HEAD_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == BP_RESULT_TAG) { // Worker is returning result
      receiveAndProcessResult(status.MPI_SOURCE);
      continue;
    } else if (status.MPI_TAG == BP_WORKER_READY_TAG) {
      MPI_Recv(&workerReady, sizeof(int), MPI_BYTE, status.MPI_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);
      if (keys.empty()) sendExitSignal(status.MPI_SOURCE);

    } else continue; // Something weird happened

    if (!workerReady || status.MPI_ERROR) continue; // Worker didn't send correct signal

    // Send the next work to the worker
    string nextKey = keys.front();
    MPI_Send(nextKey.c_str(), (int) nextKey.size(), MPI_CHAR, status.MPI_SOURCE, BP_WORK_TAG, MPI_COMM_WORLD);
    keys.pop(); // Remove the key
  }

  cout << "Signaling workers to exit..." << endl;
  for (int i = 0; i < worldSize; i++) {
    if (i != BP_HEAD_NODE) sendExitSignal(i);
  }
  cout << "Head \"" << processorName << "\" exiting.";
};


/**
 * Private method: receiveAndProcessResult
 * ---------------------------------------
 * Receives an answer from a worker node and write it to the output stream asynchronously
 */
void BatchProcessor::receiveAndProcessResult(int worker) {

  MPI_Status status;
  size_t messageSize;
  MPI_Get_count(&status, MPI_CHAR, (int*) &messageSize);
  auto answer = (char*) malloc(messageSize);
  MPI_Recv(answer, (int) messageSize, MPI_CHAR, worker, BP_RESULT_TAG, MPI_COMM_WORLD, &status);

  // Write answer to output file
  pool.schedule([](){
    outputStream << oslock << answer << endl << osunlock;
    free(answer);
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
void BatchProcessor::workerRoutine(function<string (string&)> processKey) {
  size_t numProcessed = 0; // Worker keeps track of how many it's processed

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
      cout << "Worker \"" << processorName << "\" exiting (rank " << worldRank << "/" << worldSize << ") ";
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
    string result = processKey(nextKey);
    MPI_Send(result.c_str(), result.size(), MPI_CHAR, BP_HEAD_NODE, BP_RESULT_TAG, MPI_COMM_WORLD);

    numProcessed++;
  }
};

// Sends the exit signal to a specified worker
void BatchProcessor::sendExitSignal(int worker) {
  MPI_Send(&worker, 1, MPI_BYTE, worker, BP_WORKER_EXIT_TAG, MPI_COMM_WORLD);
}

// Destructor
BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}