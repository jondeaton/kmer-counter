/**
 * File: kmer-counter.cpp
 * ----------------------
 * Presents the implementation of the k-mer counter class. This file implements the logic
 * of exact counting of k-mers in a sequence.
 */

#include "kmer-counter.hpp"
using namespace std;

KmerCounter::KmerCounter(const string& symbols, const unsigned int kmerLength) :
  symbols(symbols), num_symbols((unsigned int) symbols.length()), kmer_length(kmerLength) {
  kmer_count_vector_size = ipow(kmerLength, num_symbols);
  populate_map();
}

// Here be performance optimizations
void KmerCounter::count(const std::string& sequence, long kmerCount[]) {
  if (kmer_length == 0) return;

  auto sequenceLength = (unsigned int) sequence.length();
  if (sequenceLength < kmer_length) return;

  auto numSymbols = (unsigned int) symbols.length();
  if (numSymbols == 0) return;

  // Stores the lexicographic significance of each letter in a kmer
  auto significances = new unsigned int[kmer_length + 1];
  for (unsigned int i = 0; i <= kmer_length; i++) significances[i] = ipow(numSymbols, i);

  // index is the lexicographic index in the kmerCount array corresponding
  // to the k-mer under the sliding window. -1 indicates that there is no index
  // stored in this variable from the kmer under the previous window
  int index = -1;

  // Slide a window of size kmer_length along the sequence
  size_t maximumIndex = sequenceLength - kmer_length;
  for (size_t i = 0; i <= maximumIndex; i++) {
    const char* kmer = sequence.c_str() + i; // slide the window
    index = calculate_index(kmer, significances, index);
    if (index >= 0) kmerCount[index] += 1; // Valid k-mer encountered
    // else i -= (index + 1); // Invalid character encountered. Advance window past it.
  }
}

int KmerCounter::calculate_index(const char *kmer, const unsigned int *significances, int index) {
  if (index < 0) { // Must recalculate
    index = 0;
    for (unsigned int j = 0; j < kmer_length; j++) {
      if (symbol_index_map.find(kmer[j]) == symbol_index_map.end()) return -(j + 1); // invalid next symbol
      index += symbol_index_map[kmer[j]] * significances[kmer_length - j - 1];
    }
  } else { // May use previous window's index to make a quicker calculation
    if (symbol_index_map.find(kmer[kmer_length - 1]) == symbol_index_map.end()) return -kmer_length;
    // index = (index * num_symbols) % significances[kmer_length] + symbol_index_map[letter];
    index = ((index % significances[kmer_length - 1]) * num_symbols) + symbol_index_map[kmer[kmer_length - 1]];
  }
  return index;
}

void KmerCounter::set_symbols(const std::string &symbols) {
  this->symbols = symbols;
  num_symbols = (unsigned int) symbols.length();
  populate_map();
}

void KmerCounter::set_kmer_length(unsigned int kmer_length) {
  this->kmer_length = kmer_length;
  kmer_count_vector_size = ipow(kmer_length, num_symbols);
}

/**
 * Private method: populate_map
 * ----------------------------
 * Populates the map that maps symbol to lexicographic index. This method should
 * only be called after symbols has been initialized.
 */
void KmerCounter::populate_map() {
  symbol_index_map.clear();
  for (unsigned int i = 0; i < num_symbols; i++) {
    symbol_index_map[symbols[i]] = i;
    symbol_index_map[tolower(symbols[i])] = i;
  }
}

// Integer exponentiation
unsigned int KmerCounter::ipow(unsigned int base, unsigned int exp) {
  if (base == 0 || base == 1) return base;

  unsigned int result = 1;
  while (exp) {
    if (exp & 1) result *= base;
    exp >>= 1;
    base *= base;
  }
  return result;
}