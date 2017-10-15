cmake_minimum_required(VERSION 3.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(include)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -pedantic")
else()
    SET(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra -pedantic")
endif()

find_package(Boost 1.65 REQUIRED)
if (Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
endif()

set(SOURCE_FILES
        include/async-kmer-counter.h        include/kmer-counter.h
        include/fasta-parser.h              include/fasta-iterator.h
        include/boost-thread-pool.h         include/ostreamlock.h
        src/async-kmer-counter.cpp          src/kmer-counter.cpp
        src/fasta-parser.cpp                src/fasta-iterator.cpp
        src/boost-thread-pool.cpp           src/ostreamlock.cc
        src/main.cpp)

add_executable(count-kmers ${SOURCE_FILES})
if(APPLE OR WIN32)
    target_link_libraries(count-kmers pthread boost_thread-mt boost_system-mt boost_filesystem-mt)
else()
    target_link_libraries(count-kmers pthread boost_thread boost_system boost_filesystem)
endif()
#target_link_libraries(count-kmers ${Boost_LIBRARIES})


find_package(MPI REQUIRED)
if (MPI_FOUND)
    set(CMAKE_CXX_COMPILER "/opt/openmpi-3.0.0/bin/mpic++")
    include_directories(${MPI_INCLUDE_PATH})

    set(MPI_TEST_SOURCE
            include/batch-processor.h src/batch-processor.tpp
            include/distributed-kmer-counter.h src/distributed-kmer-counter.cpp
            test/test-mpi.cpp)

    add_executable(test-mpi ${MPI_TEST_SOURCE})
    target_link_libraries(test-mpi ${MPI_LIBRARIES})

    if(MPI_COMPILE_FLAGS)
        set_target_properties(test-mpi PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()

    if(MPI_LINK_FLAGS)
        set_target_properties(test-mpi PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()

else ()
    message(WARNING "Couldn't find MPI, not building distributed k-mer counter")
endif ()