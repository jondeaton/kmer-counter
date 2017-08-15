/* kmers.h
 * Functions for counting k-mers
 * Created by Jonathan Deaton on 8/14/17.
 */

#ifndef KMER_COUNTER_KMERS_H
#define KMER_COUNTER_KMERS_H

int countKMers(const char* sequence, const int kmerLength, const char* symbols, long* kmerCount);

#endif //KMER_COUNTER_KMERS_H
