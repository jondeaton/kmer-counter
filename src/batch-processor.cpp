/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include <mpi.h>

#include <stdio.h>
#include <iostream>
#include <string>

#include <boost/regex.hpp>


#define MPI_WORKER_READY 0
#define MPI_WORK_TAG 1
#define HEAD_NODE 0

// Constructor
template <class keyT, dataT>
BatchProcessor<keyT, dataT>::BatchProcessor(int* argcp, char*** argvp) {
  MPI_Init(argcp, argvp);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(procName, &nameLength);
  processorName.assign(procName);
}


template <class keyT, dataT>
void BatchProcessor<keyT, dataT>::process(std::function<void(std::queue<keyT>&)> getKeys,
                                          std::function<dataT (keyT)> getData,
                                          std::function<void (dataT)> processData) {

  if (worldRank == HEAD_NODE) coordinateWorkers(getKeys);
  else doWork(std::function<dataT (keyT)> getData, std::function<void (dataT)> processData);
}

template <class keyT, dataT>
void BatchProcessor<keyT, dataT>::coordinateWorkers(std::function<void(std::queue<keyT>&)> getKeys) {

  getKeys(keys); // Fill the queue with keys

  MPI_Status status;
  char workerReady;

  // Dequeue keys until the work is all done
  while(!keys.empty()) {

    // Wait for worker to become available
    MPI_recv(&workerReady, sizeof(int), MPI_BYTE, MPI_ANY_SOURCE, MPI_WORKER_READY, MPI_COMM_WORLD, &status);

    if (!workerReady || status.MPI_ERROR) continue; // Worker didn't send correct signal

    // Send the next work to the worker
    keyT nextKey = keys.front();
    MPI_send(&nextKey, sizeof(nextKey), keyT, status.MPI_SOURCE, MPI_WORK_TAG, MPI_COMM_WORLD);

    keys.pop(); // Remove the key
  }
};

//
template <class keyT, dataT>
void BatchProcessor<keyT, dataT>::doWork(std::function<dataT (keyT)> getData,
                                         std::function<void (dataT)> processData) {

  while (true) {

    // Tell the head node we are ready
    int ready = 1;
    MPI_send(&ready, sizeof(int), MPI_BYTE, HEAD_NODE, MPI_WORKER_READY, MPI_COMM_WORLD);

    // Get the next key from the head node
    keyT nextKey;
    MPI_recv(&nextKey, sizeof(nextKey), keyT, HEAD_NODE, MPI_WORK_TAG, MPI_COMM_WORLD);

    dataT data = getData(nextKey); // Get the data
    processData(data);
  }
};

template <class keyT, dataT>
void BatchProcessor<keyT, dataT>::SayHello() {
  std::cout << "Hello from processor \"" << processorName << "\", rank: " << worldRank << "/" << worldSize << endl;
}

// Destructor
template <class keyT, dataT>
BatchProcessor<keyT, dataT>::~BatchProcessor() {
  MPI_Finalize();
}