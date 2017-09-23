/**
 * Implements the semaphore class
 * Author: Jerry Cain
 */

#ifndef _semaphore_
#define _semaphore_

#include <mutex>
#include <condition_variable>

struct on_thread_exit_t {};
constexpr on_thread_exit_t on_thread_exit {};

class semaphore {
 public:
  semaphore(int value = 0);
  void wait();
  void signal();
  void signal(on_thread_exit_t ote);
  
 private:
  int value;

  std::mutex m;
  std::condition_variable_any cv;

  semaphore(const semaphore& orig) = delete;
  const semaphore& operator=(const semaphore& rhs) const = delete;
};

#endif
