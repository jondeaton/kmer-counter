/*
 * File: async-kmer-counter.cpp
 * ---------------------------
 * Presents the implementation of AsyncKmerCounter
 */
#include "async-kmer-counter.h"
#include "fasta-parser.h"
#include "ostreamlock.h"
using namespace std;

#define NUMTHREADS 8

AsyncKmerCounter::AsyncKmerCounter(const std::string &symbols, unsigned int kmerLength, bool sumFiles):
  sumFiles(sumFiles), kmerCounter(symbols, kmerLength), pool(NUMTHREADS) { }

void AsyncKmerCounter::count(istream& in, ostream& out) {

  // Gotta use heap because variable length array
  auto counts = new long[kmerCounter.kmerCountVectorSize];

  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    auto record = *it;
    memset(counts, 0, sizeof(long));
    kmerCounter.count(record->second, counts);

    // Output to file
    out << parser.parseHeader(record->first);
    for (size_t i = 0; i < kmerCounter.kmerCountVectorSize; i++)
      out << ", " << counts[i];
    out << endl;
  }

  delete[] counts;
}

// Asynchronous counting
void AsyncKmerCounter::countAsync(istream& in, ostream& out) {
  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    auto record = *it;

    pool.schedule([&] () {
      auto counts = new long[kmerCounter.kmerCountVectorSize];
      memset(counts, 0, sizeof(long));
      kmerCounter.count(record->second, counts);

      out << oslock << parser.parseHeader(record->first);
      for (size_t i = 0; i < kmerCounter.kmerCountVectorSize; i++) out << ", " << counts[i];
      out << endl << osunlock;
      delete[] counts;
    });
  }
  pool.wait();
}

void AsyncKmerCounter::countFastaFile(const std::string &fastaFile, const std::string &outfile) {
  (void) fastaFile; // todo: remove these and implement
  (void) outfile;
}

void AsyncKmerCounter::countDirectory(const std::string &directory, std::ostream &out) {
  (void) directory; // todo: remove these and implement
  (void) out;
  (void) sumFiles;
}

AsyncKmerCounter::~AsyncKmerCounter() { }
