/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/program_options.hpp>
#include <functional>

namespace po = boost::program_options;
namespace fs = boost::filesystem;
using namespace std;

#define K_DEFAULT 4
#define DNA_SYMBOLS "ATGC"

#define NUM_THREADS 8

DistributedKmerCounter::DistributedKmerCounter(int* argcp, char*** argvp) : pool(NUM_THREADS),
  processor(argcp, argvp, pool), counter(pool) {
  parseCommandLineOptions(*argcp, *argvp);

}

void DistributedKmerCounter::run() {
  processor.scheduleTasks([&](){
    ofstream outStream(outputFile);
    fs::directory_iterator it(inputDirectory);
    fs::directory_iterator endit;

    while (it != endit) {
      string filename = it->path().filename().generic_string();
      if (fs::is_regular_file(*it) && regex_match(filename, fileRegex))
        processor.scheduleKey(filename);
    }

  }, [this](const string& key){
    return process(key);
  });

  processor.wait();
}

void DistributedKmerCounter::getFiles() {

}


/**
 * Private method: process
 * -----------------------
 * Key-processing function for the distributed k-mer counter
 * @param file: The file to process
 * @return: The result of counting the file
 */
string DistributedKmerCounter::process(const string &file) {
  ostringstream ss;
  counter.countFastaFile(file, ss, true, true);
  return ss.str();
}

/**
 * Private method: parseCommandLineOptions
 * ------------------------------------
 * Parses command line options
 * @param argc: From main argc
 * @param argv: From main argv
 */
void DistributedKmerCounter::parseCommandLineOptions(int argc, const char* const* argv) {

  string file_regex;

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
    ("k,k",       po::value<int>(&kmerLength)->default_value(K_DEFAULT), "k-mer size (i.e. \"k\")")
    ("symbols,s", po::value<string>(&symbols)->default_value(DNA_SYMBOLS), "symbols to use for counting")
    ("sum,sum",   po::bool_switch(&sumFiles), "sum all k-mer counts per file");

  po::options_description hidden("Hidden");
  hidden.add_options()
    ("input-directory", po::value<string>(&inputDirectory), "directory to process from")
    ("output-file", po::value<string>(&outputFile));

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