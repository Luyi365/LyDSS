cmake_minimum_required(VERSION 3.10)
project(LyDDS VERSION 0.1.0 LANGUAGES C)

file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SOURCES_TEST test/*.c)

add_library(LyPool src/lypool.c)
add_library(LyQueue src/lyqueue.c)
add_library(LyStack src/lystack.c)
add_library(LyList src/lylist.c)
add_library(LyStream src/lystream.c)
add_library(LyTable src/lytable.c)
add_library(LySet src/lyset.c)
add_library(LyBucket src/lybucket.c)

add_executable(LyDDS_SimpleTest examples/run_simple_test.c test/simple.c)
target_link_libraries(LyDDS_SimpleTest LyPool LyQueue LyStack LyList LyStream LyTable LySet LyBucket)

add_executable(LyDDS_AllBasicTest examples/run_all_basic_test.c ${SOURCES_TEST})
target_link_libraries(LyDDS_AllBasicTest LyPool LyQueue LyStack LyList LyStream LyTable LySetzhami LyBucket)

include(CTest)
enable_testing()

add_test(NAME LyDDS_SimpleTest COMMAND LyDDS_SimpleTest)
set_tests_properties(LyDDS_SimpleTest PROPERTIES TIMEOUT 5)

add_test(NAME LyDDS_AllBasicTest COMMAND LyDDS_AllBasicTest)
set_tests_properties(LyDDS_AllBasicTest PROPERTIES TIMEOUT 5)
