/*
 * File: fasta-iterator.h
 * ----------------------
 * Presents the implementation of the FastaIterator class. This class is used to parse fasta records out
 * of a fasta file or stream by iterating through them easily in a for loop.
 *
 * Usage example:
 *
 * istream is("sequences.fasta");
 * FastaParser parser();
 * parser.parse(is);
 *
 * for (FastaIterator it = parser.begin(); it != parser.end(); it++) {
 *  string& header = it->first;
 *  string& sequence = it->second;
 *  // etc...
 * }
 */

#ifndef _fasta_iterator_
#define _fasta_iterator_

#include <string>
#include <fstream>
#include <memory>

class FastaIterator {

public:

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object that is prepared to parse fasta records from the passed stream.
   * @param in : Stream from which to read and parse fasta records
   */
  explicit FastaIterator(std::istream* in);

  /**
   * Dereference operator*
   * --------------------
   * For getting the contents that the iterator is pointing to
   * @return: A reference to the pair that the iteraotor "poitns" to
   */
  std::pair<std::string, std::string>& operator* ();

  /**
   * Dereference operator->
   * --------------------
   * For getting the contents that the iterator is pointing to
   * @return: Returns a raw pointer to the pair that the iterator "points" to
   */
  std::pair<std::string, std::string>* operator-> ();

  /**
   * Prefix operator
   * ---------------
   * For incrementing the iterator to the next record
   * @return: Reference to
   */
  FastaIterator& operator++ ();

  /**
   * PostFix operator
   * ----------------
   *
   * @return
   */
  FastaIterator operator++ (int);

  /**
   * Equality operator ==
   * ----------------------
   *
   * @param other
   * @return
   */
  bool operator == (const FastaIterator& other);

  /**
   * Inequality operator !=
   * ----------------------
   * @param other
   * @return
   */
  bool operator != (const FastaIterator& other);

private:
  std::istream* in; // The stream to read fasta records from
  bool haveNextHeader; // True is nextHeader contains the next header
  std::string nextHeader; // The next header in the records

  std::shared_ptr<std::pair<std::string, std::string>> record;

  bool findNextHeader();

};

#endif
