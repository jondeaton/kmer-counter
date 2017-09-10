/*
 * File: fasta-parser.h
 * --------------------
 * Presents the interface of the FastaParser class. This class is used to parse a fasta-formatted file or stream
 * by presenting an iterator that iterates through each record in the stream. Construct this parser by passing
 * a stream or path to a file, and then iterating through records as shown below. One parser should be created
 * for each stream/file to be created.
 *
 * Usage example:
 *
 * istream is("sequences.fasta");
 * FastaParser parser();
 * parser.parse(is);
 *
 * while (parser.hasNext()) {
 *  record = parser.next();
 *  string& header = record.first;
 *  string& sequence = record.second;
 * }
 */

#ifndef _fasta_parser_
#define _fasta_parser_

#include "fasta-iterator.h"
#include <cstring>
#include <iostream>


class FastaParser {

public:

  /**
   * Public Method: parse
   * --------------------
   * Creates a FastaIterator given a stream of fasta contents
   * @param fastaStream : A stream containing
   */
  void parse(std::istream& fastaStream);

  /**
   *
   * @return
   */
  std::pair<std::string, std::string> next();

  /**
   *
   * @return
   */
  bool hasNext();

  /**
   * Public method: parseHeader
   * --------------------------
   * Returns the ID contained in the fasta record header
   * @param header : A fasta record header
   * @return : The ID contained within the header
   */
  std::string parseHeader(const std::string& header);

private:
  std::istream* fastaStream = nullptr;
  std::string nextHeader;

  void readInRecord(std::string& target);
};
#endif