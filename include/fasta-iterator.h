/*
 * File: fasta-iterator.h
 *
 */

#ifndef _fasta_iterator_
#define _fasta_iterator_

#include <cstring>
#include <iostream>
#include "pair.h"

class FastaIterator {

public:

  FastaIterator(std::iostream in);
  FastaIterator(const std::string& fastaFile);

  pair<std::string, std::string> begin();
  pair<std::string, std::string> next();
  pair<std::string, std::string> end();

private:
  std::iostream in;

};

#endif