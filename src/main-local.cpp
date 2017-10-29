/*
 * File: main-local.cpp
 * --------------------
 * Program entry point for regualr k-mer counter command line tool.
 *
 *
 * Usage:
 *
 *  ./count-kmers < sequences.fasta > kmer_counts.kmer
 *
 *  ./count-kmers directory_of_fastas > kmer_counts.kmer
 *
 *
 * Command line options
 *
 *  -k=4
 *    The size of the kmers to count
 *
 *  --symbols=ATGC
 *    The symbols to use to count kmers
 *
 *  --sum-fasta
 *    Will sum all of the k-mer counts from a single file into one k-mer count
 *
 */

#include "async-kmer-counter.h"
#include "boost-thread-pool.h"
#include <boost/filesystem.hpp>
#include <cstring>
#include <iostream>

const std::string symbols = "ATGC";
unsigned int kmer_length = 4;
bool sequential = false;
bool sum_files = false;

static void printUsage();
static int parseCommandLineOptions(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  int numFlags = parseCommandLineOptions(argc, argv);

  ThreadPool pool;
  AsyncKmerCounter counter(pool, symbols, kmer_length, sum_files);

  if (argc > 1 + numFlags) {
    std::string path(argv[1 + numFlags]);
    if (boost::filesystem::is_regular_file(path))
      counter.count_fasta_file(path, std::cout, sequential, true);
    else if (boost::filesystem::is_directory(path))
      counter.count_directory(path, std::cout, sequential, true);
  } else counter.count(std::cin, std::cout, sequential, true);
}

/**
 * Function: printUsage
 * --------------------
 * Prints command line usage instructions for this
 * utility. For use in the case where the user has specified
 * the --help flag or has specified flags incorrectly.
 */
static void printUsage() {
  std::cerr << "usage:\n\t./count-kmers < sequences.fasta > kmer_counts.kmer" << std::endl;
}

/**
 * Function: parseCommandLineOptions
 * ---------------------------------
 * Parses the command line options from the command line arguments
 * and sets global variables to reflect these options
 * @param argc : Number of command line options including script name
 * @param argv : NULL terminated list of arguments
 * @return: The number of flags
 */
static int parseCommandLineOptions(int argc, char* argv[]) {
  int numFlags = 0;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--sequential") == 0) {
      sequential = true;
      numFlags++;
    }

    if (strcmp(argv[i], "--help") == 0) {
      printUsage();
      exit(0);
    }
  }
  return numFlags;
}
