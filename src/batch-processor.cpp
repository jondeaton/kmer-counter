/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include "batch-processor.h"

#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <boost/regex.hpp>

using namespace std;

#define MPI_WORKER_READY_TAG 0
#define HEAD_NODE 0

#define MPI_WORKER_READY 1

#define MPI_WORK_TAG 1
#define WORKER_EXIT_TAG 42

// Constructor
BatchProcessor::BatchProcessor(int* argcp, char*** argvp) : numProcessed(0) {
  MPI_Init(argcp, argvp);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(procName, &nameLength);
  processorName.assign(procName);
}

// Process function
void BatchProcessor::process(function<void(queue<string>&)> getKeys, function<void (string&)> processData) {
  if (worldRank == HEAD_NODE) coordinateWorkers(getKeys);
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

  getKeys(keys); // Fill the queue with keys

  MPI_Status status;
  char workerReady;

  // Dequeue keys until the work is all done
  while(!keys.empty()) {

    // Wait for worker to become available
    MPI_Recv(&workerReady, sizeof(int), MPI_BYTE, MPI_ANY_SOURCE, MPI_WORKER_READY_TAG, MPI_COMM_WORLD, &status);

    if (!workerReady || status.MPI_ERROR) continue; // Worker didn't send correct signal

    // Send the next work to the worker
    string nextKey = keys.front();
    MPI_Send(nextKey.c_str(), nextKey.size(), MPI_CHAR, status.MPI_SOURCE, MPI_WORK_TAG, MPI_COMM_WORLD);
    keys.pop(); // Remove the key
  }

  cout << "Instructing workers to exit..." << endl;
  for (int i = 1; i < worldSize; i++) sendExitSignal(i);
  cout << "Head \"" << processorName << "\" exiting.";
};

/**
   * Public method: doWork
   * @param processData
   */
void BatchProcessor::doWork(function<void (std::string&)> processData) {
  MPI_Status status;

  while (true) {
    // Tell the head node we are ready
    int ready = 1;
    MPI_Send(&ready, sizeof(int), MPI_BYTE, HEAD_NODE, MPI_WORKER_READY_TAG, MPI_COMM_WORLD);

    // Get the next key from the head node
    std::string nextKey;
    MPI_Recv(&nextKey, sizeof(nextKey), MPI_CHAR, HEAD_NODE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    if (status.MPI_TAG == WORKER_EXIT_TAG) { // Instructed by master to exit
      cout << "Worker \"" << processorName << "\" exiting (rank " << worldRank << "/" << worldSize << ") ";
      cout << "Processed: " << numProcessed << endl;
      break;
    }

    processData(nextKey);
    numProcessed++;
  }
};

// Sends the exit signal to a specified worker
void BatchProcessor::sendExitSignal(int worker) {
  MPI_Send(&worker, 1, MPI_BYTE, worker, WORKER_EXIT_TAG, MPI_COMM_WORLD);
}

// Destructor
BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}