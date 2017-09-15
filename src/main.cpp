/*
 * Command line tool for counting k-mers
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

const std::string symbols = "ATGC";
unsigned int kmerLength = 4;
bool sumFiles = false;
bool fromstdin = true;
std::string directory;

static void printUsage();
static void parseCommandLineOptions(int argc, char* argv[]);

int main(int argc, char* argv[]) {
  parseCommandLineOptions(argc, argv);

  AsyncKmerCounter counter(symbols, kmerLength, sumFiles);

  if (fromstdin) counter.countAsync(std::cin, std::cout);
  else counter.countDirectory(directory, std::cout);
}

/**
 * Function: printUsage
 * --------------------
 * Prints command line usage instructions for this
 * utility. For use in the case where the user has specified
 * the --help flag or has specified flags incorrectly.
 */
static void printUsage(){
  std::cerr << "usage:\n\t./count-kmers < sequences.fasta > kmer_counts.kmer" << std::endl;
}

/**
 * Function: parseCommandLineOptions
 * ---------------------------------
 * Parses the command line options from the command line arguments
 * and sets global variables to reflect these options
 * @param argc : Number of command line options including script name
 * @param argv : NULL terminated list of arguments
 */
static void parseCommandLineOptions(int argc, char* argv[]) {
  // todo
  (void) argc;
  (void) argv;
  if (false) printUsage();
}