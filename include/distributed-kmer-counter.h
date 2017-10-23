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

  /**
   * Public method: run
   * ------------------
   * This method runs the distributed k-mer counter using the files
   * that
   */
  void run();

private:

  ThreadPool pool;
  AsyncKmerCounter counter;
  BatchProcessor processor;

  bool verbose;
  bool debug;

  int kmerLength;
  std::string symbols;
  bool sumFiles;

  std::string inputDirectory;
  boost::regex fileRegex;
  std::string outputFile;

  std::string process(const std::string& file);
  void parseCommandLineOptions(int argc, const char* const* argv);
};

#endif