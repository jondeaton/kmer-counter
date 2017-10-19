/*
 * File: main-distributed.cpp
 * --------------------------
 * Program entry point for the distributed k-mer counter.
 *
 * Usage
 *
 * mpirun count-kmers
 *
 */

#include "distributed-kmer-counter.h"
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <vector>
#include <iostream>

namespace po = boost::program_options;
using namespace std;

#define K_DEFAULT 4

bool verbose;
bool debug;
int k;
string input_directory;
string file_regex;
string output_file;

void parse_command_line_options(int argc, const char* const* argv);

// Entry point
int main(int argc, char* argv[]) {

  DistributedKmerCounter counter(&argc, &argv);

  parse_command_line_options(argc, argv);

  counter.k = k;
  boost::regex fre(file_regex); // convert string to regex
  counter.process(input_directory, fre, output_file);
}

/**
 * Function: parse_command_line_options
 * ------------------------------------
 * Parses command line options
 * @param argc: From main argc
 * @param argv: From main argv
 */
void parse_command_line_options(int argc, const char* const* argv) {

  po::options_description info("Info");
  info.add_options()
    ("help",    "show help dialog")
    ("version", "print program version");

  po::options_description log_options("Logging");
  log_options.add_options()
    ("verbose,v",   "Verbose output")
    ("debug,debug", "Debug output");

  po::options_description config("Config");
  config.add_options()
    ("regex,r", po::value<string>()->default_value("*"),      "file pattern regular expression")
    ("k,k",     po::value<int>(&k)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")");

  po::options_description hidden("Hidden");
  hidden.add_options()
    ("input-directory", po::value<string>(), "directory to process from")
    ("output-file",     po::value<string>(), "output file");

  po::positional_options_description p;
  p.add("input-directory", -1);

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

  // Set globals
  verbose = (bool) vm.count("verbose");
  debug = (bool) vm.count("debug");
  try {
    file_regex = vm["regex"].as<string>();

    cout << "trying things" << endl;

    auto shit = vm["input-directory"].as<vector<string>>(); // <-- why the fuck doesn't this work

    cout << "MADE IT THIS FAR @@@@@@@" << endl; exit(69);
  } catch (int e) {
    cout << desc << endl;
    exit(1);
  }
}