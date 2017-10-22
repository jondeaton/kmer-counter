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
  BatchProcessor(int* argcp, char*** argvp);

  void schedule(const std::function<void(void)>& task, );
  void wait();

  /**
   * Public method: process
   * ----------------------
   * This function is executed on all nodes. If the node is the head node then
   * this function will call coordinateWorkers, and if this is executed on a worker node, then
   * this method will
   * @param getKeys: A function that fills a queue with keys to be processed
   * @param getData: A function that gets raw data to be processed from a key
   * @param processData: A function that does the data processing
   */
  void process(std::function<void (std::queue<std::string>&)> getKeys,
               std::function<void (std::string&)> processData);

  /**
   * Deconstructor: BatchProcessor
   * -----------------------------
   * Calls MPI_Finalize
   */
  ~BatchProcessor();

private:

  // Basic MPI data
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;


  std::ofstream outputStream; // stream to write answers to

  ThreadPool& pool;

  void coordinateWorkers(std::function<void(std::queue<std::string>&)> getKeys);
  void receiveAndProcessResult(int worker);
  void doWork(std::function<void (std::string&)> processData);
  void sendExitSignal(int worker);
};

#endif