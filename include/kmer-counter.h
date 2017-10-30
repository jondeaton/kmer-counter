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
#include <cstring>
#include <cstdio>
#include <map>

class KmerCounter {

public:


  /**
   * Constructor
   * -----------
   * Creates a KmerCounter object for k-mers of length kmerLength in sequences of the provided symbols
   * @param symbols: The symbols which are recognized. Order determined lexicographic ordering
   * @param kmerLength: The length of the window/word length to count in sequences
   */
  KmerCounter() = default;
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
   * Public Method: set_symbols
   * -------------------------
   * Sets the symbols that the k-mer counter uses in future calls to count
   * @param symbols: The symbols which are recognized. Order determined lexicographic ordering
   */
  void set_symbols(const std::string &symbols);

  /**
   * Public Method: set_kmer_length
   * ----------------------------
   * Sets the k-mer length that will be used for future calls to count
   * @param kmer_length: The length of the window/word length to count in sequences
   */
  void set_kmer_length(unsigned int kmer_length);

  /**
   * Public Method: get_vector_size
   * ------------------------------
   * Returns the size of the vector in which k-mer counts will be stored which is equal
   * to the number of unique k-mers of the given symbols and k-mer length
   */
  unsigned int get_vector_size() { return kmer_count_vector_size; }

private:
  std::string symbols;
  unsigned int num_symbols = 0;
  unsigned int kmer_length = 0;

  // The number of unique k-mers of the given symbols and k-mer length
  // kmer_count_vector_size = pow(num_symbols, kmer_length)
  unsigned int kmer_count_vector_size = 0;

  std::map<char, int> symbol_index_map;

  void populate_map();
  int calculate_index(const char *kmer, const unsigned int *significances, int index);
  unsigned int ipow(unsigned int base, unsigned int exp);
};

#endif