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
  fastaParser() {

//  for (int i = 0; i < NUMTHREADS; i++)
//    threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

}

void AsyncKmerCounter::count(istream& in, ostream& out) {

  FastaIterator it(&in);

  // Gotta use heap because variable length array
  auto counts = new long[kmerCounter.kmerCountVectorSize];

  pair<string, string> record;
  for (it.begin(record); !it.endOfRecords(); it.next(record)) {
    memset(counts, 0, sizeof(long));
    kmerCounter.count(record.second, counts);

    // Output to file
    out << fastaParser.parseHeader(record.first);
    for (size_t i = 0; i < kmerCounter.kmerCountVectorSize; i++)
      out << ", " << counts[i];
    out << endl;
  }

  delete(counts);
}

void AsyncKmerCounter::countFastaFile(const std::string &fastaFile, const std::string &outfile) {
  (void) fastaFile; // todo: remove these and implement
  (void) outfile;
}

void AsyncKmerCounter::countDirectory(const std::string &directory, std::ostream &out) {
  (void) directory; // todo: remove these and implement
  (void) out;
}

void AsyncKmerCounter::countAsync(istream& in, ostream& out) {
  // todo: actually implement this lol
  count(in, out);
//  ioService.reset();
}

AsyncKmerCounter::~AsyncKmerCounter() {
//  threadpool.join_all(); // Destructor must destroy all of the threads in the threadpool
}
