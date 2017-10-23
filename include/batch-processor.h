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
#include <iostream>

class BatchProcessor {

public:

  /**
   * Constructor: BatchProcessor
   * ---------------------------
   * Constructs the batch processor using "mpirun" command line options
   * @param argcp: Pointer to argc from program entry point
   * @param argvp: Pointer to argv from program entry point
   * @param pool: A thread pool to schedule asynchronous tasks on
   */
  BatchProcessor(int* argcp, char*** argvp, ThreadPool& pool);

  /**
   * Public Method: processKeys
   * ----------------------------
   * Coordinates tasks between workers. Head
   * @param scheduleKeys: Task executed only on the head node to get and schedule the processing of keys for the rest
   * of the workers to process. Note that this task will be executed asynchronously and calls to wait will block until
   * at least after this function returns. Keys should be scheduled in the body of this task using this processor's
   * scheduleKey method.
   * @param processKey: Routine used to process a single key by a worker
   */
  void processKeys(std::function<void (void)> scheduleKeys,
                   std::function<std::string (const std::string &)> processKey,
                   std::function<std::shared_ptr<std::ostream>()> getOstream);

  /**
   * Public Method: schedule
   * -----------------------
   * Schedules a key for processing on the batch processor
   * @param key: A reference to a key that should be processed. (Will be coppied)
   */
  void scheduleKey(const std::string &key);

  /**
   * Public Method: wait
   * -------------------
   * Wait
   */
  void wait();

  /**
   * Deconstructor
   * -------------
   * Calls MPI_Finalize
   */
  ~BatchProcessor();

private:

  std::condition_variable cv;

  std::queue<std::string> keys;
  std::mutex queue_mutex;
  ThreadPool& pool;

  std::shared_ptr<std::ostream> outputStream;

  bool schedulingComplete;

  // Basic MPI data
  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;

  void masterRoutine(std::function<void ()> scheduleKeys, std::function<std::shared_ptr<std::ostream>()> getOstream);
  void workerRoutine(std::function<std::string (const std::string &)> processKey);

  void receiveAndProcessResult(int worker);

  void sendExitSignal(int worker);
};

#endif