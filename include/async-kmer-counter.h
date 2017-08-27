/*
 *
 */

#ifndef _async_kmer_counter_
#define _async_kmer_counter_

#include "fasta-parser.h"
#include "kmer-counter.h"
#include <iostream>

class AsyncKmerCounter {

public:
  AsyncKmerCounter(const std::string& symbols, const unsigned int kmerLength);

  countFastaFile(const std::string& fastaFile, const std::string& outfile);
  countDirectory(const std::string^ directory, const std::string& outfile);
  countAsync(const iostream in, const iostream out);

private:
  KmerConter kmerConter;
  FastaParser fastaParser;

  bool sum_file;

};
#endif