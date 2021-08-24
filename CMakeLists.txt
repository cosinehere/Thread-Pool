cmake_minimum_required(VERSION 3.12)

project(ThreadPool
    VERSION 1.0
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_executable(test
    ${CMAKE_SOURCE_DIR}/ThreadPool.cpp
    ${CMAKE_SOURCE_DIR}/test.cpp)

if(CMAKE_SYSTEM_NAME MATCHES Linux)
    target_link_libraries(test pthread)
endif()
