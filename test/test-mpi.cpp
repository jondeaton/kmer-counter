/*
 * File: test-mpi.cpp
 * ------------------
 * For testing the batch processor.
 *
 * To run:
 *  $(which mpirun) -n 2 --host node1,node2  ./test-mpi
 *
 */

#include "batch-processor.h"

int main(int argc, char* argv[]) {
  BatchProcessor bp(&argc, &argv);
  bp.SayHello();
  bp.run();
}