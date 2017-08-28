#include "fasta-iterator.h"
#include <boost/algorithm/string/predicate.hpp>
using namespace std;

FastaIterator::FastaIterator(std::ifstream& in) : in(in) {}

FastaIterator::FastaIterator(const string &fastaFile) {
  ifstream file(fastaFile);
  in = file;
}

pair<string, string> FastaIterator::begin() {
  pair<string, string> record;

  string line;

  // Search for first record
  while (getline(in, line)) {
    if (line == nullptr) return nullptr;

    if (boost::starts_with(line, '>')) {
      record.first = line; // found it
      break;
    }
  }

  // Read in sequence until the next record
  readInRecord(record.second);
  return record;
};

pair<string, string> FastaIterator::next() {
  if (next_header == nullptr) return nullptr;

  pair<string, string> record;
  record.first = next_header;
  readInRecord(record.second);
  return record;
};

void FastaIterator::readInRecord(string& target) {
  string line;
  while (getline(in, line)) {

    // Check if ran into the next record
    if (boost::starts_with(line, '>')) {
      next_header = line;
      break;
    }
    target += line; // todo: figure out hot to not copy!
  }
}