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
        include/async-kmer-counter.h        src/async-kmer-counter.cpp
        include/kmer-counter.h              src/kmer-counter.cpp
        include/fasta-parser.h              src/fasta-parser.cpp
        include/fasta-iterator.h            src/fasta-iterator.cpp
        include/boost-thread-pool.h         src/boost-thread-pool.cpp
        include/ostreamlock.h               src/ostreamlock.cc
        src/main-local.cpp)

add_executable(count-kmers ${SOURCE_FILES})
if(APPLE OR WIN32)
    target_link_libraries(count-kmers pthread boost_thread-mt boost_system-mt boost_filesystem-mt boost_program_options-mt)
else()
    target_link_libraries(count-kmers pthread boost_thread boost_system boost_filesystem boost_program_options)
endif()


# Distributed Target
find_package(MPI REQUIRED)
if (MPI_FOUND)
    set(CMAKE_CXX_COMPILER "/opt/openmpi-3.0.0/bin/mpic++")
        include_directories(${MPI_INCLUDE_PATH})

    set(MPI_SOURCES
            include/batch-processor.h               src/batch-processor.cpp
            include/distributed-kmer-counter.h      src/distributed-kmer-counter.cpp
            include/async-kmer-counter.h            src/async-kmer-counter.cpp
            include/kmer-counter.h                  src/kmer-counter.cpp
            include/fasta-parser.h                  src/fasta-parser.cpp
            include/fasta-iterator.h                src/fasta-iterator.cpp
            include/boost-thread-pool.h             src/boost-thread-pool.cpp
            include/ostreamlock.h                   src/ostreamlock.cc
            src/main-distributed.cpp)

    add_executable(count-kmer-dist ${MPI_SOURCES})
    target_link_libraries(count-kmer-dist ${MPI_LIBRARIES})

    if(MPI_COMPILE_FLAGS)
        set_target_properties(count-kmer-dist PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()

    if(MPI_LINK_FLAGS)
        set_target_properties(count-kmer-dist PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()

    if(APPLE OR WIN32)
        target_link_libraries(count-kmer-dist pthread boost_thread-mt boost_system-mt boost_filesystem-mt)
    else()
        target_link_libraries(count-kmer-dist pthread boost_thread boost_system boost_filesystem)
    endif()

    if(APPLE OR WIN32)
        target_link_libraries(count-kmer-dist pthread boost_thread-mt boost_system-mt boost_filesystem-mt boost_program_options-mt)
    else()
        target_link_libraries(count-kmer-dist pthread boost_thread boost_system boost_filesystem boost_program_options)
    endif()

else ()
    message(WARNING "Couldn't find MPI. Not building distributed k-mer counter.")
endif ()