/*
 * File: fasta-iterator.h
 * ----------------------
 * Presents the implementation of the custon FastaIterator class.
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

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object around the provided ifstream
   * @param in : Stream to read fata contents from
   */
  FastaIterator(std::istream* in);

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object using the path to the provided
   * file as the source of fasta content
   * @param fastaFile : File containing fasta formatted content
   */
  FastaIterator(const std::string& fastaFile);

  /**
   * Public method: begin
   * --------------------
   * Gets the first record from the ifstream if any
   * @return : A pair of
   */
  void begin(std::pair<std::string, std::string>& record);

  /**
   * Public method: next
   * -------------------
   * Gets the next record from the instream
   * @return
   */
  void next(std::pair<std::string, std::string>& record);

  bool end;

private:
  std::istream* in;
  std::string next_header;

  /**
   * Private method: readInRecord
   * ----------------------------
   * Reads in lines from the ifstream into the
   * target string provided.
   * @param target : A string to put the sequence into
   */
  void readInRecord(std::string& target);

};

#endif