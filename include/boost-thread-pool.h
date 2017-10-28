/*
 * File: boost-thread-pool.h
 * -------------------------
 * Presents the interface of a thread pool implemented with boost.
 */

#include <boost/bind.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

#ifndef _boost_thread_pool_h
#define _boost_thread_pool_h

class ThreadPool {
public:
  ThreadPool();
  explicit ThreadPool(size_t numThreads);

/**
   * Public Method: schedule
   * -----------------------
   * Used to schedule a task to be performed by the thread pool
   * @param thunk: A task to complete on a thread in the thread pool
   */
  void schedule(const std::function<void(void)>& thunk);

  /**
   * Public Method: wait
   * -------------------
   * Blocks until all previously scheduled tasks are completed
   */
  void wait();

  ~ThreadPool();

private:
  boost::asio::io_service ioService;
  boost::thread_group thread_group;

  // In order to prevent cloning, remove the copy constructor and the assignment operator.
  ThreadPool(const ThreadPool& original) = delete;
  ThreadPool& operator=(const ThreadPool& rhs) = delete;
};

#endif