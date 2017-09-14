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


class FastaIterator {

public:

  typedef std::input_iterator_tag iterator_category;
  typedef std::pair<std::string, std::string> value_type;
  typedef void difference_type;
  typedef std::pair<std::string, std::string>* pointer;
  typedef std::pair<std::string, std::string>& reference;

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object that is prepared to parse fasta records from the passed stream.
   * @param in : Stream from which to read and parse fasta records
   */
  explicit FastaIterator(std::istream* in);

  /**
   * Prefix operator
   * ---------------
   *
   * @return
   */
  FastaIterator& operator++ ();

  /**
   * Postrix operatord
   * @return
   */
  FastaIterator operator++ (int);


private:
  std::istream* in; // The stream to read fasta records from
  std::string next_header; // The next header from the stream (for internal implementation
  bool end; // Indicates end of file

  std::shared_ptr<std::pair<std::string, std::string>> record;
};

//typedef FastaIterator<std::pair <std::string, std::string>> fasta_iterator;

#endif