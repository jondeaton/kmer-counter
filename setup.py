import os
from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from Cython.Distutils import build_ext

os.environ["CC"] = "clang"
os.environ["CXX"] = "clang++"

kmerCounterModule = Extension('kmer_counter',
                              language="c++",
                              include_dirs = ['/usr/include', '/usr/local/include', 'include'],
                              library_dirs = ['/usr/local/lib'],
                              sources = ['src/kmer-counter.cpp'])

setup (name = 'Kmer-Counter',
       version = '1.0',
       description = 'For counting k-mers',
       ext_modules = [kmerCounterModule])

