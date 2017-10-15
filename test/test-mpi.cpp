/*
 * File: test-mpi.cpp
 * ------------------
 * For testing the batch processor
 */

#include "batch-processor.h"

int main(int argc, char* argv[]) {
  BatchProcessor bp(&argc, &argv);
  bp.SayHello();
}