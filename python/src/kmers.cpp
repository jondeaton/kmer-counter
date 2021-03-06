#include "kmers.h"
#include <cstdio>
#include <map>
#include <string.h>
using namespace std;

// Static function declarations
static void populateMap(map<char, int>& symbolIndexMap, const char* symbols);
static unsigned int calculateIndex(const char* kmer, const size_t kmerLength, map<char, int>& symbolIndexMap,
                          const int* significances, const unsigned int numSymbols, int index);
static unsigned int ipow(unsigned int base, unsigned int exp);

/**
 * Function: countKMers
 * --------------------
 * This function encodes the logic for counting k-mers. It will count the k-mers
 * in the provided sequence and store them in memory in the location given by
 * the kmerCount array.
 * @param sequence: The sequence to count k-mers in
 * @param kmerLength: The length of the k-mer (the value of k)
 * @param symbols: The symbols to sort
 * @param kmerCount: Reference to array to store k-mer counts
 * @return
 */
int countKMers(const char* sequence, const int kmerLength, const char* symbols, long* kmerCount) {
  if (kmerLength == 0) return 0;

  size_t sequenceLength = strlen(sequence);
  if (sequenceLength < kmerLength) return 0;

  size_t numSymbols = strlen(symbols);
  if (numSymbols == 0) return 0;

  // Stores mapping from symbol to lexicographic index
  map<char, int> symbolIndexMap;
  populateMap(symbolIndexMap, symbols);

  // Stores the lexocographic significance of each letter in a kmer
  unsigned int* significances = new unsigned int[kmerLength + 1];
  for (size_t i = 0; i <= kmerLength; i++) significances[i] = ipow(numSymbols, i);

  // index is the lexicographic index in the kmerCount array corresponding
  // to the kmer under the sliding window. -1 indicates that there is no index
  // stored in this variable from the kmer under the previous window
  int index = -1;

  // Slide a window of size kmerLength along the sequence
  size_t maximumIndex = sequenceLength - kmerLength;
  for (int i = 0; i <= maximumIndex; i++) {
    const char* kmer = sequence + i; // slide the window
    index = calculateIndex(kmer, kmerLength, symbolIndexMap, significances, numSymbols, index);
    if (index >= 0) kmerCount[index] += 1; // Valid kmer encountered
    //else i -= (index + 1); // Invalid character encountered. Advance window past it.
  }
  return 0;
}

/**
 * Function: populateMap
 * ---------------------
 * Populates a map of
 * @param symbolIndexMap: The map to populate
 * @param symbols: The symbols to populate it with
 */
static void populateMap(map<char, int>& symbolIndexMap, const char* symbols) {
  int numSymbols = strlen(symbols);
  for (int i = 0; i < numSymbols; i++) {
    symbolIndexMap[symbols[i]] = i;
    symbolIndexMap[tolower(symbols[i])] = i;
  }
}

/**
 * Function: calculateIndex
 * ------------------------
 * This function calculates the index of the kmer in the kmer array where its stored
 * @param kmer
 * @param kmerLength
 * @param symbolIndexMap
 * @param significances
 * @param numSymbols
 * @param index
 * @return
 */
static unsigned int calculateIndex(const char* kmer, const unsigned int kmerLength, map<char, unsigned int>& symbolIndexMap,
                          const int* significances, const int numSymbols, int index) {
  if (index < 0) {
    // Must recalculate
    // index = sum([lookup[kmer[n]] * pow(num_symbols, kmer_length - n - 1) for n in xrange(kmer_length)])
    index = 0;
    for (int j = 0; j < kmerLength; j++) {
      // char letter = kmer[j];
      if (symbolIndexMap.find(kmer[j]) == symbolIndexMap.end()) return -(j + 1); // invalid next symbol
      index += symbolIndexMap[kmer[j]] * significances[kmerLength - j - 1];
    }
  } else {
    // May use previous window's index to make a quicker calculation
    // index = (index * num_symbols) % pow(num_symbols, kmer_length) + lookup[sequence[i + kmer_length - 1]]
    // char letter = kmer[kmerLength - 1];
    if (symbolIndexMap.find(kmer[kmerLength - 1]) == symbolIndexMap.end()) return -kmerLength;
    // index = (index * numSymbols) % significances[kmerLength] + symbolIndexMap[letter];
    index = ((index % significances[kmerLength - 1]) * numSymbols) + symbolIndexMap[kmer[kmerLength - 1]];
  }
  return index;
}

/**
 * Function: ipow
 * --------------
 * Integer exponentiation
 * @param base
 * @param exp
 * @return
 */
static unsigned int ipow(unsigned int base, unsigned int exp) {
  if (base == 0 || base == 1) return base;

  int result = 1;
  while (exp) {
    if (exp & 1) result *= base;
    exp >>= 1;
    base *= base;
  }
  return result;
}
