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
 * up the k-mer counter. This method initializes the thread
 * pool that can be used to asynchronously count k-mers
 */
static void initiateKmerCounter() {
  cout << "Initializing k-mer counter threadpool..." << endl;

//  boost::asio::io_service::work work(ioService);
//
//  for (size_t i = 0; i < kNumThreads; i++) {
//    threadpool.create_thread(
//      boost::bind(&boost::asio::io_service::run, &ioService)
//    );
//  }

  cout << "K-mer counter Initialized." << endl;
}

/**
 * Function: PyCountKmers
 * --------------------
 * Basic k-mer counting functino
 * @param self
 * @param args
 * @return
 */
static PyObject *PyCountKmers(PyObject *self, PyObject *args) {

  char* sequence;
  int kmerLength;
  char* symbols;
  long* kmerCount;

  int parsed = PyArg_ParseTuple(args, "sdsd", &sequence, &kmerLength, &symbols, &kmerCount);

  if (!parsed) return NULL;

  int error = countKMers(sequence, kmerLength, symbols, kmerCount);
  if (error < 0) {
    return NULL;
  }

  return Py_BuildValue(""); // Return None
}

/**
 * Funciton: PyScheduleCount
 * -------------------------
 * Schedules an asynchronous k-mer counting
 * @param self
 * @param args
 * @return
 */
static PyObject *PyScheduleCount(PyObject *self, PyObject *args) {


  return Py_BuildValue(""); // Return None
};

/**
 * Function: PyWaitCount
 * ---------------------
 * Waits for all previous asynchronous calls to PyScheduleCount to finish
 * @param self
 * @param args
 * @return
 */
static PyObject *PyWaitCount(PyObject *self, PyObject *args) {

  return Py_BuildValue(""); // Return None
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


