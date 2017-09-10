/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class
 */

#include "fasta-parser.h"
using namespace std;

FastaIterator FastaParser::parse(istream& fastaStream) {
  FastaIterator it(&fastaStream);
  return it;
}

string FastaParser::parseHeader(const string& header) {
  return header;
}