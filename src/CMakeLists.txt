# CMakeList.txt : CMake project for chesspeer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
configure_file(config.h.in config.h)

# Add source to this project's executable.
add_executable (${PROJECT_NAME} "chesspeer.cpp" "chesspeer.h" "chessgame.h" "chessgame.cpp")

# TODO: Add tests and install targets if needed.
