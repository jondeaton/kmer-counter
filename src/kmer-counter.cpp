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
  cout << "Initiate called" << endl;
  // TODO: Setup threadpool
}

/**
 * Method: countKmers
 * --------------------
 *
 * @param self
 * @param args
 * @return
 */
static PyObject *countKmers(PyObject *self, PyObject * args) {
  const char *command;

  if(!PyArg_ParseTuple(args, "s", &args))
    return NULL;

  cout << "Count was called" << endl;
  return NULL;
}



PyMODINIT_FUNC PyInit_kmer_counter(void)
{
  initiateKmerCounter();
  return PyModule_Create(&kmer_counter);
}

