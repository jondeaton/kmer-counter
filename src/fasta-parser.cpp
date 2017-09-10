/*
 * File: fasta-parser.cpp
 * ----------------------
 * Presents the implementation of the FastaParser class.
 */

#include "fasta-parser.h"
using namespace std;

static bool startsWith(string& str, char character);

void FastaParser::parse(istream& fastaStream) {
  this->fastaStream = &fastaStream;

  string line;
  while(!fastaStream.eof()) {
    getline(fastaStream, line);
    if (startsWith(line, '>')) {
      nextHeader = line;
      break;
    }
  }
}

bool FastaParser::hasNext() { return fastaStream != nullptr && !fastaStream->eof(); }

pair<string, string> FastaParser::next() {
  pair<string, string> record;
  record.first = nextHeader;
  record.second = ""; // <-- perhaps unnecessary
  readInRecord(record.second);
  return record;
};

string FastaParser::parseHeader(const string& header) {
  return header; //todo
};


void FastaParser::readInRecord(string& target) {
  string line;
  while (getline(*fastaStream, line) && !startsWith(line, '>'))
    target += line; // todo: figure out how not to copy

  if (!fastaStream->eof()) nextHeader = line;
}

static bool startsWith(string& str, char character) {
  return str.c_str()[0] == character;
}