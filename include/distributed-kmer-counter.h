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
  void process(std::string directory, boost::regex pattern, std::string output);

private:

  BatchProcessor processor;
  AsyncKmerCounter counter;

  bool verbose;
  bool debug;

  int kmerLength;
  std::string symbols;
  bool sumFiles;

  std::string input_directory;
  std::string file_regex;
  boost::regex fileRegex;
  std::string output_file;

  void process(std::string& file);
  void getFiles(std::string directory, boost::regex pattern, std::queue<std::string> &fileQueue);
  void parse_command_line_options(int argc, const char* const* argv);
};

#endif