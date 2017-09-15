/*
 * File: fasta-parser.h
 * --------------------
 * Presents the interface of the FastaParser class. This class is used to parse a fasta-formatted file or stream
 * by presenting an iterator that iterates through each record in the stream. Construct this parser by passing
 * a stream or path to a file, and then iterating through records as shown below. One parser should be created
 * for each stream/file to be created.
 *
 * Usage with iterator:
 *
 * istream is("sequences.fasta");
 * FastaParser parser(is);
 *
 * for (auto it = parser.begin(); it != parser.end(); it++) {
 *  auto record = it*;
 *  string& header = record.first;
 *  string& sequence = record.second;
 * }
 *
 * You can also parser fasta headers like so:
 *
 * parser.parseHeader("> Fasta header");
 *
 */

#ifndef _fasta_parser_
#define _fasta_parser_

#include "fasta-iterator.h"
#include <cstring>
#include <iostream>
#include <memory>

class FastaParser {

public:

  /**
   * Constructor: FastaParser
   * ------------------------
   * For creating a fasta parser for parsing from a fasta stream
   * @param in: A stream from which to read fasta formatted records
   */
  explicit FastaParser(std::istream* in);

  /**
   * Constructor: FastaParser
   * ------------------------
   * For creating a fasta parser for parsing from a fasta formatted file
   * @param fastaFile: A path to a fasta formatted file
   */
  explicit FastaParser(const std::string& fastaFile);

  /**
   * Public method: begin
   * --------------------
   * Gives an iterator pointing at a parsed record
   * @return: A FastaIterator object which points to the first parsed record in the stream
   */
  FastaIterator begin();

  /**
   * Public method: end
   * ------------------
   * Gives an iterator pointing to the end of the records for testing if you have
   * iterated through all of the records.
   * @return: A FastaIterator object representing the end of the records
   */
  FastaIterator end();

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
  FastaIterator endIterator;
};
#endif
