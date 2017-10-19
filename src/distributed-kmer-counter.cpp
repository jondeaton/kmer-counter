/*
 * File: distributed-kmer-counter.cpp
 * ----------------------------------
 * Implementation of distributed k-mer counter
 */

#include "distributed-kmer-counter.h"

DistributedKmerCounter::DistributedKmerCounter(int *argcp, char ***argvp) : processor(argcp, argvp) {}

void DistributedKmerCounter::process(std::string directory, boost::regex pattern, std::string output) {
  (void) directory;
  (void) pattern;
  (void) output;
}

void DistributedKmerCounter::process(std::string &file) {
  (void) file;
}

void DistributedKmerCounter::getKeys(std::queue<std::string> &fileQueue) {
  (void) fileQueue;
}