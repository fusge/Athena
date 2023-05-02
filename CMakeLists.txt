cmake_minimum_required(VERSION 3.25)

project(
    Athena
    VERSION 0.1.0
    DESCRIPTION "A chess friend for your chess needs"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

# ---- Declare Dependencies ----
find_package(wxWidgets REQUIRED COMPONENTS base core net)

list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include_directories(${PROJECT_BINARY_DIR})
configure_file(AthenaConfig.h.in AthenaConfig.h)

include(cmake/prelude.cmake)
include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# Include sub-projects.
add_subdirectory("source")
add_subdirectory("data")

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT Athena_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of Athena"
  )
endif()

include(cmake/dev-mode.cmake)
