cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 17)

#SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -pedantic")
SET(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pedantic")

find_package(Boost 1.65 REQUIRED)
IF (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
ENDIF()


include_directories(include)
include_directories(src)

set(SOURCE_FILES
        include/async-kmer-counter.h
        include/kmer-counter.h
        include/fasta-parser.h
        include/fasta-iterator.h
        include/boost-thread-pool.h
        include/ostreamlock.h
        src/async-kmer-counter.cpp
        src/kmer-counter.cpp
        src/fasta-parser.cpp
        src/fasta-iterator.cpp
        src/boost-thread-pool.cpp
        src/ostreamlock.cc
        src/main.cpp)

add_executable(count-kmers ${SOURCE_FILES})
if(APPLE OR WIN32)
    target_link_libraries(count-kmers pthread boost_thread-mt boost_system-mt boost_filesystem-mt)
#    target_link_libraries(count-kmers pthread boost_thread boost_system boost_filesystem)
else()
    target_link_libraries(count-kmers pthread boost_thread boost_system boost_filesystem)
endif()
#target_link_libraries(count-kmers ${Boost_LIBRARIES})


set(TEST_SOURCE
        include/batch-processor.h
        src/batch-processor.cpp
        test/test-mpi.cpp)

set(CMAKE_CXX_COMPILER "/opt/openmpi-3.0.0/bin/mpic++")
find_package ( MPI )
if ( MPI_FOUND )
    include_directories( ${MPI_INCLUDE_PATH} )
endif( MPI_FOUND )
add_executable(test-mpi ${TEST_SOURCE})