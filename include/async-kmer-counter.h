/*
 * File: async-kmer-counter.h
 * --------------------------
 * Presents the interface of AsyncKmerCounter, an asynchronous kmer counter
 */

#ifndef _async_kmer_counter_
#define _async_kmer_counter_

#include "kmer-counter.h"
#include "fasta-parser.h"
#include <iostream>
#include <cstring>

//#include <boost/asio/io_service.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>

class AsyncKmerCounter {

public:

  /**
   * Constructor: AsyncKmerCounter
   * -----------------------------
   * Constructs the asynchronous kmer counter object
   * @param symbols
   * @param kmerLength
   * @param sumFiles
   */
  AsyncKmerCounter(const std::string& symbols, unsigned int kmerLength, bool sumFiles);

  /**
   * Public Method: countFastaFile
   * -----------------------------
   * Counts the fasta file and prints the output to the provided outfile
   * @param fastaFile : Path to fasta file to count k-mers in
   * @param outfile : Path to file to write k-mer coutns to
   */
  void countFastaFile(const std::string& fastaFile, const std::string& outfile);

  /**
   * Public Method: countDirectory
   * -----------------------------
   *
   * @param directory
   * @param out
   */
  void countDirectory(const std::string& directory, std::ostream& out);

  /**
   * Public Method: countAsync
   * -------------------------
   * Asynchronously counts the k-mers in the instream, sending the results
   * to the out stream not necessarily in the original order
   * @param in : Stream to read fasta records in from
   * @param out : Stream to outptu k-mer coutns to
   */
  void countAsync(std::istream& in, std::ostream& out);


  /**
   * Public Method: count
   * --------------------
   * Sequentially counts the k-mers from the in stream and outputs them
   * to the out stream in their origin all order
   * @param in : Stream to read fasta records in from
   * @param out : Stream to outptu k-mer coutns to
   */
  void count(std::istream& in, std::ostream& out);

  /**
   * Destructor: AsyncKmerCounter
   * ----------------------------
   * For disposal of the asynchronous kmer counter. This will join
   * all of the threads in the thread pool
   */
  ~AsyncKmerCounter();

private:
  bool sumFiles;
  std::string directory;

  KmerCounter kmerCounter;

//  boost::asio::io_service ioService;
//  boost::asio::io_service::work work;
//  boost::thread_group threadpool;

};
#endif