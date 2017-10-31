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

#include <unistd.h>

int main(int argc, char* argv[]) {
  cout << "Starting program..." << endl;
  usleep(3000);
  DistributedKmerCounter counter(&argc, &argv);
  counter.run();
}
