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


#   include <boost/date_time/posix_time/posix_time_types.hpp>
#   include <boost/log/trivial.hpp>
#   include <boost/log/core.hpp>
#   include <boost/log/expressions.hpp>
#   include <boost/log/sources/logger.hpp>
#   include <boost/log/utility/setup/file.hpp>
#   include <boost/log/utility/setup/console.hpp>
#   include <boost/log/utility/setup/common_attributes.hpp>
#   include <boost/log/support/date_time.hpp>
#   include <boost/log/sinks/sync_frontend.hpp>
#   include <boost/log/sinks/text_file_backend.hpp>
#   include <boost/log/sinks/text_ostream_backend.hpp>
#   include <boost/log/attributes/named_scope.hpp>



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
  if (from_stdin) counter.count(cin, *out_stream_p, sequential);
  else {
    if (directory_count) counter.count_directory(input_source, *out_stream_p, sequential);
    else counter.count_fasta_file(input_source, *out_stream_p, sequential);
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

  // Make the output stream
  if (from_stdin) out_stream_p = &cout;
  else out_stream_p = new ofstream(output_file);
}

/**
 * Private method: setup_logging
 * -----------------------------
 * Sets up the logger for this object
 */
void LocalKmerCounter::setup_logging() {
  boost::log::add_common_attributes();
  boost::log::core::get()->add_global_attribute("Scope",
                                                boost::log::attributes::named_scope());
  boost::log::core::get()->set_filter(
    boost::log::trivial::severity >= boost::log::trivial::trace
  );

  /* log formatter:
   * [TimeStamp] [ThreadId] [Severity Level] [Scope] Log message
   */
  auto fmtTimeStamp = boost::log::expressions::
  format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
  auto fmtThreadId = boost::log::expressions::
  attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");
  auto fmtSeverity = boost::log::expressions::
  attr<boost::log::trivial::severity_level>("Severity");
  auto fmtScope = boost::log::expressions::format_named_scope("Scope",
                                                              boost::log::keywords::format = "%n(%f:%l)",
                                                              boost::log::keywords::iteration = boost::log::expressions::reverse,
                                                              boost::log::keywords::depth = 2);
  boost::log::formatter logFmt =
    boost::log::expressions::format("[%1%] (%2%) [%3%] [%4%] %5%")
    % fmtTimeStamp % fmtThreadId % fmtSeverity % fmtScope
    % boost::log::expressions::smessage;

  /* console sink */
  auto consoleSink = boost::log::add_console_log(std::clog);
  consoleSink->set_formatter(logFmt);

  /* fs sink */
  auto fsSink = boost::log::add_file_log(
    boost::log::keywords::file_name = "test_%Y-%m-%d_%H-%M-%S.%N.log",
    boost::log::keywords::rotation_size = 10 * 1024 * 1024,
    boost::log::keywords::min_free_space = 30 * 1024 * 1024,
    boost::log::keywords::open_mode = std::ios_base::app);
  fsSink->set_formatter(logFmt);
  fsSink->locked_backend()->auto_flush(true);
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