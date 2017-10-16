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

int main(int argc, const char* const* argv) {

  po::options_description desc("Distributed k-mer counter options");
  desc.add_options()
    ("help", "Show help dialog")
    ("k", po::value<int>()->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")");


  po::positional_options_description p;
  p.add("input-directory", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return 1;
  }

  if (vm.count("compression")) {
    cout << "Compression level was set to "
         << vm["compression"].as<int>() << ".\n";
  } else {
    cout << "Compression level was not set.\n";
  }


  DistributedKmerCounter counter(&argc, &argv);
}