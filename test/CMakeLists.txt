# CMakeList.txt : CMake project for chesspeer test
#
cmake_minimum_required (VERSION 3.24)

enable_testing()
# Add source to this project's executable.
add_executable (AthenaTests
    athenaTests.cpp
)

target_link_libraries(AthenaTests PUBLIC gtest gtest_main)

add_test(
    NAME AtheanTests
    COMMAND AthenaTests
)
