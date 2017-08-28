/*
 * File: fasta-iterator.cpp
 * ------------------------
 * Defines the implementation of the FastaIterator class
 */

#include "fasta-iterator.h"
using namespace std;

bool startsWith(string& str, char character);

FastaIterator::FastaIterator(istream* in) : end(false) {
  this->in = in;
}

FastaIterator::FastaIterator(const string &fastaFile) : end(false) {
  this->in = new ifstream(fastaFile);
}

void FastaIterator::begin(pair<string, string>& record) {
  string line;

  bool found = true;
  // Search for first record
  while (getline(*in, line) && startsWith(line, '>'))
      record.first = line;

  end = in->eof(); // Check if the file was found
  if (!end) readInRecord(record.second); // Read in sequence until the next record
};

void FastaIterator::next(pair<string, string>& record) {
  if (end) return;

  record.first = next_header;
  record.second = "";
  readInRecord(record.second);
};

void FastaIterator::readInRecord(string& target) {
  string line;
  while (getline(*in, line) && !startsWith(line, '>'))
    target += line; // todo: figure out how not to copy

  if (in->eof()) end = true; // end of file
  else next_header = line; // ran into the next record
}

bool startsWith(string& str, char character) {
  return str.c_str()[0] == character;
}