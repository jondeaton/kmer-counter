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

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object that is prepared to parse fasta records from the passed stream.
   * @param in : Stream from which to read and parse fasta records
   */
  explicit FastaIterator(std::istream* in);

  /**
   * Constructor: FastaIterator
   * --------------------------
   * Creates a FastaIterator object that is prepared to parse fasta records from the passed file.
   * @param fastaFile : File from which to read and parse fasta records
   */
  explicit FastaIterator(const std::string& fastaFile);

  /**
   * Public method: begin
   * --------------------
   * Begins the reading in of a fasta formatted record from the stream provided by the construction of the iterator.
   * This method will simply iterate through the beginning of the record until the beginning of a record
   * is found, indicated by a ">" character at the beginning of the line.
   * @param record: A pair of strings that will be popualted with the record ID and sequence, respectively.
   */
  void begin(std::pair<std::string, std::string>& record);

  /**
   * Public method: next
   * -------------------
   * Parses the next record from the stream or file that the iterator was constructed. This method will
   * iterate until it encounters the next record and then will populate the pair of strings passed
   * with the ID and sequence of the pared record. This method should be called if endOfRecords returns true.
   * @param record: A pair of strings that will be populated with the record ID and sequence, respectively.
   */
  void next(std::pair<std::string, std::string>& record);

  /**
   * Public method: endOfRecords
   * ---------------------------
   * This method indicates if there are any more records to parse from the stream by subsequent
   * calls to next.
   * @return: True if there are no more records to read from the stream or file, false otherwise
   */
  bool endOfRecords();

private:
  std::istream* in; // The stream to read fasta records from
  std::string next_header; // The next header from the stream (for internal implementation
  bool end; // Indicates end of file

  void readInRecord(std::string& sequence);
};

#endif