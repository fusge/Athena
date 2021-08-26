# CMakeList.txt : CMake project for chesspeer, include source and define
# project specific logic here.
#

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

cmake_minimum_required (VERSION 3.8)
# Add source to this project's executable.
add_executable (${PROJECT_NAME} 
    "chesspeer.cpp" "chesspeer.h" 
    "chessgame.h" "chessgame.cpp"
    "ChesspeerConfig.h")

# TODO: Add tests and install targets if needed.
