# kmer-counter

This repository contains code for multi-threaded and distributed counting of k-mers.

### Dependencies
1. C++17
2. `cmake`
3. `boost`

### Build Executables
In the top level directory (the one containing this file) run:
`cmake . && make`

### Usage (Unix)
Once built, to count k-mers (locally) in a fasta file run 

`./count-kmers path/to/file.fasta`

The results of this calculation will be writen to standard output. If you want to capture these and save them to file pipe the result to a file like so:

`./count-kmers path/to/file.fasta > counts.kmer`

Processing multiple files is also supported by providing a path to a directory instead like so:

`./count-kmers path/to/directory > counts.kmer`

This will attempt to count k-mers in all of the regular files located in the directory. Note that asynchronous processing is enabled by default and the order in which sequences and files appear is not necessarily the order in whcih they will appear in the output files. To turn this off, and count sequences sequentially add the `--sequential` flag like so:

`./count-kmers --sequential path/to/file.kmer > counts.kmer`

## Distributed k-mer counting
Distributed implementation of k-mer counting using OpenMPI is currently under development.

## Python bindings for Python3.6 
Currently under development.
