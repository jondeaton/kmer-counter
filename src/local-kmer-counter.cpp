/*
 * File: local-kmer-counter.cpp
 * ----------------------------
 * Implementation of the class representing the
 */

#include "local-kmer-counter.hpp"
#include "Logger.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#define NUM_THREADS 8
#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace std;

LocalKmerCounter::LocalKmerCounter(int argc, const char* argv[]) : pool(NUM_THREADS), counter(pool) {
  parse_CLI_options(argc, argv);

  init_logging();
  setup_streams();

  counter.set_kmer_length(kmer_length);
  counter.set_symbols(symbols);
  counter.set_sum_files(sum_files);
}

void LocalKmerCounter::run() {
  if (from_stdin) counter.count(cin, *out_stream_p, sequential);
  else {
    if (directory_count) counter.count_directory(input_source, *out_stream_p, sequential);
    else counter.count_fasta_file(input_source, *out_stream_p, sequential);
  }
}

/**
 * Private method: setup_streams
 * -----------------------------
 * Sets up the input and output streams for the k-mer counter
 */
void LocalKmerCounter::setup_streams() {

  // Check to make sure that the file exists
  if (!fs::exists(input_source)) {
    BOOST_LOG_TRIVIAL(error) << "File not found: " << input_source;
    exit(1);
  }

  fs::path p(input_source);
  fs::file_status s = status(p);
  if (s.permissions() | fs::is_oth)

    fs

  // Check to make sure we can read the file
  if (fs::is_directory(input_source)) directory_count = true;
  else {
    if (fs::is_regular_file(input_source)) directory_count = false;
    else {
      BOOST_LOG_TRIVIAL(error) << "Not a regular file: " << input_source;
      exit(1);
    }
  }

  directory_count = fs::is_directory(input_source); // Counting from a directory

  // Make the output stream
  if (to_stdout) out_stream_p = &cout;
  else out_stream_p = new ofstream(output_file);
}

/**
 * Private method: setup_logging
 * -----------------------------
 * Sets up the logger for this object
 */
void LocalKmerCounter::init_logging() {

  // boost::log::sources::severity_logger_mt<  severity_level >& lg = logger::get();





//  boost::log::add_common_attributes();
//
//  if (debug) boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
//  else if (verbose) boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
//  else boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
//
//  // log format: [TimeStamp] [Severity Level] Log message
//  auto fmtTimeStamp = boost::log::expressions::
//  format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S");
//  auto fmtSeverity = boost::log::expressions::
//  attr<boost::log::trivial::severity_level>("Severity");
//
//  boost::log::formatter logFmt =
//    boost::log::expressions::format("[%1%] [%2%] %3%")
//    % fmtTimeStamp
//    % fmtSeverity
//    % boost::log::expressions::smessage;
//
//  // console sink
//  auto consoleSink = boost::log::add_console_log(std::clog);
//  consoleSink->set_formatter(logFmt);
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
void LocalKmerCounter::parse_CLI_options(int argc, const char* argv[]) {
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
  p.add("input", 1).add("output-file", 1);

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