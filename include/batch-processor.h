/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor. This class will
 */

#ifndef _batch_proc_
#define _batch_proc_

#include <cstring>
#include <string.h>

class BatchProcessor {

public:

  void SayHello();

private:
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;
};

#endif