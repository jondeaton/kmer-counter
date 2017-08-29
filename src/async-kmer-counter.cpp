/*
 * File: async-kmer-counter.cpp
 * ---------------------------
 * Presents the implementation of AsyncKmerCounter
 */
#include "async-kmer-counter.h"
#include "fasta-parser.h"
using namespace std;

#define NUMTHREADS 8

AsyncKmerCounter::AsyncKmerCounter(const std::string &symbols, unsigned int kmerLength, bool sumFiles):
  sumFiles(sumFiles), kmerCounter(symbols, kmerLength) {

//  for (int i = 0; i < NUMTHREADS; i++)
//    threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

}

void AsyncKmerCounter::count(istream& in, ostream& out) {

  // Gotta use heap because variable length array
  long* counts = new long[kmerCounter.kmerCountVectorSize];

  FastaParser parser;
  parser.parse(in);

  pair<string, string> record;
  while(parser.hasNext()) {
    record = parser.next();

    memset(counts, 0, sizeof(long));
    kmerCounter.count(record.second, counts);

    out << parser.parseHeader(record.first);
    for (int i = 0; i < kmerCounter.kmerCountVectorSize; i++)
      out << ", " << counts[i];
    out << endl;
  }

  delete[] counts;
}

void AsyncKmerCounter::countFastaFile(const std::string &fastaFile, const std::string &outfile) {}

void AsyncKmerCounter::countDirectory(const std::string &directory, std::ostream &out) {}

void AsyncKmerCounter::countAsync(istream& in, ostream& out) {
  count(in, out); // DEBUGGING
//  ioService.reset();
}

AsyncKmerCounter::~AsyncKmerCounter() {
//  threadpool.join_all(); // Destructor must destroy all of the threads in the threadpool
}
