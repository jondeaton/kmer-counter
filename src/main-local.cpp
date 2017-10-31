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

#include "local-kmer-counter.hpp"

int main(int argc, const char* argv[]) {
  LocalKmerCounter counter(argc, argv);
  counter.run();
}