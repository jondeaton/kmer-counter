/*
 * File: async-kmer-counter.cpp
 * ---------------------------
 * Presents the implementation of AsyncKmerCounter
 */
#include "async-kmer-counter.h"
using namespace std;

#define NUMTHREADS 8

AsyncKmerCounter::AsyncKmerCounter(const std::string &symbols, unsigned int kmerLength, bool sumFiles):
  sumFiles(sumFiles), kmerCounter(symbols, kmerLength),
  fastaParser(), work(ioService) , threadpool() {

  for (int i = 0; i < NUMTHREADS; i++)
    threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

}

void AsyncKmerCounter::count(const std::iostream in, const std::iostream out) {

  FastaIterator it = fastaParser.parse(in);

  

  pair<string, string> record;
  for (record = it.begin(); record != it.end(); record = it.next()) {
    kmerCounter.count(record.second, )

  }

}


void AsyncKmerCounter::countAsync(const std::iostream in, const std::iostream out) {



//  ioService.reset();
}

AsyncKmerCounter::~AsyncKmerCounter() {
  // Destructor must destroy all of the threads in the threadpool
  threadpool.join_all();
}
