cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 11)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "DEBUG")
    #SET(CMAKE_BUILD_TYPE "RELEASE")
    #SET(CMAKE_BUILD_TYPE "RELWITHDEBINFO")
    #SET(CMAKE_BUILD_TYPE "MINSIZEREL")
ENDIF()

set(Python_ADDITIONAL_VERSIONS 3.6)

find_package(PythonLibs 3.6 REQUIRED)
find_package( PythonInterp 3.6 REQUIRED )
include_directories(${PYTHON_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost)
#find_package(Boost COMPONENTS python)
include_directories(${Boost_INCLUDE_DIR})

include_directories("include")
include_directories(src)

set(SOURCE_FILES
        include/kmer-counter.h
        src/kmers.cpp
        src/kmer-counter.cpp
        README.md
        setup.py)

#add_executable(kmer_counter.so ${SOURCE_FILES} ${PYTHON_LIBRARIES} ${Boost_LIBRARIES})

add_library(kmer_counter SHARED src/kmer-counter.cxx)
target_link_libraries(kmer_counter ${Boost_LIBRARIES})