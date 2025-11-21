cmake_minimum_required(VERSION 3.10)

project(MemoryManager C CXX)

enable_testing()

add_library(memory memory.c)

add_executable(tests tests.cpp)

target_link_libraries(tests memory UnitTest++)

add_test(NAME TestAll COMMAND tests)
