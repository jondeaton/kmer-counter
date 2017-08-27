/**
 * File: kmer-counter.h
 * --------------------
 * This file defines the KmerCounter class which is for counring k-mers.
 */

#ifndef _kmer_counter_
#define _kmer_counter_

#include <cstdio>
#include <map>
#include <cstring>

class KmerCounter {

public:

  /**
   * Constructor
   * -----------
   *
   * @param symbols
   * @param kmerLength
   */
  KmerCounter(const std::string& symbols, const unsigned int kmerLength);

  /**
   * Public Method: count
   * --------------------
   *
   * @param sequence
   * @return
   */
  void count(const std::string& sequence, long kmerCount[]);

private:
  std::string symbols;
  unsigned int numSymbols;
  unsigned int kmerLength;
  std::map<char, int> symbolIndexMap;

  unsigned int calculateIndex(const char* kmer, const unsigned int* significances, int index);
  void populateMap();

  /**
   *
   * @param base
   * @param exp
   * @return
   */
  unsigned int ipow(unsigned int base, unsigned int exp);
};
#endif