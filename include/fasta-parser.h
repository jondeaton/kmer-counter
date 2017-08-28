/*
 *
 */

#ifndef _fasta_parser_
#define _fasta_parser_

#include "fasta-iterator.h"
#include <cstring>
#include <iostream>

class FastaParser {

public:
  FastaParser();

  FastaIterator parse(std::iostream fastaStream);
  FastaIterator parse(const std::string& fastaFile);


};

#endif