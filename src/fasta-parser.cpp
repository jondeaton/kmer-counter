/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class.
 */

#include "fasta-parser.h"
using namespace std;

FastaParser::FastaParser(istream* in) : fastaStream(in), endIterator(nullptr) {};

FastaParser::FastaParser(const std::string& fastaFile) : endIterator(nullptr) {
  this->fastaStream = new ifstream(fastaFile);
}

string FastaParser::parseHeader(const string& header) {
  return header;
};

FastaIterator FastaParser::begin() {
  return FastaIterator(fastaStream);
}

FastaIterator FastaParser::end() {
  return endIterator;
}