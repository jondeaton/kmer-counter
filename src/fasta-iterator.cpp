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
}


FastaIterator& FastaIterator::operator++ () {

  record = new pair<string, string>();

  string line;
  while (getline(*in, line) && !startsWith(line, '>'))
    record.first = line;

  while (!in->eof()) {
    getline(*in, line);
    record.second += line;
  }

  return *this;
}

FastaIterator FastaIterator::operator++ (int) {
  FastaIterator result(*this); // make a copy for result
  ++(*this); // Now use the prefix version to do the work
  return result; // return the copy (the old) value.
}

T FastaIterator::operator->() {

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