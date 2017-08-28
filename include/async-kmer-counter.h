/*
 * File: async-kmer-counter.h
 * --------------------------
 * Presents the interface of AsyncKmerCounter, an asynchronous kmer counter
 */

#ifndef _async_kmer_counter_
#define _async_kmer_counter_

#include "fasta-parser.h"
#include "kmer-counter.h"
#include <iostream>
#include <cstring>

//#include <boost/asio/io_service.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>

class AsyncKmerCounter {

public:

  /**
   * Constructor: AsyncKmerCounter
   * -----------
   * Constructs the asynchronous kmer counter object
   * @param symbols
   * @param kmerLength
   * @param sumFiles
   */
  AsyncKmerCounter(const std::string& symbols, unsigned int kmerLength, bool sumFiles);

  /**
   *
   * @param fastaFile
   * @param outfile
   */
  void countFastaFile(const std::string& fastaFile, const std::string& outfile);

  /**
   *
   * @param directory
   * @param out
   */
  void countDirectory(const std::string& directory, std::ostream& out);

  /**
   *
   * @param in
   * @param out
   */
  void countAsync(std::istream& in, std::ostream& out);


  /**
   *
   * @param in
   * @param out
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
  FastaParser fastaParser;

//  boost::asio::io_service ioService;
//  boost::asio::io_service::work work;
//  boost::thread_group threadpool;

};
#endif