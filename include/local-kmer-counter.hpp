/*
 * File: local-kmer-counter.h
 * --------------------------
 * Class representing the command line local k-mer counter tool
 */

#ifndef _LOCAL_KMER_COUNTER_H_INCLUDED
#define _LOCAL_KMER_COUNTER_H_INCLUDED

#include "async-kmer-counter.hpp"

#include <ostream>
#include <string>
#include <memory>
#include <threadpool.hpp>
#include <boost/regex.hpp>

class LocalKmerCounter {

public:
  LocalKmerCounter(int argc, const char* argv[]);
  void run();

private:
  boost::threadpool::pool pool;
  AsyncKmerCounter counter;

  // Program options
  bool verbose;
  bool debug;
  std::string symbols;
  size_t kmer_length;
  bool sequential;
  bool sum_files;

  bool directory_count;
  bool from_stdin;
  bool to_stdout;

  std::string input_source;
  boost::regex file_regex;
  std::string output_file;

  std::ostream* out_stream_p;

  void setup_streams();
  void init_logging();
  void parse_CLI_options(int argc, const char *argv[]);
};

#endif // _LOCAL_KMER_COUNTER_H_INCLUDED