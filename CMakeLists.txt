cmake_minimum_required(VERSION 3.4)
project(crow)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp)
add_executable(crow ${SOURCE_FILES})