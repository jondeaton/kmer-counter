cmake_minimum_required(VERSION 2.8)
project(Kmer-Counter)
set(CMAKE_CXX_STANDARD 11)

include_directories(src include boost)

find_package(Boost 1.54 REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions("-DHAS_BOOST")

    set(Boost_USE_STATIC ON)
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME ON)
endif()

############################
#       local build        #
############################

# cmake -DCMAKE_BUILD_TYPE=Debug ..
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Mode: Debug")
    SET(CMAKE_CXX_FLAGS "-std=c++11 -g -O0 -Wall -fpermissive -Wextra -pedantic -DBOOST_LOG_DYN_LINK")
    if (APPLE)
        # LLDB needs executables to end with .o I guess...?
        SET(LOC_EXECUTABLE  "count-kmers.o")
        SET(DIST_EXECUTABLE "count-kmers-dist.o")
    else()
        SET(LOC_EXECUTABLE "count-kmers")
        SET(DIST_EXECUTABLE "count-kmers-dist")
    endif()
else()
    message(STATUS "Mode: Release")
    # Release mode
    SET(CMAKE_CXX_FLAGS "-std=c++11 -Ofast -fpermissive -DBOOST_LOG_DYN_LINK")
    SET(LOC_EXECUTABLE  "count-kmers")
    SET(DIST_EXECUTABLE "count-kmers-dist")
endif()

set(SOURCE_FILES
        boost/threadpool.hpp
        include/local-kmer-counter.hpp          src/local-kmer-counter.cpp
        include/async-kmer-counter.hpp          src/async-kmer-counter.cpp
        include/kmer-counter.hpp                src/kmer-counter.cpp
        include/fasta-parser.hpp                src/fasta-parser.cpp
        include/fasta-iterator.hpp              src/fasta-iterator.cpp
        include/ostreamlock.hpp                 src/ostreamlock.cc
        
        src/main-local.cpp)

add_executable(${LOC_EXECUTABLE} ${SOURCE_FILES})
if(APPLE OR WIN32)

    target_link_libraries(${LOC_EXECUTABLE}
            pthread
            boost_thread-mt
            boost_system-mt
            boost_filesystem-mt
            boost_log-mt
            boost_log_setup-mt
            boost_chrono-mt
            boost_date_time-mt
            boost_program_options-mt
            boost_regex-mt)
else()
    target_link_libraries(${LOC_EXECUTABLE}
            pthread
            boost_thread
            boost_system
            boost_filesystem
            boost_log
            boost_log_setup
            boost_chrono
            boost_date_time
            boost_program_options
            boost_regex)
endif()

############################
#       Dist build        #
############################
find_package(MPI REQUIRED)
if (MPI_FOUND)
    if (APPLE)
    else()
        message(STATUS "Setting C++ compiler to: mpic++")
        set(CMAKE_CXX_COMPILER mpic++)
    endif(APPLE)
    include_directories(${MPI_INCLUDE_PATH})

    set(MPI_SOURCES
            boost/threadpool.hpp
            include/batch-processor.hpp             src/batch-processor.cpp
            include/distributed-kmer-counter.hpp    src/distributed-kmer-counter.cpp
            include/async-kmer-counter.hpp          src/async-kmer-counter.cpp
            include/kmer-counter.hpp                src/kmer-counter.cpp
            include/fasta-parser.hpp                src/fasta-parser.cpp
            include/fasta-iterator.hpp              src/fasta-iterator.cpp
            include/ostreamlock.hpp                 src/ostreamlock.cc
            src/main-distributed.cpp)

    add_executable(${DIST_EXECUTABLE} ${MPI_SOURCES})
    target_link_libraries(${DIST_EXECUTABLE} ${MPI_LIBRARIES})

    if(MPI_COMPILE_FLAGS)
        set_target_properties(${DIST_EXECUTABLE} PROPERTIES COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
    endif()

    if(MPI_LINK_FLAGS)
        set_target_properties(${DIST_EXECUTABLE} PROPERTIES LINK_FLAGS "${MPI_LINK_FLAGS}")
    endif()

    if(APPLE OR WIN32)
        target_link_libraries(${DIST_EXECUTABLE} pthread boost_thread-mt boost_system-mt boost_filesystem-mt)
    else()
        target_link_libraries(${DIST_EXECUTABLE} pthread boost_thread boost_system boost_filesystem)
    endif()

    if(APPLE OR WIN32)
        target_link_libraries(${DIST_EXECUTABLE}
                pthread boost_thread-mt
                boost_system-mt
                boost_filesystem-mt
                boost_log-mt
                boost_log_setup-mt
                boost_chrono-mt
                boost_date_time-mt
                boost_program_options-mt
                boost_regex-mt)
    else()
        target_link_libraries(${DIST_EXECUTABLE}
                pthread boost_thread
                boost_system
                boost_filesystem
                boost_log
                boost_log_setup
                boost_chrono
                boost_date_time
                boost_program_options
                boost_regex)
    endif()

else ()
    message(WARNING "Couldn't find MPI. Not building distributed k-mer counter.")
endif ()
