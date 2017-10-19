/*
 * File: distributed-kmer-counter.h
 * --------------------------------
 * Presents the interface of a distributed k-mer counter
 */

#ifndef _dist_kmer_counter_
#define _dist_kmer_counter_

#include "batch-processor.h"
#include <boost/regex.hpp>


class DistributedKmerCounter {

public:

  int k;

  DistributedKmerCounter(int* argcp, char*** argvp);
  void process(std::string directory, boost::regex pattern, std::string output);

private:


  void getKeys(std::queue<std::string>& fileQueue);
  void process(std::string& file);

  BatchProcessor processor;
};

#endif