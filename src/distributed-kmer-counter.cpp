/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.hpp"

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"
#define NUM_THREADS 8

DistributedKmerCounter::DistributedKmerCounter(int* argcp, char*** argvp) :
  pool(NUM_THREADS), processor(argcp, argvp, pool), counter(pool) {

  parse_CLI_options(*argcp, *argvp);

  stringstream s;
  s << output_file << "." << processor.getRank();
  out_stream_p = make_shared<ofstream>(s.str());

  counter.set_kmer_length(kmer_length);
  counter.set_symbols(symbols);
  counter.set_sum_files(sum_files);

  processor.init_logger(verbose, debug);
}

void DistributedKmerCounter::run() {
  processor.process_keys
    (
      // Schedule files task
      [this](){ schedule_files(); },

      // Process key task
      [this](const string &file) { return count_kmers(file); }
    );

  processor.wait();
}

// File scheduling
void DistributedKmerCounter::schedule_files() {

  fs::directory_iterator it(input_directory);
  fs::directory_iterator endit;

  while (it != endit) {
    string file_name = it->path().generic_string();
    if (fs::is_regular_file(file_name) && regex_match(file_name, file_regex))
      processor.schedule_key(file_name);
    ++it;
  }
}

/**
 * Private method: process
 * -----------------------
 * Key-processing function for the distributed k-mer counter
 * @param file: The file to process
 * @return: The result of counting the file
 */
void DistributedKmerCounter::count_kmers(const string &file) {
  counter.count_fasta_file(file, *out_stream_p, true, true);
}

/**
 * Private method: parseCommandLineOptions
 * ------------------------------------
 * Parses command line options
 * @param argc: From main argc
 * @param argv: From main argv
 */
void DistributedKmerCounter::parse_CLI_options(int argc, const char *const *argv) {

  string fre;

  po::options_description info("Info");
  info.add_options()
    ("help",    "show help dialog")
    ("version", "print program version");

  po::options_description log_options("Logging");
  log_options.add_options()
    ("verbose,v",   po::bool_switch(&verbose), "verbose output")
    ("debug,debug", po::bool_switch(&debug), "debuging output");

  po::options_description config("Config");
  config.add_options()
    ("regex,r",   po::value<string>(&fre)->default_value(".*"),      "file pattern regular expression")
    ("k,k",       po::value<size_t>(&kmer_length)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")")
    ("symbols,s", po::value<string>(&symbols)->default_value(DNA_SYMBOLS), "symbols to use for counting")
    ("sum,sum",   po::bool_switch(&sum_files), "sum all k-mer counts per file");

  po::options_description hidden("Hidden");
  hidden.add_options()
    ("input-directory", po::value<string>(&input_directory), "directory to process from")
    ("output-file", po::value<string>(&output_file));

  po::positional_options_description p;
  p.add("input-directory", 1).add("output-file", 1);

  po::options_description desc("Distributed k-mer counter options");
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

  boost::regex fileRegex(fre); // convert string to regex
  file_regex = fileRegex;
}