cmake_minimum_required(VERSION 3.10)
project(LyDDS VERSION 0.1.0 LANGUAGES C)

file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SOURCES_EXAMPLES examples/*.c)

add_library(LyPOOL src/lypool.c)
add_library(LyQUEUE src/lyqueue.c)
add_library(LySET src/lyset.c)
add_library(LyBucket src/lybucket.c)

add_executable(LyDDS_SimpleTest test/run_simple_examples.c examples/simple.c)
target_link_libraries(LyDDS_SimpleTest LyPOOL LyQUEUE LySET LyBucket)

add_executable(LyDDS_ExamplesTest test/run_all_basic_examples.c ${SOURCES_EXAMPLES})
target_link_libraries(LyDDS_ExamplesTest LyPOOL LyQUEUE LySET LyBucket)

include(CTest)
enable_testing()

add_test(NAME LyDDS_SimpleTest COMMAND LyDDS_SimpleTest)
set_tests_properties(LyDDS_SimpleTest PROPERTIES TIMEOUT 5)

add_test(NAME LyDDS_ExamplesTest COMMAND LyDDS_ExamplesTest)
set_tests_properties(LyDDS_ExamplesTest PROPERTIES TIMEOUT 5)
