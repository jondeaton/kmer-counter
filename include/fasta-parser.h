/*
 * File: fasta-parser.h
 * --------------------
 * Presents the interface of the FastaParser class
 */

#ifndef _fasta_parser_
#define _fasta_parser_

#include "fasta-iterator.h"
#include <cstring>
#include <iostream>

class FastaParser {

public:

  /**
   * Public method: parse
   * --------------------
   * Creates a FastaIterator given a stream of fasta contents
   * @param fastaStream : A stream containing
   * @return : A FastaIterator object around the stream
   */
  FastaIterator parse(std::istream& fastaStream);

  /**
   * Public method: parseHeader
   * --------------------------
   * Returns the ID contained in the fasta record header
   * @param header : A fasta record header
   * @return : The ID contained within the header
   */
  std::string parseHeader(const std::string& header);
};

#endif