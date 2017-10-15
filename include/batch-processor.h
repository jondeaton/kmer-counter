/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor.
 */

#ifndef _batch_proc_
#define _batch_proc_

#include <cstring>
#include <string.h>
#include <string>

class BatchProcessor {

public:

  /**
   * Constructor: BatchProcessor
   * ---------------------------
   *
   * @param argcp
   * @param argvp
   */
  BatchProcessor(int* argcp, char*** argvp);

  /**
   * Public Method: SayHello
   * -----------------------
   * Simple testing function
   */
  void SayHello();

  /**
   * Destructor: BatchProcessor
   * --------------------------
   * Makes call to MPI_Finalize
   */
  ~BatchProcessor();

private:
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;
};

#endif