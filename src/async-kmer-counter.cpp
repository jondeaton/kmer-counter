/*
 * File: async-kmer-counter.cpp
 * ---------------------------
 * Presents the implementation of AsyncKmerCounter
 */

#include "async-kmer-counter.h"
#include "ostreamlock.h"
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
using namespace std;

AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool) : sumFiles(false), pool(pool) { }

AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool, const std::string &symbols, unsigned int kmerLength) :
  sumFiles(false), kmerCounter(symbols, kmerLength), pool(pool) { }

AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool, const std::string &symbols, unsigned int kmerLength, bool sumFiles):
  sumFiles(sumFiles), kmerCounter(symbols, kmerLength), pool(pool) { }

void AsyncKmerCounter::count(istream& in, ostream& out, bool sequential, bool block) {
  if (sequential) countSequential(in, out);
  else countAsync(in, out, block);
}

void AsyncKmerCounter::countSequential(istream& in, ostream& out) {

  // Gotta use heap because variable length array
  auto counts = new long[kmerCounter.GetVectorSize()];

  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    memset(counts, 0, sizeof(long) * kmerCounter.GetVectorSize());
    kmerCounter.count(it->second.str(), counts);

    // Output to file
    out << parser.parseHeader(it->first);
    for (size_t i = 0; i < kmerCounter.GetVectorSize(); i++)
      out << ", " << counts[i];
    out << endl;
  }
  delete[] counts;
}

// Asynchronous counting
void AsyncKmerCounter::countAsync(istream& in, ostream& out, bool block) {
  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    shared_ptr<pair<string, ostringstream>> record = *it;

    pool.schedule([&, record] () {
      auto counts = new long[kmerCounter.kmerCountVectorSize];
      memset(counts, 0, sizeof(long) * kmerCounter.kmerCountVectorSize);
      kmerCounter.count(record->second.str(), counts);

      out << oslock << parser.parseHeader(record->first);
      for (size_t i = 0; i < kmerCounter.kmerCountVectorSize; i++) out << ", " << counts[i];
      out << endl << osunlock;
      delete[] counts;
    });
  }
  if (block) pool.wait();
}

void AsyncKmerCounter::countFastaFile(const string &fastaFile, ostream &out, bool sequential, bool block) {
  if (!boost::filesystem::exists(fastaFile)) return; // File not found
  ifstream is(fastaFile);
  count(is, out, sequential, block);
}

void AsyncKmerCounter::countDirectory(const string &directory, ostream &out, bool sequential, bool block) {
  if (!boost::filesystem::exists(directory)) return;

  boost::filesystem::directory_iterator end;
  for (boost::filesystem::directory_iterator it(directory); it != end; ++it) {
    if (boost::filesystem::is_regular_file(it->path())) {
      string fileName = it->path().generic_string();
      if (sequential) countFastaFile(fileName, out, sequential, true);
      else pool.schedule([&, fileName] () {
        countFastaFile(fileName, out, true, false);
      });
    }
  }
  if (!sequential && block) pool.wait();
}

AsyncKmerCounter::~AsyncKmerCounter() {
  (void) sumFiles; // just to get the compiler to chill
}