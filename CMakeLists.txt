cmake_minimum_required(VERSION 3.5)
project(debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES znaczki.cpp)
add_executable(debug ${SOURCE_FILES})

set(FILES_TO_TEST znaczki.cpp)
set(UNIT_TESTS tests/tests.cpp )
set(MAIN_FILE tests/tests_main.cpp)

add_library(codeToTest ${FILES_TO_TEST})

add_executable(Proj2 ${MAIN_FILE})
target_link_libraries(Proj2 codeToTest)

add_executable(unit-test ${UNIT_TESTS})
target_link_libraries(unit-test gtest gtest_main rt pthread codeToTest)