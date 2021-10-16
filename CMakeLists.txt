# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.13)

project ("Chesspeer" VERSION 0.1.0 LANGUAGES CXX)

# Include sub-projects.
add_subdirectory("lib")
add_subdirectory("src")
add_subdirectory("tst")
add_subdirectory("data")

# windows users might have a hard time running tests
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Installation targets
install(TARGETS Chesspeer DESTINATION bin)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(${PROJECT_SOURCE_DIR}/src/ChesspeerConfig.h.in 
    ${PROJECT_BINARY_DIR}/src/ChesspeerConfig.h)

target_include_directories(${PROJECT_NAME} 
    PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} 
    wx::net
    wx::core
    wx::base
)



