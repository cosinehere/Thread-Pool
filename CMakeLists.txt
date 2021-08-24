cmake_minimum_required(VERSION 3.12)

project(ThreadPool
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(test
    ${CMAKE_SOURCE_DIR}/ThreadPool.cpp
    ${CMAKE_SOURCE_DIR}/test.cpp)

target_link_libraries(test pthread)
