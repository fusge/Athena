# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.25)

include(cmake/prelude.cmake)

project(
    athena 
    VERSION 0.1.0
    DESCRIPTION "A chess friend for your chess needs"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
#list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES lib)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

include_directories(${PROJECT_BINARY_DIR})
configure_file(AthenaConfig.h.in AthenaConfig.h)

# windows users might have a hard time running tests
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
find_package(GTest REQUIRED)

# Include sub-projects.
add_subdirectory(source)
add_subdirectory(data)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT athena_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of athena"
  )
endif()

include(cmake/dev-mode.cmake)
