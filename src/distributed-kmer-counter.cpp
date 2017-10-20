/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>

using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"

DistributedKmerCounter::DistributedKmerCounter(int *argcp, char ***argvp) :
  processor(argcp, argvp) {
  parse_command_line_options(*argcp, *argvp);
  boost::regex fileRegex(file_regex); // convert string to regex
}

void DistributedKmerCounter::process(string directory, boost::regex pattern, string output) {

  processor.process([this, directory, pattern](queue<string> &fileQueue){
    this->getFiles(directory, pattern, fileQueue);
  },[this](string& file){
    this->process(file);
  });

  (void) directory;
  (void) pattern;
  (void) output;
}

void DistributedKmerCounter::process(string &file) {
  ostream os();
  counter.countFastaFile(file, os, true, true);
}

// Gets all file matching a name in a directry
void DistributedKmerCounter::getFiles(string directory, boost::regex pattern, queue<string> &fileQueue) {

  fs::directory_iterator it(directory);
  fs::directory_iterator endit;

  while( it != endit) {
    string filename = it->path().filename().generic_string();
    if(fs::is_regular_file(*it) && regex_match(filename, pattern) fileQueue.push(filename);
  }
}

/**
 * Function: parse_command_line_options
 * ------------------------------------
 * Parses command line options
 * @param argc: From main argc
 * @param argv: From main argv
 */
void DistributedKmerCounter::parse_command_line_options(int argc, const char* const* argv) {

  po::options_description info("Info");
  info.add_options()
    ("help",    "show help dialog")
    ("version", "print program version");

  po::options_description log_options("Logging");
  log_options.add_options()
    ("verbose,v",   po::bool_switch(&verbose), "Verbose output")
    ("debug,debug", po::bool_switch(&debug), "Debug output");

  po::options_description config("Config");
  config.add_options()
    ("regex,r",   po::value<string>(&file_regex)->default_value(".*"),      "file pattern regular expression")
    ("k,k",       po::value<int>(&k)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")")
    ("symbols,s", po::value<string>(&symbols)->default_value(DNA_SYMBOLS), "symbols to use for counting")
    ("sum,sum",   po::bool_switch(&sumFiles), "sum all k-mer counts per file");

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
}