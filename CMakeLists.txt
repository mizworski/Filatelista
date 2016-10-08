cmake_minimum_required(VERSION 3.5)
project(cpptest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES znaczki.cpp)
add_executable(cpptest ${SOURCE_FILES})