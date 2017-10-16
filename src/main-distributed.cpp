/*
 * File: main-distributed.cpp
 * --------------------------
 * Program entry point for the distributed k-mer counter.
 *
 * Usage
 *
 * mpirun count-kmers
 *
 */

#include "distributed-kmer-counter.h"

int main(int argc, char* argv[]) {
  DistributedKmerCounter counter(&argc, &argv);

}