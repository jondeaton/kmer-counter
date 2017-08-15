cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 11)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "DEBUG")
    #SET(CMAKE_BUILD_TYPE "RELEASE")
ENDIF()

set(Python_ADDITIONAL_VERSIONS 3.6)

find_package(PythonLibs 3.6 REQUIRED)
find_package( PythonInterp 3.6 REQUIRED )
include_directories(${PYTHON_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIR})

include_directories(include)
include_directories(src)

set(SOURCE_FILES
        include/kmer-counter.h
        include/kmers.h
        src/kmer-counter.cpp
        src/kmers.cpp)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fPIC")
SET(GCC_COVERAGE_LINK_FLAGS "-I/usr/include/python3.6")

add_executable(kmer_counter.so ${SOURCE_FILES})
target_link_libraries(kmer_counter.so ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

