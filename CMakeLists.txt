cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 11)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

include_directories("include")
include_directories(src)

set(SOURCE_FILES
        include/kmer-counter.h
        src/kmers.cpp
        src/kmer-counter.cpp
        README.md
        setup.py)

add_executable(kmer_counter ${SOURCE_FILES} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})