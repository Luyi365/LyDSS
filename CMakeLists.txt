cmake_minimum_required(VERSION 3.10)
project(LyBOX VERSION 0.1.0 LANGUAGES C)

add_library(LyBOX LyBOX.c)

file(GLOB_RECURSE SOURCES_EXAMPLES examples/*.c)
add_executable(LyBOX_ExamplesTest test/run_all_examples.c ${SOURCES_EXAMPLES})
target_link_libraries(LyBOX_ExamplesTest LyBOX)

include(CTest)
enable_testing()
add_test(NAME LyBOX_ExamplesTest COMMAND LyBOX_ExamplesTest WORKING_DIRECTORY)
set_tests_properties(LyBOX_ExamplesTest PROPERTIES TIMEOUT 5)
