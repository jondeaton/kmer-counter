/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

using namespace std;

DistributedKmerCounter::DistributedKmerCounter(int *argcp, char ***argvp) :
  processor(argcp, argvp) { }

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
  os.c_str();
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