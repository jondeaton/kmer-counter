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
  fastaParser(){
//
//  for (int i = 0; i < NUMTHREADS; i++)
//    threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

}

void AsyncKmerCounter::count(istream& in, ostream& out) {

  FastaIterator it(&in);
  auto counts = new long[kmerCounter.kmerCountVectorSize];

  pair<string, string> record;
  for (it.begin(record); it.end ; it.next(record)) {

    counts = {0}; // Set elements of array to zero
    kmerCounter.count(record.second, counts);

    // Output to file
    out << fastaParser.parseHeader(record.first);
    for (int i = 0; i < kmerCounter.kmerCountVectorSize; i++)
      out << ", " << counts[i];
    out << endl;
  }
}


void AsyncKmerCounter::countAsync(istream& in, ostream& out) {

//  ioService.reset();
}

AsyncKmerCounter::~AsyncKmerCounter() {
  // Destructor must destroy all of the threads in the threadpool
//  threadpool.join_all();
}
