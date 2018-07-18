/**
 * File: kmer-counter.cpp
 * ----------------------
 * Presents the implementation of the k-mer counter class. This file implements the logic
 * of exact counting of k-mers in a sequence.
 */

#include "kmer-counter.h"
using namespace std;

KmerCounter::KmerCounter(const string& symbols, const unsigned int kmerLength) :
  symbols(symbols), numSymbols((unsigned int) symbols.length()), kmerLength(kmerLength) {
  kmerCountVectorSize = ipow(kmerLength, numSymbols);
  populateMap();
}

// Here be performance optimizations
void KmerCounter::count(const std::string& sequence, long kmerCount[]) {
  if (kmerLength == 0) return;

  auto sequenceLength = (unsigned int) sequence.length();
  if (sequenceLength < kmerLength) return;

  auto numSymbols = (unsigned int) symbols.length();
  if (numSymbols == 0) return;

  // Stores the lexocographic significance of each letter in a kmer
  auto significances = new unsigned int[kmerLength + 1];
  for (unsigned int i = 0; i <= kmerLength; i++) significances[i] = ipow(numSymbols, i);

  // index is the lexicographic index in the kmerCount array corresponding
  // to the kmer under the sliding window. -1 indicates that there is no index
  // stored in this variable from the kmer under the previous window
  int index = -1;

  // Slide a window of size kmerLength along the sequence
  size_t maximumIndex = sequenceLength - kmerLength;
  for (size_t i = 0; i <= maximumIndex; i++) {
    const char* kmer = sequence.c_str() + i; // slide the window
    index = calculateIndex(kmer, significances, index);
    if (index >= 0) kmerCount[index] += 1; // Valid kmer encountered
    // else i -= (index + 1); // Invalid character encountered. Advance window past it.
  }
}

int KmerCounter::calculateIndex(const char *kmer, const unsigned int *significances, int index) {
  if (index < 0) { // Must recalculate
    index = 0;
    for (unsigned int j = 0; j < kmerLength; j++) {
      if (symbolIndexMap.find(kmer[j]) == symbolIndexMap.end()) return -(j + 1); // invalid next symbol
      index += symbolIndexMap[kmer[j]] * significances[kmerLength - j - 1];
    }
  } else { // May use previous window's index to make a quicker calculation
    if (symbolIndexMap.find(kmer[kmerLength - 1]) == symbolIndexMap.end()) return -kmerLength;
    // index = (index * numSymbols) % significances[kmerLength] + symbolIndexMap[letter];
    index = ((index % significances[kmerLength - 1]) * numSymbols) + symbolIndexMap[kmer[kmerLength - 1]];
  }
  return index;
}

void KmerCounter::populateMap() {
  for (unsigned int i = 0; i < numSymbols; i++) {
    symbolIndexMap[symbols[i]] = i;
    symbolIndexMap[tolower(symbols[i])] = i;
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