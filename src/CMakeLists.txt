# CMakeList.txt : CMake project for athena, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.13)
# Add source to this project's executable.


if (MSVC)
    # wxWidgets complains on windows. Need proper compiler flags
    # or else it won't compile
    add_executable (${PROJECT_NAME} 
        WIN32
        "chessgame.h" "chessgame.cpp"
        "main_window.cpp" "main_window.h")
else()
    add_executable (${PROJECT_NAME} 
        "chessgame.h" "chessgame.cpp"
        "main_window.cpp" "main_window.h")
endif()


