/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include "batch-processor.h"

#include <mpi.h>
#include <iostream>
#include <boost/regex.hpp>

using namespace std;

#define BP_WORKER_READY_TAG 0
#define BP_HEAD_NODE 0

#define BP_WORKER_READY 1

#define BP_WORK_TAG 1
#define BP_WORKER_EXIT_TAG 42

// Constructor
BatchProcessor::BatchProcessor(int* argcp, char*** argvp) {
  int error = MPI_Init(argcp, argvp);
  if (error) {
    cerr << "Error: " << error << " initalizing MPI." << endl;
    exit(error);
  }

  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(procName, &nameLength);
  processorName.assign(procName);
}

// Process function
void BatchProcessor::process(function<void(queue<string>&)> getKeys, function<void (string&)> processData) {
  if (worldRank == BP_HEAD_NODE) coordinateWorkers(getKeys);
  else doWork(processData);
}

/**
 * Private method: coordinateWorkers
 * --------------------------------
 * Method used by the "head" node to coordinate work on all of the worker nodes.
 * This method will first call the get keys method which was passed, and fill the queue
 * of keys in the batch processor with
 * @param getKeys: Function for the head node to get all of the keys to be processed
 */
void BatchProcessor::coordinateWorkers(function<void(queue<string>&)> getKeys) {

  std::queue<std::string> keys; // Allocate queue of keys to be processed
  getKeys(keys); // Fill the queue with keys

  MPI_Status status;
  char workerReady;

  // Dequeue keys until the work is all done
  while(!keys.empty()) {

    // Wait for worker to become available
    MPI_Recv(&workerReady, sizeof(int), MPI_BYTE, MPI_ANY_SOURCE, BP_WORKER_READY_TAG, MPI_COMM_WORLD, &status);

    if (!workerReady || status.MPI_ERROR) continue; // Worker didn't send correct signal

    // Send the next work to the worker
    string nextKey = keys.front();
    MPI_Send(nextKey.c_str(), nextKey.size(), MPI_CHAR, status.MPI_SOURCE, BP_WORK_TAG, MPI_COMM_WORLD);
    keys.pop(); // Remove the key
  }

  cout << "Signaling workers to exit..." << endl;
  for (int i = 0; i < worldSize; i++) {
    if (i != BP_HEAD_NODE) sendExitSignal(i);
  }
  cout << "Head \"" << processorName << "\" exiting.";
};

/**
 * Private method: doWork
 * ----------------------
 *
 * @param processData
 */
void BatchProcessor::doWork(function<void (std::string&)> processData) {
  size_t numProcessed = 0; // Worker keeps track of how many it's processed

  MPI_Status status;
  int ready = 1;

  size_t messageSize;
  std::string nextKey; // Stores the next key

  while (true) {

    // Tell the head node we are ready
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

    processData(nextKey);
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