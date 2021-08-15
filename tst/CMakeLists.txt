# CMakeList.txt : CMake project for chesspeer test
#
cmake_minimum_required (VERSION 3.8)

set(BINARY ${PROJECT_NAME}Tests)
set(SOURCES ${PROJECT_NAME}Tests.cpp)

# Add source to this project's executable.
add_executable (${BINARY}
    ${SOURCES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC ${PROJECT_BINARY_DIR}/lib/googletest/include
)

target_link_libraries(${BINARY} PUBLIC gtest)
