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

#include "distributed-kmer-counter.hpp"
using namespace std;

int main(int argc, char* argv[]) {
  DistributedKmerCounter counter(&argc, &argv);
  counter.run();
}
