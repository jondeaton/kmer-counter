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

FastaIterator::FastaIterator(const string &fastaFile) : end(false) {
  this->in = new ifstream(fastaFile);
}

void FastaIterator::begin(pair<string, string>& record) {
  if (in->eof()) return;

  string line; // Each line of the file will be stored here

  // Loop through lines until the first record is found
  while (getline(*in, line) && !startsWith(line, '>'))
      record.first = line;

  readInRecord(record.second); // Read in sequence until the next record
}

void FastaIterator::next(pair<string, string>& record) {
  if (in->eof()) return;
  record.first = next_header; // Store the header
  readInRecord(record.second);
}

/**
   * Private method: readInRecord
   * ----------------------------
   * Parses lines from a file into the provided string,
   * Reads in lines from the ifstream into the target string provided.
   * @param sequence : A string to put the sequence into
   */
void FastaIterator::readInRecord(std::string& sequence) {
  if (in->eof()) return;

  sequence.clear();
  string line;
  while (getline(*in, line) && !startsWith(line, '>'))
    sequence += line; // todo: figure out how not to copy

  if (!in->eof()) next_header = line; // Encountered next record
}

bool FastaIterator::endOfRecords(){
  return in->eof();
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