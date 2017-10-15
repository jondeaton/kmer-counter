/*
 * File: batch-processor.h
 * -----------------------
 * Presents the interface of a batch processor. This class will
 */

#ifndef _BatchProcessor_H
#define _BatchProcessor_H

#include <cstring>
#include <string>

template <class keyT, dataT>
class BatchProcessor<keyT, dataT> {

public:

  BatchProcessor(int* argcp, char*** argvp);



  void SayHello();

  /**
   * Public Method: process
   * ----------------------
   * Method
   * @param getKeys: A function that fills a queue with keys to be processed
   * @param getData: A function that gets raw data to be processed from a key
   * @param processData: A function that does the data processing
   */
  void process(std::function<void(std::queue<keyT>&)> getKeys,
               std::function<dataT (keyT)> getData,
               std::function<void (dataT)> processData);


  void coordinateWorkers(std::queue<keyT>& keys);
  void doWork(std::function<dataT (keyT)> getData, std::function<void (dataT)> processData);

  ~BatchProcessor();

private:

  std::queue<keyT> keys; // queue of keys to be processed

  int worldSize;
  int worldRank;
  int nameLength;
  std::string processorName;
};

#include "batch-processor.cpp"

#endif