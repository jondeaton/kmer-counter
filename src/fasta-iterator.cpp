/*
 * File: fasta-iterator.cpp
 * ------------------------
 * Defines the implementation of the FastaIterator class
 */

#include "fasta-iterator.h"
using namespace std;

// Static function declarations
static inline bool startsWith(string& str, char character);

FastaIterator::FastaIterator(istream* in) : end(false) {
  this->in = in;
  // On construction, the iterator should already have
  // parsed the first record, which is why we increment here
  ++(*this);
}

pair<string, string>& FastaIterator::operator*() {
  return *record;
}

pair<string, string>* FastaIterator::operator-> () {
  return &(*record);
}

/*
 * Basically just reads in from the stream until the next record or the
 * end of the stream is encountered. If the end of the stream is encountered
 * then the record is set to be a null pointer.
 */
FastaIterator& FastaIterator::operator++ () {
  
  findNextHeader();

  record = shared_ptr<pair<string, string>>(new pair<string, string>());
  
  if (in->eof()) record = shared_ptr<pair<string, string>>(nullptr);
  else {
    while (!in->eof()) {
      getline(*in, line);
      if (starts


      record->second += line;
    }
  }
  return *this;
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

/**
 * Function: findNextHeader
 * ------------------------
 *  Finds the next
 *
 */
void FastaIterator::findNextHeader() {
  if (nextHeader != nullptr) return;

  string line;
  while (getline(*in, line)) {
      if (startsWith(line, '>')) {
          nextHeader = line;
            break;
        }
  }
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
