/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor. This class will
 */

#ifndef _BatchProcessor_H
#define _BatchProcessor_H

#include <threadpool.hpp>
#include <functional>
#include <string>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <boost/log/trivial.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;

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
  BatchProcessor(int* argcp, char*** argvp, boost::threadpool::pool& pool);

  /**
   * Public Method: processKeys
   * ----------------------------
   * Coordinates tasks between workers. Head
   * @param schedule_keys: Task executed only on the head node to get and schedule the processing of keys for the rest
   * of the workers to process. Note that this task will be executed asynchronously and calls to wait will block until
   * at least after this function returns. Keys should be scheduled in the body of this task using this processor's
   * scheduleKey method.
   * @param process_key: Routine used to process a single key by a worker
   */
  void process_keys(std::function<void()> schedule_keys,
                    std::function<std::string(const std::string &)> process_key,
                    std::function<std::shared_ptr<std::ostream>()> get_ostream);

  /**
   * Public Method: schedule
   * -----------------------
   * Schedules a key for processing on the batch processor
   * @param key: A reference to a key that should be processed. (Will be coppied)
   */
  void schedule_key(const std::string &key);

  /**
   * Public Method: wait
   * -------------------
   * Wait for all tasks on the batch processor to be completed before returning.
   */
  void wait();

  /**
   * Public method: init_logger
   * --------------------------
   * Initialize the logger for the batch processor
   * @param verbose: True for verbose output
   * @param debug" True for debugging output
   */
  void init_logger(bool verbose, bool debug);

  /**
   * Deconstructor
   * -------------
   * Calls MPI_Finalize
   */
  ~BatchProcessor();

private:

  // Basic MPI data about this current process
  int world_size;
  int world_rank;
  int name_length;
  std::string processor_name;

  std::shared_ptr<std::ostream> output_stream; // stream for master node to write answers to
  boost::threadpool::pool& pool;                           // For processing work asynchronously

  // Synchronization primitives
  std::mutex schedule_mutex;
  std::condition_variable schedule_cv;        // For signaling when there is work to be done

  std::queue<std::string> keys;               // Queue of keys to process
  std::mutex queue_mutex;                     // Lock on the queue since it is modified in multiple threads

  bool scheduling_complete;                    // Indicates when scheduling has been completed
  std::mutex scheduling_complete_mutex;

  // Vector indicating which workers are ready
  std::vector<bool> worker_ready_list;
  std::vector<std::shared_ptr<std::mutex>> worker_mutex_list;

  std::mutex worker_done_mutex;
  std::condition_variable worker_done_cv;

  src::severity_logger<logging::trivial::severity_level> log; // Logger

  /**
   * Private method: master_routine
   * -----------------------------
   * Routine carried out by worker nodes upon call to process_keys
   * @param schedule_keys: Routine carried out by the head node to schedule keys
   * @param get_ostream: Routine carried out by the head node to get a stream to write answers into
   */
  void master_routine(std::function<void()> schedule_keys,
                      std::function<std::shared_ptr<std::ostream>()> get_ostream);

  /**
   * Private method: worker_routine
   * -----------------------------
   * Routine carried out by worker nodes upon call to process_keys
   * @param processKey: The routine that should be used to process a single key
   */
  void worker_routine(std::function<std::string(const std::string &)> processKey);

  // Helpful utility functions
  void receive_and_process_result(int worker);
  bool scheduling_completed();
  bool work_completed();
  bool queue_empty();
  void send_exit_signal(int worker);
};

#endif