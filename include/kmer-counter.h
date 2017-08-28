/**
 * File: kmer-counter.h
 * --------------------
 * Presents the inferface of KmerCounter, a class that will count k-mers
 * in sequences for arbitrary symbol sets and k-mer lengths.
 */

#ifndef _kmer_counter_
#define _kmer_counter_

#include <string>
#include <cstdio>
#include <map>

class KmerCounter {

public:

  unsigned int kmerCountVectorSize;

  /**
   * Constructor
   * -----------
   *
   * @param symbols
   * @param kmerLength
   */
  KmerCounter(const std::string& symbols, unsigned int kmerLength);

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