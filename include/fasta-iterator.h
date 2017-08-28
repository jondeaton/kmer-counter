/*
 * File: fasta-iterator.h
 *
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
  FastaIterator(std::ifstream& in);

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
  std::pair<std::string, std::string> begin();

  /**
   * Public method: next
   * -------------------
   * Gets the next record from the instream
   * @return
   */
  std::pair<std::string, std::string> next();

  /**
   * Public method: end
   * ------------------
   * Signifies the end of the FastaIterator
   * @return : A nullptr specifying the end of the fasta sequence
   */
  std::pair<std::string, std::string>* end() { return nullptr; }

private:
  std::ifstream in;
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