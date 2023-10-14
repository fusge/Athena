# CMakeList.txt : CMake project for athena, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.25)
# Add source to this project's executable.

add_library(athena_lib 
    chessgame.h chessgame.cpp
)

if (MSVC)
    # wxWidgets complains on windows. Need proper compiler flags
    # or else it won't compile
    add_executable (Athena_exe WIN32 athena.h athena.cpp)
else()
    add_executable (Athena_exe athena.h athena.cpp)
endif()

add_executable(athena::exe ALIAS Athena_exe)

set_property(TARGET Athena_exe PROPERTY OUTPUT_NAME athena)

target_compile_features(Athena_exe PRIVATE cxx_std_20)

target_link_libraries(Athena_exe PRIVATE
    athena_lib
)


