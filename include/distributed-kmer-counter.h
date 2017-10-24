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
   * Runs the distributed k-mer counter for processing the files specified in the command line arguments.
   * This method will read all files matching the regular expression from the specified input directory,
   * count k-mers in them, and write the resulting counts to the output file.
   */
  void run();

private:

  ThreadPool pool;
  AsyncKmerCounter counter;
  BatchProcessor processor;

  bool verbose;
  bool debug;

  int kmer_length;
  std::string symbols;
  bool sum_files;

  std::string input_directory;
  boost::regex file_regex;
  std::string output_file;

  std::string count_kmers(const std::string &file);
  void schedule_files();

  void parse_CLI_options(int argc, const char *const *argv);
};

#endif