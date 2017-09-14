/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class.
 */

#include "fasta-parser.h"
using namespace std;

FastaParser::FastaParser(istream* in) : fastaStream(in) {};

FastaParser::FastaParser(const std::string& fastaFile) {
  this->fastaStream = new ifstream(fastaFile);
}

string FastaParser::parseHeader(const string& header) {
  return header;
};

FastaIterator FastaParser::begin() {
  return FastaIterator(fastaStream);
}

FastaIterator FastaParser::end() {
  return nullptr;
}