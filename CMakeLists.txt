cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
message(STATUS "start running cmake...")

set(CMAKE_CXX_STANDARD 11)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

SET (BOOST_ROOT "/usr/local/lib/boost/boost_1_64_0")
SET (BOOST_INCLUDEDIR "/opt/boost/boost-1.57.0/boost")
SET (BOOST_LIBRARYDIR "/opt/boost/boost-1.57.0/lib")

find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
if(Boost_FOUND)
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
endif()

if(Boost_FOUND)
    target_link_libraries(BoostTest ${Boost_LIBRARIES})
endif()

include_directories("include")
include_directories(src)

set(SOURCE_FILES
        build/lib.macosx-10.7-x86_64-3.6/kmer_counter.cpython-36m-darwin.so
        build/temp.macosx-10.7-x86_64-3.6/src/kmer-counter.o
        include/kmer-counter.h
        src/kmers.cpp
        src/kmer-counter.cpp
        test/build/lib.macosx-10.7-x86_64-3.6/spam.cpython-36m-darwin.so
        test/build/temp.macosx-10.7-x86_64-3.6/spammodule.o
        test/python-spam-module/spammodule.c
        test/setup.py
        test/spammodule.c
        README.md
        setup.py)

target_link_libraries(kmer_counter.so ${SOURCE_FILES} ${PYTHON_LIBRARIES})