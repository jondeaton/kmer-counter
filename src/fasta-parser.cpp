/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class.
 */

#include "fasta-parser.hpp"
using namespace std;

FastaParser::FastaParser(istream* in) : fasta_stream(in), endit(nullptr) {}

FastaParser::FastaParser(const std::string& fasta_file) : endit(nullptr) {
  this->fasta_stream = new ifstream(fasta_file);
}

string FastaParser::parse_header(const string &header) {
  return header;
}

FastaIterator FastaParser::begin() {
  return FastaIterator(fasta_stream);
}

FastaIterator FastaParser::end() {
  return endit;
}
