cmake_minimum_required(VERSION 3.10)
project(MemoryManager C)

enable_testing()

add_library(memory memory.c)

add_executable(tests tests.c)
target_link_libraries(tests memory)
add_test(NAME TestAll COMMAND tests)
