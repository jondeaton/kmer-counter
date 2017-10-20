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

  DistributedKmerCounter(int* argcp, char*** argvp);
  void run();

private:

  BatchProcessor processor;
  AsyncKmerCounter counter; // not every node needs one of these... move this...

  bool verbose;
  bool debug;

  int kmerLength;
  std::string symbols;
  bool sumFiles;

  std::string inputDirectory;
  boost::regex fileRegex;
  std::string outputFile;

  void process(const std::string& file);
  void getFiles(std::string directory, boost::regex pattern, std::queue<std::string> &fileQueue);
  void parseCommandLineOptions(int argc, const char* const* argv);
};

#endif