# CMakeList.txt : CMake project for chesspeer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source to this project's executable.
add_executable (chesspeer "chesspeer.cpp" "chesspeer.h" "chessboard.h" "chessboard.cpp")

# TODO: Add tests and install targets if needed.
