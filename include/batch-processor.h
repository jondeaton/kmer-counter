/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor. This class will
 */

#ifndef _BatchProcessor_H
#define _BatchProcessor_H

#include "boost-thread-pool.h"
#include <functional>
#include <string>
#include <queue>
#include <fstream>

class BatchProcessor {

public:

  /**
   * Constructor: BatchProcessor
   * ---------------------------
   * Constructs the batch processor from "mpirun" command line options
   * @param argcp: Pointer to argc from program entry point
   * @param argvp: Pointer to argv from program entry point
   */
  BatchProcessor(int* argcp, char*** argvp,
                 ThreadPool& pool,
                 std::function<std::string (const std::string&)> processKey);

  void

  void schedule(std::string key);

  void wait();

  /**
   * Deconstructor
   * -------------
   * Calls MPI_Finalize
   */
  ~BatchProcessor();

private:

  ThreadPool& pool;
  std::queue<std::string> keys;

  // Basic MPI data
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;

  void masterRoutine();
  void workerRoutine();

  void receiveAndProcessResult(int worker);

  void sendExitSignal(int worker);
};

#endif