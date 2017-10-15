/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor. This class will
 */

#ifndef _BatchProcessor_H
#define _BatchProcessor_H

#include <functional>
#include <string>
#include <queue>

template <class keyT, class dataT>
class BatchProcessor<keyT, dataT> {

public:

  /**
   * Constructor: BatchProcessor
   * ---------------------------
   * Constructs the batch processor from "mpirun" command line options
   * @param argcp: Pointer to argc from program entry point
   * @param argvp: Pointer to argv from program entry point
   */
  BatchProcessor(int* argcp, char*** argvp);

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
  void process(std::function<void(std::queue<keyT>&)> getKeys,
               std::function<dataT (keyT)> getData,
               std::function<void (dataT)> processData);

  /**
   * Public method: coordinateWorkers
   * --------------------------------
   * Method used by the "head" node to coordinate work on all of the worker nodes.
   * This method will first call the get keys method which was passed, and fill the queue
   * of keys in the batch processor with
   * @param getKeys
   */
  void coordinateWorkers(std::function<void(std::queue<keyT>&)> getKeys);
  void doWork(std::function<dataT (keyT)> getData, std::function<void (dataT)> processData);

  /**
   * Public method: SayHello
   * -----------------------
   * Debugging method. Prints information about this process to stdout.
   */
  void SayHello();

  /**
   * Deconstructor: BatchProcessor
   * -----------------------------
   * Calls MPI_Finalize
   */
  ~BatchProcessor();

private:

  std::queue<keyT> keys; // queue of keys to be processed

  // Basic MPI data
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;
};

#include "batch-processor.tpp"
#endif