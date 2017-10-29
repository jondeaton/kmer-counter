


#include "async-kmer-counter.h"
#include <threadpool.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>



#define NUM_THREADS 8


class LocalKmerCounter {



public:
  void run();


private:
  boost::threadpool::pool pool;
  AsyncKmerCounter counter;




// Program options
  bool verbose;
  bool debug;
  const string symbols;
  unsigned int kmer_length;
  bool sequential;
  bool sum_files;


};


LocalKmerCounter::LocalKmerCounter(int argc, char* argv) {
  parseCommandLineOptions(argc, argv);
}



void LocalKmerCounter::run() {

  boost::threadpool::pool pool(NUM_THREADS);
  AsyncKmerCounter counter(pool, symbols, kmer_length, sum_files);

  if (argc > 1 + numFlags) {
    string path(argv[1 + numFlags]);
    if (boost::filesystem::is_regular_file(path))
      counter.count_fasta_file(path, cout, sequential, true);
    else if (boost::filesystem::is_directory(path))
      counter.count_directory(path, cout, sequential, true);
  } else counter.count(cin, cout, sequential, true);
}