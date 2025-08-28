cmake_minimum_required(VERSION 3.10)
project(LyBKT VERSION 0.1.0 LANGUAGES C)

add_library(LyBKT lybkt.c)

file(GLOB_RECURSE SOURCES_EXAMPLES examples/*.c)
add_executable(LyBKT_ExamplesTest test/run_all_examples.c ${SOURCES_EXAMPLES})
target_link_libraries(LyBKT_ExamplesTest LyBKT)

include(CTest)
enable_testing()
add_test(NAME LyBKT_ExamplesTest COMMAND LyBKT_ExamplesTest WORKING_DIRECTORY)
set_tests_properties(LyBKT_ExamplesTest PROPERTIES TIMEOUT 5)

