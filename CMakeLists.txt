cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 17)

#SET(CMAKE_CXX_FLAGS  "-g -O0 -Wall -Wextra -pedantic")
SET(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pedantic -lpthread -lboost_system -lboost_thread")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(include)
include_directories(src)

set(SOURCE_FILES
        include/async-kmer-counter.h
        include/kmer-counter.h
        include/fasta-parser.h
        include/fasta-iterator.h
        include/boost-thread-pool.h
        include/ostreamlock.h
        include/semaphore.h
        src/async-kmer-counter.cpp
        src/kmer-counter.cpp
        src/fasta-parser.cpp
        src/fasta-iterator.cpp
        src/boost-thread-pool.cpp
        src/ostreamlock.cc
        src/semaphore.cc
        src/main.cpp)

add_executable(count-kmers ${SOURCE_FILES})
target_link_libraries(count-kmers ${Boost_LIBRARIES})
