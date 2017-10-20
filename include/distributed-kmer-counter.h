/*
 * File: distributed-kmer-counter.h
 * --------------------------------
 * Presents the interface of a distributed k-mer counter
 */

#ifndef _dist_kmer_counter_
#define _dist_kmer_counter_

#include "batch-processor.h"
#include "async-kmer-counter.h"
#include <boost/regex.hpp>

class DistributedKmerCounter {

public:

  int k;

  DistributedKmerCounter(int* argcp, char*** argvp);
  void process(std::string directory, boost::regex pattern, std::string output);

private:

  void process(std::string& file);
  void getFiles(std::string directory, boost::regex pattern, std::queue<std::string> &fileQueue);

  BatchProcessor& processor;
  AsyncKmerCounter counter;
};

#endif