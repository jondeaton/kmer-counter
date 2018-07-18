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
  cout << "Hello from processor " << processorName << ", rank " << worldRank << " of " << worldSize << endl;
}

void BatchProcessor::run() {
  if (worldRank == 0) {
    cout << "Head node sending message worker..." << endl;

    const char* message = "1234567890";
    MPI_Send(message, 10, MPI_CHAR, 1, 0, MPI_COMM_WORLD);

  } else {

    MPI_Status status;
    MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

    size_t size;
    MPI_Get_count(&status, MPI_CHAR, &size);

    cout << "Worker receiving a message..." << endl;

    char message[255];
    MPI_Recv(message, 255, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);

    cout << "I received: " << message << endl;
  }
}


void getFile() {

}


BatchProcessor::~BatchProcessor() {
  MPI_Finalize();
}