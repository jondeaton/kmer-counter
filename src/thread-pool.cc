/**
 * File: thread-pool.cc
 * --------------------
 * Presents the implementation of the ThreadPool class.
 */

#include "thread-pool.h"
using namespace std;

/**
 * Constructor
 * -----------
 * Constructs a ThreadPool object configured to spawn up to the specified
 * number of threads.
 * @param numThreads : The number of threads in this thread pool
 */
ThreadPool::ThreadPool(size_t numThreads) :
  wts(numThreads), numThreads(numThreads),
  threadsShouldExit(false), dispatchSignal(0) {

  // Make a dispatcher thread
  dt = thread([this]() {
    dispatcher();
  });

  // Spawn all workers
  lock_guard<mutex> lg1(funcitonMapLock);
  lock_guard<mutex> lg2(freeWorkerLock);

  for (id_t workerID = 0; workerID < numThreads; workerID++) {

    // New semaphore to know when to execute thunk
    workerSignalMap[workerID].reset(new semaphore(0));
    freeWorkerMap[workerID] = true; // Worker begins free

    // Create a thread for the worker
    wts[workerID] = thread([this](size_t workerID) {
      worker(workerID);
    }, workerID);
  }
}

/**
 * Public Method: schedule
 * -----------------------
 * Schedules the provided thunk (which is something that can
 * be invoked as a zero-argument function without a return value)
 * to be executed by one of the ThreadPool's threads as soon as
 * all previously scheduled thunks have been handled.
 * @param thunk : A function with no parameters and no return value
 * that will be scheduled for execution by a worker thread
 */
void ThreadPool::schedule(const function<void(void)>& thunk) {
  lock_guard<mutex> lg(queueLock); // Lock the queue for editing
  functionQueue.push(thunk); // Add the function to the queue
  dispatchSignal.signal(); // Something is in the queue!
}

/**
 * Public Method: wait
 * -------------------
 * This function blocks until all previously scheduled thunks have
 * been executed. This is accomplished by setting up a local
 * semaphore and then scheduling a thunk to signal that semaphore.
 * After being signaled, wait() knows that all thunks in the queue have been
 * dispatched, however some previously scheduled thunks may need to finish.
 * At this point, wait will halt dispatching and wait until all workers
 * become available.
 */
void ThreadPool::wait() {

  // Signal waiting thread that this function was pushed through the queue
  semaphore waitSemaphore(0);
  schedule([&waitSemaphore]() {
    waitSemaphore.signal();
  });

  waitSemaphore.wait(); // Wait for signaling function to go through queue

  // Halt dispatching to wait until all workers are done
  dispatchingLock.lock();

  // Wake up when some worker has finished to check if all the workers are finished
  lock_guard<mutex> lg(workerFinished);
  cv.wait(workerFinished, [this]() {

    lock_guard<mutex> lg(freeWorkerLock);

    for (id_t workerID = 0; workerID < numThreads; workerID++)
      if (!freeWorkerMap[workerID]) return false;

    return true; // All workers are done
  });

  dispatchingLock.unlock(); // Continue dispatching jobs
}

/**
 * Private Method: dispatcher
 * --------------------------
 * This method will continuously wait until there is something in the
 * thunk queue and then dispatch that thunk to be executed by a worker
 */
void ThreadPool::dispatcher() {

  while (true) {

    // Wait for something to be scheduled
    queueLock.lock();
    while (functionQueue.empty()) {
      queueLock.unlock();
      dispatchSignal.wait();

      // Exit if indicated so
      if (threadsShouldExit) return;

      queueLock.lock();
    }

    dispatchingLock.lock(); // Make sure that we are allowed to dispatch
    freeWorkerLock.lock(); // Take control of the list of free workers

    // This code block searches for a free worker
    id_t workerID;
    while (true) {

      for (workerID = 0; workerID < this->numThreads; workerID++)
        if (freeWorkerMap[workerID]) break; // Found a free worker

      if (workerID != this->numThreads) break; // Found free worker
      else {
        // Didn't find a free worker wait until one signals completion

        // Need to unlock this so that worker can notify us that it's done
        freeWorkerLock.unlock();

        // Wait for a workerFinished notification
        workerFinished.lock();
        cv.wait(workerFinished);
        workerFinished.unlock();

        freeWorkerLock.lock(); // Re-secure this resource
      }
    }
    // At this point we have identified a certain worker as available
    freeWorkerMap[workerID] = false; // Mark worker as unavailable

    // Assign the next function to the free worker
    funcitonMapLock.lock();
    workerFunctions[workerID] = functionQueue.front();
    funcitonMapLock.unlock(); // Done with the function map

    functionQueue.pop(); // Dequeue this function
    queueLock.unlock(); // Done with the function queue

    // Signal the worker to start working
    workerSignalLock.lock();
    workerSignalMap[workerID]->signal();
    workerSignalLock.unlock();

    // This unlock needs to be down here so that ThreadPool::wait only
    // wait on threads after they have been signaled
    freeWorkerLock.unlock(); // Done examining free workers
    dispatchingLock.unlock(); // Dispatch complete
  }
}

/**
 * Private Method: worker
 * ---------------------
 * This method first waits until signaled by the workerSignalLock associated
 * with the ID, then it will retrieve the function to execute from the queue
 * and execute the funciton. This method will also indicate that it is available
 * after it has completed it's assigned task and notify the condition_variable_any
 * cv that it has finished.
 * This method is meant to be executed as a thread routine.
 * @param workerID : The ID of the worker that is
 */
void ThreadPool::worker(const id_t workerID) {

  while (true) {

    // Get the semaphore that this thread needs waits for
    workerSignalLock.lock();
    unique_ptr<semaphore>& s = workerSignalMap[workerID];
    workerSignalLock.unlock();

    // Wait until thread is signaled to execute function
    s->wait();

    // Exit if indicated so
    if (threadsShouldExit) return;

    // Retrieve the function this thread will execute
    funcitonMapLock.lock();
    function<void(void)> thunk = workerFunctions[workerID];
    funcitonMapLock.unlock();

    thunk(); // Do the assigned task

    // Indicate that this worker is now free
    freeWorkerLock.lock();
    freeWorkerMap[workerID] = true;
    freeWorkerLock.unlock();

    // Notify that this worker has finished
    workerFinished.lock();
    cv.notify_all();
    workerFinished.unlock();
  }
}

/**
 * Deconstructor
 * -------------
 * This deconstructor will wait until all scheduled tasks have been done and then
 * set the threadsShouldExit boolean to true and then instruct all threads to
 * continue at which point they will check this boolean and see that they are
 * supposed to exit. This deconstructor then joins up with all threads from
 * this ThreadPool.
 */
ThreadPool::~ThreadPool() {

  wait(); // Wait for all scheduled tasks to finish

  threadsShouldExit = true; // Indicate all threads to exit

  // Signal to the dispatcher to continue (it will exit)
  dispatchSignal.signal();

  // Signal all the threads to go (they will immediately exit)
  workerSignalLock.lock();
  for(id_t workerID = 0; workerID < numThreads; workerID++)
    workerSignalMap[workerID]->signal();
  workerSignalLock.unlock();

  dt.join(); // Wait for the dispatcher to exit
  for(thread& t : wts) t.join(); // Wait for workers to exit
}