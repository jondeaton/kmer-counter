#include <Python.h>

PyObject *KmerError;

static const size_t kNumThreads = 8;

// Python setup declarations
static PyObject *PyCountKmers(PyObject *self, PyObject *args);
static PyObject *PyScheduleCount(PyObject *self, PyObject *args);
static PyObject *PyWaitCount(PyObject *self, PyObject *args);

static PyMethodDef module_methods[] = {
  {"count", (PyCFunction) PyCountKmers, METH_VARARGS, "Count k-mers"},
  {"countAsync", (PyCFunction) PyScheduleCount, METH_VARARGS, "Counts k-mers asynchronously"},
  {"wait", (PyCFunction) PyWaitCount, METH_VARARGS, "Wait for asynchronous calls to finish"},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef kmer_counter = {
  PyModuleDef_HEAD_INIT,
  "kmer_counter", /* name of module */
  "Module documentation goes here",
  -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
  module_methods
};

PyMODINIT_FUNC PyInit_kmer_counter(void);