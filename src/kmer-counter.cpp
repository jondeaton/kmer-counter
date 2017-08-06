#include "kmer-counter.h"
#include <cstdio>
#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include "kmers.cpp"
using namespace std;

// Global variables
boost::asio::io_service ioService;
boost::thread_group threadpool;

/**
 * Method: Main
 * ------------
 * Entry method for kmer counting python module, called on module import
 * @param argc: Number of arguments
 * @param argv: List of arguments
 * @return
 */
int main(int argc, char *argv[]) {
  Py_SetProgramName((wchar_t*) argv[0]);
  Py_Initialize();
  PyInit_kmer_counter();
  return 0;
}

/**
 * Method: initiateKmerCounter
 * ---------------------------
 * Initializing method called on import for setting
 * up the k-mer counter.
 */
static void initiateKmerCounter() {
  cout << "K-mer counter threadpool..." << endl;
  boost::asio::io_service::work work(ioService);

  cout << "Adding threads..." << endl;

  for (size_t i = 0; i < kNumThreads; i++) {
    threadpool.create_thread(
      boost::bind(&boost::asio::io_service::run, &ioService)
    );
  }

  cout << "Initialized." << endl;
}

/**
 * Function: PyCountKmers
 * --------------------
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *PyCountKmers(PyObject *self, PyObject *args) {

  char* sequence;
  int kmerLength;
  char* symbols;
  long* kmerCount;

  bool parsed = PyArg_ParseTuple(args, "sdsd", &sequence, &kmerLength, &symbols, &kmerCount);

  if (!parsed) return NULL;

  int error = countKMers(sequence, kmerLength, symbols, kmerCount);
  if (error < 0) {
    return NULL;
  }
  return NULL;
}

/**
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *PyScheduleCount(PyObject *self, PyObject *args) {

};

/**
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *PyWaitCount(PyObject *self, PyObject *args) {


};


/**
 * Function: PyInit_kmer_counter
 * -----------------------------
 * Python initialization function for k-mer counter
 */
PyMODINIT_FUNC PyInit_kmer_counter(void) {
  initiateKmerCounter();
  return PyModule_Create(&kmer_counter);
}


