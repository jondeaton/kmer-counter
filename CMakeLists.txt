cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 11)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "DEBUG")
    #SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

include_directories(include)
include_directories(src)

set(SOURCE_FILES
        include/kmer-counter.h
        src/kmer-counter.cpp)

add_executable(kmer_counter ${SOURCE_FILES})
target_link_libraries(kmer_counter ${Boost_LIBRARIES})
