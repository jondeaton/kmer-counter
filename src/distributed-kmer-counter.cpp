/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.hpp"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <functional>
#include <memory>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"
#define NUM_THREADS 8

DistributedKmerCounter::DistributedKmerCounter(int* argcp, char*** argvp) :
  pool(NUM_THREADS), processor(argcp, argvp, pool), counter(pool) {

  parse_CLI_options(*argcp, *argvp);

  counter.set_kmer_length(kmer_length);
  counter.set_symbols(symbols);
  counter.set_sum_files(sum_files);

  processor.init_logger(verbose, debug);
}

void DistributedKmerCounter::run() {
  processor.process_keys([this](){
    schedule_files();
  }, [this](const string &file) {
    return count_kmers(file);
  }, [this]() {
    shared_ptr<ostream> osp(new ofstream(output_file));
    return osp;
  });

  processor.wait();
}

void DistributedKmerCounter::schedule_files() {
  cout << "Scheculing files!!" << endl;
  fs::directory_iterator it(input_directory);
  fs::directory_iterator endit;

  while (it != endit) {
    string file_name = it->path().filename().generic_string();
    if (fs::is_regular_file(*it) && regex_match(file_name, file_regex))
      processor.schedule_key(file_name);
  }
}

/**
 * Private method: process
 * -----------------------
 * Key-processing function for the distributed k-mer counter
 * @param file: The file to process
 * @return: The result of counting the file
 */
string DistributedKmerCounter::count_kmers(const string &file) {
  ostringstream ss;
  counter.count_fasta_file(file, ss, true, true);
  return ss.str();
}

/**
 * Private method: parseCommandLineOptions
 * ------------------------------------
 * Parses command line options
 * @param argc: From main argc
 * @param argv: From main argv
 */
void DistributedKmerCounter::parse_CLI_options(int argc, const char *const *argv) {

  string file_regex;

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
    ("regex,r",   po::value<string>(&file_regex)->default_value(".*"),      "file pattern regular expression")
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

  boost::regex fileRegex(file_regex); // convert string to regex
}