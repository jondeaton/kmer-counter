/*
 * File: async-kmer-counter.cpp
 * ---------------------------
 * Presents the implementation of AsyncKmerCounter
 */

#include "async-kmer-counter.h"
#include "ostreamlock.h"
#include <boost/filesystem.hpp>
using namespace std;


AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool) : pool(pool), sum_files(false) { }

AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool, const string &symbols, unsigned int kmer_length) :
  kmer_counter(symbols, kmer_length), pool(pool), sum_files(false) { }

AsyncKmerCounter::AsyncKmerCounter(ThreadPool& pool, const string &symbols, unsigned int kmer_length, bool sum_files) :
  kmer_counter(symbols, kmer_length), pool(pool), sum_files(sum_files) { }

void AsyncKmerCounter::count(istream& in, ostream& out, bool sequential, bool block) {
  if (sequential) count_sequential(in, out);
  else count_async(in, out, block);
}

void AsyncKmerCounter::count_sequential(istream &in, ostream &out) {

  // Gotta use heap because variable length array
  auto counts = new long[kmer_counter.get_vector_size()];

  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    memset(counts, 0, sizeof(long) * kmer_counter.get_vector_size());
    kmer_counter.count(it->second.str(), counts);

    // Output to file
    out << parser.parse_header(it->first);
    for (size_t i = 0; i < kmer_counter.get_vector_size(); i++)
      out << ", " << counts[i];
    out << endl;
  }
  delete[] counts;
}

// Asynchronous counting
void AsyncKmerCounter::count_async(istream &in, ostream &out, bool block) {
  FastaParser parser(&in);
  for (auto it = parser.begin(); it != parser.end(); ++it) {
    shared_ptr<pair<string, ostringstream>> record = *it;

    pool.schedule([&, record] () {
      auto counts = new long[kmer_counter.get_vector_size()];
      memset(counts, 0, sizeof(long) * kmer_counter.get_vector_size());
      kmer_counter.count(record->second.str(), counts);

      out << oslock << parser.parse_header(record->first);
      for (size_t i = 0; i < kmer_counter.get_vector_size(); i++) out << ", " << counts[i];
      out << endl << osunlock;
      delete[] counts;
    });
  }
  if (block) pool.wait();
}

void AsyncKmerCounter::count_fasta_file(const string &fastaFile, ostream &out, bool sequential, bool block) {
  if (!boost::filesystem::exists(fastaFile)) return; // File not found
  ifstream is(fastaFile);
  count(is, out, sequential, block);
}

void AsyncKmerCounter::count_directory(const string &directory, ostream &out, bool sequential, bool block) {
  if (!boost::filesystem::exists(directory)) return;

  boost::filesystem::directory_iterator end;
  for (boost::filesystem::directory_iterator it(directory); it != end; ++it) {
    if (boost::filesystem::is_regular_file(it->path())) {
      string file_name = it->path().generic_string();
      if (sequential) count_fasta_file(file_name, out, sequential, true);
      else pool.schedule([&, file_name] () {
          count_fasta_file(file_name, out, true, false);
      });
    }
  }
  if (!sequential && block) pool.wait();
}

AsyncKmerCounter::~AsyncKmerCounter() {
  (void) sum_files; // just to get the compiler to chill tf out
}