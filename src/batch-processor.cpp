/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include "batch-processor.h"
#include <mpi.h>
#include <stdio.h>
#include <iostream>

using namespace std;


BatchProcessor::BatchProcessor(int* argcp, char*** argvp) {

  // Initializes MPI and gets world size and rank within world
  MPI_Init(argcp, argvp);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  // Get information about processor name
  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(procName, &nameLength);
  processorName.assign(procName, nameLength);
}

void BatchProcessor::SayHello() {
  cout << "Hello from processor " << processorName << ", rank " << worldRank << " of " << worldSize;
}

BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}