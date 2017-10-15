/*
 * File: batch-processor.cpp
 * -------------------------
 * Presents the implementation of the batch processor
 */

#include <mpi.h>
#include <batch-processor.h>
#include <stdio.h>
using namespace std;

BatchProcessor::BatchProcessor(int* argcp, char*** argvp) {
  MPI_Init(argcp, argvp);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);

  char procName[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name(processor_name, &nameLength);
  processorName(procName);
}

void BatchProcessor::SayHello() {
  cout << "Hello from processor " << processorName << ", rank " << worldRank << " of " << worldSize;
}

BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}