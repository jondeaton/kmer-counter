# kmer-counter

This repository contains code for multi-threaded and distributed counting of k-mers.

### Dependencies
1. C++17
2. `cmake`
3. `boost`

### Build
In the top level directory run:

`cmake .`
`make`

### Use
Once build, to count k-mers in a fasta file run 

`./count-kmers path/to/file.fasta`

The results of this calculation will be writen to standard output. If you want to capture these and save them to file pipe the result to a file like so:

`./count-kmers path/to/file.fasta > result.kmer`

## Distributed k-mer counting
Currently under development.

## Python bindings for Python3.6 
Currently under development.
