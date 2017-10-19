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
#include <iostream>
#include <boost/program_options.hpp>
namespace po = boost::program_options;
using namespace std;

#define K_DEFAULT 4

int main(int argc, char* argv[]) {
  DistributedKmerCounter counter(&argc, &argv);
  int k;
  po::options_description desc("Distributed k-mer counter options");
  desc.add_options()
    ("help", "Show help dialog")
    ("k", po::value<int>(&k)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")");

  

  po::positional_options_description p;
  p.add("input-directory", -1);

  po::variables_map vm;
  po::store(po::parse_command_line(argc, (const char * const *) argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) { // Display help page
    cout << desc << endl;
    return 1;
  }



}