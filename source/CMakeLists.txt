# CMakeList.txt : CMake project for athena, include source and define
# project specific logic here.
#

cmake_minimum_required (VERSION 3.25)
# Add source to this project's executable.

include(${wxWidgets_USE_FILE})

add_library(athena_lib 
    chessgame.h chessgame.cpp
    main_window.cpp main_window.h
)

if (MSVC)
    # wxWidgets complains on windows. Need proper compiler flags
    # or else it won't compile
    add_executable (athena_exe WIN32 athena.h athena.cpp)
else()
    add_executable (athena_exe athena.h athena.cpp)
endif()

add_executable(athena::exe ALIAS athena_exe)

set_property(TARGET athena_exe PROPERTY OUTPUT_NAME athena)

target_compile_features(athena_exe PRIVATE cxx_std_17)

target_link_libraries(athena_exe PRIVATE
    athena_lib
    ${wxWidgets_LIBRARIES}
)


