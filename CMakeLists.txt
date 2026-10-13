cmake_minimum_required(VERSION 3.21)
project(LyDSS VERSION 0.1.0 LANGUAGES C)

# 语言基线 C11，以 C17 构建，不启用编译器扩展
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES src/*.c)
file(GLOB_RECURSE SOURCES_TEST test/*.c)

add_library(LyPool src/lypool.c)
add_library(LyArray src/lyarray.c)
add_library(LyVector src/lyvector.c)
add_library(LyPingPong src/lypingpong.c)
add_library(LyStack src/lystack.c)

add_library(LyQueue src/lyqueue.c)
add_library(LyList src/lylist.c)
add_library(LyStream src/lystream.c)
add_library(LyTable src/lytable.c)
add_library(LySet src/lyset.c)
add_library(LyBucket src/lybucket.c)

add_executable(LyDSS_SimpleTest examples/run_simple_test.c test/simple.c)
target_link_libraries(LyDSS_SimpleTest LyPool LyArray LyVector LyPingPong LyStack LyQueue LyList LyStream LyTable LySet LyBucket)

add_executable(LyDSS_AllBasicTest examples/run_all_basic_test.c ${SOURCES_TEST})
target_link_libraries(LyDSS_AllBasicTest LyPool LyArray LyVector LyPingPong LyStack LyQueue LyList LyStream LyTable LySet LyBucket)

include(CTest)
enable_testing()

add_test(NAME LyDSS_SimpleTest COMMAND LyDSS_SimpleTest)
set_tests_properties(LyDSS_SimpleTest PROPERTIES TIMEOUT 5)

add_test(NAME LyDSS_AllBasicTest COMMAND LyDSS_AllBasicTest)
set_tests_properties(LyDSS_AllBasicTest PROPERTIES TIMEOUT 5)
