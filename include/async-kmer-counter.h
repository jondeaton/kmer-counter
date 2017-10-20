/*
 * File: async-kmer-counter.h
 * --------------------------
 * Presents the interface of the AsyncKmerCounter class, an asynchronous k-mer counter.
 */

#ifndef _async_kmer_counter_
#define _async_kmer_counter_

#include "kmer-counter.h"
#include "fasta-parser.h"
#include "boost-thread-pool.h"
#include <iostream>
#include <cstring>

class AsyncKmerCounter {

public:

  /**
   * Constructor
   * -----------
   * Constructs an asynchronous k-mer counting object for counting k-mers in sequences with
   * the specified k-mer length and symbol set.
   * @param symbols: A string with the lexicographic ordering of the symbols in the sequences
   * @param kmerLength: The length of the sliding window ("k" in "k-mer")
   * @param sumFiles:True if all k-mer counts in each file should be summed together
   */
  AsyncKmerCounter();
  AsyncKmerCounter(const std::string& symbols, unsigned int kmerLength);
  AsyncKmerCounter(const std::string& symbols, unsigned int kmerLength, bool sumFiles);

  /**
   * Public Method: count
   * --------------------
   * Count k-mers from a fasta formatted stream either sequentially or asynchronously
   * @param in: Stream to read fasta-formatted sequences from
   * @param out: Stream to output k-mer counts to
   * @param sequential: If true, k-mers will be counted in order, if false, multi-threading will be used
   */
  void count(std::istream& in, std::ostream& out, bool sequential, bool block);

  /**
   * Public Method: countSequential
   * --------------------
   * Sequentially counts the k-mers from the in stream and outputs them
   * to the out stream in their origin all order
   * @param in: Stream to read fasta records in from
   * @param out: Stream to output k-mer counts to
   */
  void countSequential(std::istream& in, std::ostream& out);

  /**
   * Public Method: countAsync
   * -------------------------
   * Asynchronously counts the k-mers in the instream, sending the results
   * to the out stream not necessarily in the original order
   * @param in: Stream to read fasta records in from
   * @param out: Stream to output k-mer counts to
   */
  void countAsync(std::istream& in, std::ostream& out, bool block);

  /**
   * Public Method: countFastaFile
   * -----------------------------
   * Counts the fasta file and prints the output to the provided outfile
   * @param fastaFile : Path to fasta file to count k-mers in
   * @param out: Output stream to output k-mer counts to
   */
  void countFastaFile(const std::string& fastaFile, std::ostream& out, bool sequential, bool block);

  /**
   * Public Method: countDirectory
   * -----------------------------
   * Count fasta files in a stream
   * @param directory: Directory to read fasta files from
   * @param out: Output stream to output k-mer counts to
   */
  void countDirectory(const std::string& directory, std::ostream& out, bool sequential, bool block);

  void setSumFiles() { this->sumFiles = sumFiles; }
  void setSymbols(const std::string& symbols) { kmerCounter.setSymbols(symbols); }
  void setSetKmerLength(unsigned int kmerLength) { kmerCounter.setKmerLength(kmerLength); }

  /**
   * Destructor
   * ----------
   * For disposal of the asynchronous k-mer counter. This will dispose of the threads in the thread pool
   */
  ~AsyncKmerCounter();

private:

  bool sumFiles; // True if all k-mer counts in each file should be summed together
  KmerCounter kmerCounter;
  ThreadPool pool;
};
#endif
