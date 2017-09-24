/*
 * File: fasta-iterator.cpp
 * ------------------------
 * Defines the implementation of the FastaIterator class
 */

#include "fasta-iterator.h"
using namespace std;

// Static function declarations
static inline bool startsWith(string& str, char character);

FastaIterator::FastaIterator(istream* in) : haveNextHeader(false) {
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
 * Check this out: https://stackoverflow.com/questions/24851291/read-huge-text-file-line-by-line-in-c-with-buffering
 */
FastaIterator& FastaIterator::operator++ () {
  haveNextHeader = findNextHeader();
  if (!haveNextHeader) record = nullptr;
  else {
    record = shared_ptr<pair<string, ostringstream>>(new pair<string, ostringstream>());
    record->first = nextHeader;

    string line;
    while (!in->eof()) {
      getline(*in, line);
      if (!startsWith(line, '>')) record->second << line;
      else {
        haveNextHeader = true;
        nextHeader = line;
        break;
      }
    }
    if (in->eof()) haveNextHeader = false;
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

//Finds the next header in the stream, and stores it in nextHeader
bool FastaIterator::findNextHeader() {
  if (haveNextHeader) return true;
  while (getline(*in, nextHeader))
    if (startsWith(nextHeader, '>')) return true;
  return false;
}

/**
 * Function: startsWith
 * --------------------
 * Determines if a C++ string starts with a certain character
 * @param str: The string to check the first character of
 * @param character: The character to check
 * @return: True if the first character in the passed string starts with the passed character
 */
static inline bool startsWith(string& str, char character) {
  return str.c_str()[0] == character;
}
