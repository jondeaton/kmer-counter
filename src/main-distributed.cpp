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

  int i = 0;
  char hostname[256];
  gethostname(hostname, sizeof(hostname));
  printf("PID %d on %s ready for attach\n", getpid(), hostname);
  fflush(stdout);
  while (0 == i)
    sleep(5);

  DistributedKmerCounter counter(&argc, &argv);
  counter.run();
}
