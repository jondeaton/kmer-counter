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
#include "getopt.h"
#include <iostream>

const std::string symbols = "ATGC";
const unsigned int kmerLength = 4;

std::ifstream in;

int main(int argc, char* argv[]) {

  if (argc == 1) {
    // Read from stdin and print to stdout
    in = std::stdin;
  }
  else if (argc > 1) {

  }


  AsyncKmerCounter counter(symbols, kmerLength);
  counter.countFastaFile()


}