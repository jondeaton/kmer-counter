/**
 * File: kmer-counter.h
 * --------------------
 * Presents the interface of a thread-safe KmerCounter. Objects of this class
 * will count k-mers in sequences for arbitrary symbol sets and k-mer lengths
 * in a thread-safe manner.
 */

#ifndef _kmer_counter_
#define _kmer_counter_

#include <string>
#include <cstdio>
#include <map>

class KmerCounter {

public:

  /**
   * Constructor
   * -----------
   * Creates a KmerCounter object. Calls to count after using this consructor without first
   * setting symbols and  k-mer length results in undefined behavior.
   */
  KmerCounter() = default;

  /**
   * Constructor
   * -----------
   * Creates a KmerCounter object for k-mers of length kmerLength in sequences of the provided symbols
   * @param symbols: The symbols which are recognized. Order determined lexicographic ordering
   * @param kmerLength: The length of the window/word length to count in sequences
   */
  KmerCounter(const std::string& symbols, unsigned int kmerLength);

  /**
   * Public Method: count
   * --------------------
   * Count k-mers in a sequence, writing results to the array passed as the second parameter
   * @param sequence: Sequence of symbols to count k-mers in
   * @return: Nothing. Result is stored in the array passed as the second parameter
   */
  void count(const std::string& sequence, long kmerCount[]);

  /**
   * Public Method: setSymbols
   * -------------------------
   * Sets the symbols that the k-mer counter uses in future calls to count
   * @param symbols: The symbols which are recognized. Order determined lexicographic ordering
   */
  void setSymbols(const std::string& symbols);

  /**
   * Public Method: setKmerLength
   * ----------------------------
   * Sets the k-mer length that will be used for future calls to count
   * @param kmerLength: The length of the window/word length to count in sequences
   */
  void setKmerLength(unsigned int kmerLength);

  /**
   * Public Method: GetVectorSize
   * ----------------------------
   * Returns the size of the vector in which k-mer counts will be stored which is equal
   * to the number of unique k-mers of the given symbols and k-mer length
   */
  unsigned int GetVectorSize() { return kmerCountVectorSize; }

private:
  std::string symbols;
  unsigned int numSymbols = 0;
  unsigned int kmerLength = 0;
  unsigned int kmerCountVectorSize = 0; // The number of unique k-mers of the given symbols and k-mer length

  std::map<char, int> symbolIndexMap;

  void populateMap();
  int calculateIndex(const char* kmer, const unsigned int* significances, int index);
  unsigned int ipow(unsigned int base, unsigned int exp);
};

#endif