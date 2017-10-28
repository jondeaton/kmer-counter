/**
 * File: boost-thread-pool.cpp
 * ---------------------------
 * Presents the implementation of the boost thread pool
 * Implemented based on a Stack Overflow post here:
 * https://stackoverflow.com/questions/19500404/how-to-create-a-thread-pool-using-boost-in-c
 */

#include <boost-thread-pool.h>
using namespace std;

#define NUM_THREADS_DEFAULT 8

ThreadPool::ThreadPool() : ThreadPool(NUM_THREADS_DEFAULT) {}

ThreadPool::ThreadPool(size_t numThreads) {
  for (size_t i = 0; i < numThreads; i++)
    thread_group.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
  ioService.post(boost::bind(thunk));
}

void ThreadPool::wait() {
  ioService.run();
  ioService.reset();
}

ThreadPool::~ThreadPool() {
  wait();
  ioService.stop();
  thread_group.join_all();
}