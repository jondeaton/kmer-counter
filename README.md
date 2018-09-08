# kmer-counter
This project showcases the exact couting of k-mers in DNA sequences using distributed batch processing with MPI (Message Passing Interface). This project implementes a simple but generic batch processing framework in MPI, for coordinating multiple nodes to complete a subset of some large data-processing tasks in parallel. This framework is then used to parallelize a k-mer counting application.

There is also a Spark-based implementation for demonstration purposes.

## Dependencies
1. C++11
2. Boost
3. OpenMPI

## Background
In biology,  the analysis of DNA sequences is critical in understanding biologic systems. Many DNA analysis algorithms focus on identifying genes (the functional units that DNA encodes), however, some DNA analysis algorithms focus on other features of DNA sequences. One alternate approach is analyzing the "k-mer" content of a DNA sequence. K-mers are short sub-sequences of a DNA sequence of length k. Many DNA analysis algorithms make conclusions about biologic systems based on the abundances of each k-mer in the DNA sequence. Other k-mer based metrics include the number of unique k-mers in a DNA sequence and the shape of the distribution of k-mer frequencies. In my undergraduate research, I used the frequencies of k-mers in DNA sequences to

## Algorithm
The k-mer counting algorithm that I used is very simple: for a given DNA sequence, use a sliding window length k (base pairs), and then slide along the DNA sequence from beginning to end. For each position of the sliding window, increment a counter of how many times that particular k-mer has been seen in the sequence. Since DNA has four base pairs (i.e. A, T, G, C), there are 4k distinct k-mers to count the occurrences of.  To store these counts create an array of integers of size 4k and designate each k-mers to each element of the array lexicographically (e.g. AAA → 0, AAT → 1, AAC → 2, ... GGG → 4k-1). Thus, for each position of the sliding window, we simply calculate the lexicographic index of the k-mer and increment that element in the array.

