/*
 * File: local-kmer-counter.cpp
 * ----------------------------
 * Implementation of the class representing the
 */

#include "local-kmer-counter.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <boost/log/sinks.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <memory>

#define NUM_THREADS 8
#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"

namespace po = boost::program_options;
namespace fs = boost::filesystem;
namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
using namespace std;

LocalKmerCounter::LocalKmerCounter(int argc, const char* argv[]) : pool(NUM_THREADS), counter(pool) {
  parse_CLI_options(argc, argv);

  setup_streams();
  setup_logging();

  counter.set_kmer_length(kmer_length);
  counter.set_symbols(symbols);
  counter.set_sum_files(sum_files);
}

void LocalKmerCounter::run() {
  if (from_stdin) counter.count(cin, *out_stream, sequential);
  else {
    if (directory_count) counter.count_directory(input_source, *out_stream, sequential);
    else counter.count_fasta_file(input_source, *out_stream, sequential);
  }
}

/**
 * Private method: setup_streams
 * -----------------------------
 *
 */
void LocalKmerCounter::setup_streams() {
  if (!fs::exists(input_source)) {
    cerr << "File not found: " << input_source << endl;
    exit(1);
  }

  if (fs::is_directory(input_source)) directory_count = true;
  else {
    if (fs::is_regular_file(input_source)) directory_count = false;
    else {
      cerr << "File not found: " << input_source << endl;
    }
  }
  directory_count = !fs::is_regular_file(input_source);

  if (from_stdin) out_stream = make_shared<ostream>(cout);
  else out_stream = make_shared<ostream>(new ofstream(output_file));
}

/**
 * Private method: setup_logging
 * -----------------------------
 * Sets up the logger for this object
 */
void LocalKmerCounter::setup_logging() {

//  typedef sinks::synchronous_sink<sinks::text_ostream_backend> text_sink;
//  shared_ptr<text_sink> sink = make_shared<text_sink>();
//
//  if (to_stdout) sink->locked_backend()->add_stream(make_shared<ofstream>("kmers.log"));
//  else sink->locked_backend()->add_stream(make_shared<ostream>(cout));

  typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
  boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();

  sink->locked_backend()->add_stream(
    boost::make_shared< std::ofstream >("sample.log"));

  sink->set_formatter
    (
      expr::format("%1%: <%2%> %3%")
      % expr::attr<unsigned int>("LineID")
      % logging::trivial::severity
      % expr::smessage
    );

  logging::core::get()->add_sink(sink);
}

/**
 * Private method: parse_command_line_options
 * ------------------------------------
 * Parses the command line options from the command line arguments
 * and sets global variables to reflect these options
 * @param argc : Number of command line options including script name
 * @param argv : NULL terminated list of arguments
 * @return: The number of flags
 */
void LocalKmerCounter::parse_CLI_options(int argc, const char *argv[]) {
  string file_regex;

  po::options_description info("Info");
  info.add_options()
    ("help",    "show help dialog")
    ("version", "print version information");

  po::options_description log_options("Logging");
  log_options.add_options()
    ("verbose,v",   po::bool_switch(&verbose), "verbose output")
    ("debug,debug", po::bool_switch(&debug), "debuging output");

  po::options_description config("Config");
  config.add_options()
    ("regex,r",   po::value<string>(&file_regex)->default_value(".*"),      "file pattern regular expression")
    ("k,k",       po::value<size_t>(&kmer_length)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")")
    ("symbols,s", po::value<string>(&symbols)->default_value(DNA_SYMBOLS), "symbols to use for counting")
    ("sum,sum",   po::bool_switch(&sum_files), "sum all k-mer counts per file");

  po::options_description hidden("Hidden");
  hidden.add_options()
    ("input", po::value<string>(&input_source), "File or directory directory to process from")
    ("output-file", po::value<string>(&output_file), "file to write ");

  po::positional_options_description p;
  p.add("input-directory", 1).add("output-file", 1);

  po::options_description desc("Multithreaded k-mer counter options");
  desc.add(info).add(log_options).add(config);

  po::options_description cmdline_options;
  cmdline_options.add(desc).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc,  argv)
              .options(cmdline_options)
              .positional(p)
              .run(), vm);
  po::notify(vm);

  if (vm.count("help")) { // Display help page
    cout << desc << endl;
    exit(1);
  }

  if (vm.count("version")) {
    cout << "Distributed k-mer counter version 1.0" << endl;
    exit(1);
  }

  boost::regex fileRegex(file_regex); // convert string to regex
}