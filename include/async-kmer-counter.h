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
   * the specified k-mer length and symbol set. Note: Using the constructor that only takes
   * the ThreadPool means that you must call set_symbols, and set_kmer_length before calling
   * any "count" variant.
   * @param symbols: A string with the lexicographic ordering of the symbols in the sequences
   * @param kmer_length: The length of the sliding window ("k" in "k-mer")
   * @param sumFiles:True if all k-mer counts in each file should be summed together
   */
  explicit AsyncKmerCounter(ThreadPool& pool);
  AsyncKmerCounter(ThreadPool& pool, const std::string& symbols, unsigned int kmer_length, bool sumFiles=false);

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
   * Public Method: count_sequential
   * -------------------------------
   * Sequentially counts the k-mers from the in stream and outputs them
   * to the out stream in their origin all order
   * @param in: Stream to read fasta records in from
   * @param out: Stream to output k-mer counts to
   */
  void count_sequential(std::istream &in, std::ostream &out);

  /**
   * Public Method: count_async
   * --------------------------
   * Asynchronously counts the k-mers in the instream, sending the results
   * to the out stream not necessarily in the original order
   * @param in: Stream to read fasta records in from
   * @param out: Stream to output k-mer counts to
   */
  void count_async(std::istream &in, std::ostream &out, bool block);

  /**
   * Public Method: count_fasta_file
   * -------------------------------
   * Counts the fasta file and prints the output to the provided outfile
   * @param fastaFile : Path to fasta file to count k-mers in
   * @param out: Output stream to output k-mer counts to
   */
  void count_fasta_file(const std::string &fastaFile, std::ostream &out, bool sequential, bool block);

  /**
   * Public Method: count_directory
   * ------------------------------
   * Count fasta files in a stream
   * @param directory: Directory to read fasta files from
   * @param out: Output stream to output k-mer counts to
   */
  void count_directory(const std::string &directory, std::ostream &out, bool sequential, bool block);

  /**
   * Public method: set_fum_files
   * --------------------------
   * Set the sum-files parameter of the k-mer counter.
   * @param sum_files: If set to true then counts from all sequences
   * located in a single files will be summed together. If false, then the k-mer will output a seperate
   * count entry for each sequence in the input file(s)
   */
  void set_fum_files(bool sum_files) { this->sum_files = sum_files; }

  /**
   * Public method: set_symbols
   * --------------------------
   * Set the symbols of the k-mer counter
   * @param symbols: The new symbols to use for counting
   */
  void set_symbols(const std::string &symbols) { kmer_counter.set_symbols(symbols); }

  /**
   * Public method: set_kmer_length
   * ------------------------------
   * Set the k-mer length (i.e. "k" in k-mer) of the k-mer counter.
   * @param kmer_length: The new k
   */
  void set_kmer_length(unsigned int kmer_length) { kmer_counter.set_kmer_length(kmer_length); }

  /**
   * Destructor
   * ----------
   * For disposal of the asynchronous k-mer counter. This will dispose of the threads in the thread pool
   */
  ~AsyncKmerCounter();

private:

  bool sum_files; // True if all k-mer counts in each file are be summed together
  KmerCounter kmer_counter;
  ThreadPool& pool;
};
#endif
