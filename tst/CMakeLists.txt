# CMakeList.txt : CMake project for chesspeer test
#
cmake_minimum_required (VERSION 3.13)

# Treat warnings as errors in all platforms
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

set(BINARY ${PROJECT_NAME}Tests)
set(SOURCES ${PROJECT_NAME}Tests.cpp)

# Add source to this project's executable.
add_executable (${BINARY}
    ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_BINARY_DIR}/lib/googletest/include
)


target_link_libraries(${BINARY} PUBLIC gtest gtest_main)
