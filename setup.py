#!/usr/bin/env python3.6
import os
from distutils.core import setup
from distutils.extension import Extension

os.environ["CC"] = "clang"
os.environ["CXX"] = "clang++"

kmerCounterModule = Extension('kmer_counter',
                              language="c++",
                              include_dirs = ['/usr/include', '/usr/local/include', 'include', '/opt/local/include'],
                              library_dirs = ['/usr/local/lib', '/opt/local/lib'],
                              sources = ['src/kmer-counter.cpp'])

setup (name = 'Kmer-Counter',
       version = '1.0',
       description = 'For counting k-mers',
       ext_modules = [kmerCounterModule])

