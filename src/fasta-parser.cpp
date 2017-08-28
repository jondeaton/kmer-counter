/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class
 */

#include "fasta-parser.h"
using namespace std;

FastaIterator FastaParser::parse(ifstream fastaStream) {
  FastaIterator it(fastaStream);
  return it;
}

FastaIterator FastaParser::parse(const string& fastaFile) {
  FastaIterator it(fastaFile);
  return it;
}

string FastaParser::parseHeader(const string header) {
  return header;
};
