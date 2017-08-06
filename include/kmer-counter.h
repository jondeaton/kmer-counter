#include <Python.h>

PyObject *KmerError;

// Python setup declarations
static PyObject *countKmers(PyObject *self, PyObject * args);
PyMODINIT_FUNC PyInit_kmer_counter(void);

static PyMethodDef module_methods[] = {
  {"countAsync", (PyCFunction) countKmers, METH_VARARGS, "Counts k-mers"},
  {NULL, NULL, 0, NULL}
};

static struct PyModuleDef kmer_counter = {
  PyModuleDef_HEAD_INIT,
  "kmer_counter", /* name of module */
  "Module documentation goes here",
  -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
  module_methods
};