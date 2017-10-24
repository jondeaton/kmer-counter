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
 * for (FastaIterator it = parser.begin(); it != parser.end(); ++it) {
 *  string& header = it->first;
 *  string& sequence = it->second;
 *  // analysis, etc...
 * }
 */

#ifndef _fasta_iterator_
#define _fasta_iterator_

#include <string>
#include <fstream>
#include <memory>
#include <sstream>

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
   * @return: A copy of a shared_ptr to a record pair
   */
  std::shared_ptr<std::pair<std::string, std::ostringstream>> operator* ();

  /**
   * Dereference operator->
   * --------------------
   * For getting the contents that the iterator is pointing to
   * @return: A copy of a shared_ptr to a record pair
   */
  std::shared_ptr<std::pair<std::string, std::ostringstream>> operator-> ();

  /**
   * Prefix operator
   * ---------------
   * For incrementing the iterator to the next record
   * @return: Reference to this FastaIterator
   */
  FastaIterator& operator++ ();

  /**
   * PostFix operator
   * ----------------
   * For incrementing the iterator to the next record
   * @return: Reference to this FastaIterator
   */
  FastaIterator operator++ (int);

  /**
   * Equality operator ==
   * ----------------------
   * For comparing one FastaIterator with another
   * @param other: The other FastaIterator to compare to
   * @return: True if the record they contain are equal, false otherwise
   */
  bool operator == (const FastaIterator& other);

  /**
   * Inequality operator !=
   * ----------------------
   * For comparing one FastaIterator with another
   * @param other: The other FastaIterator to compare to
   * @return: True if the record they contain are inequal, false otherwise
   */
  bool operator != (const FastaIterator& other);

private:
  std::istream* in; // The stream to read fasta records from
  bool have_next_header; // True is nextHeader contains the next header
  std::string nextHeader; // The next header in the records
  std::shared_ptr<std::pair<std::string, std::ostringstream>> record; // Pointer to the parsed content

  bool find_next_header();
};

#endif
