/*
 * File: fasta-iterator.cpp
 * ------------------------
 * Defines the implementation of the FastaIterator class
 */

#include "fasta-iterator.h"
#include <boost/algorithm/string/predicate.hpp>
using namespace std;

FastaIterator::FastaIterator(istream* in) : have_next_header(false) {
  if (in == nullptr) record = nullptr;
  else {
    this->in = in;
    ++(*this); // On construction, the iterator should already have parsed the first record
  }
}

/*
 * Basically just reads in from the stream until the next record or the
 * end of the stream is encountered. If the end of the stream is encountered
 * then the record is set to be a null pointer.
 *
 * Check this out for more optimizations:
 * https://stackoverflow.com/questions/24851291/read-huge-text-file-line-by-line-in-c-with-buffering
 */
FastaIterator& FastaIterator::operator++ () {
  have_next_header = find_next_header();
  if (!have_next_header) record = nullptr;
  else {
    record = shared_ptr<pair<string, ostringstream>>(new pair<string, ostringstream>());
    record->first = nextHeader;

    string line;
    while (!in->eof()) {
      getline(*in, line);
      if (!boost::starts_with(line, ">")) record->second << line;
      else {
        have_next_header = true;
        nextHeader = line;
        break;
      }
    }
    if (in->eof()) have_next_header = false;
  }
  return *this;
}

shared_ptr<pair<string, ostringstream>> FastaIterator::operator*() {
  return record;
}

shared_ptr<pair<string, ostringstream>> FastaIterator::operator-> () {
  return record;
}


FastaIterator FastaIterator::operator++ (int) {
  FastaIterator result(*this); // make a copy for result
  ++(*this); // Now use the prefix version to do the work
  return result; // return the copy (the old) value.
}

bool FastaIterator::operator == (const FastaIterator& other) {
  return record == other.record;
}

bool FastaIterator::operator != (const FastaIterator& other) {
  return !this->operator==(other);
}

// Finds the next header in the stream, and stores it in nextHeader
bool FastaIterator::find_next_header() {
  if (have_next_header) return true;
  while (getline(*in, nextHeader))
    if (boost::starts_with(nextHeader, ">")) return true;
  return false;
}