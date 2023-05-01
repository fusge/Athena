cmake_minimum_required(VERSION 3.25)

project(
    Athena
    VERSION 0.1.0
    DESCRIPTION "A chess friend for your chess needs"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

# ---- Download Dependencies ----
include(FetchContent)
set(FetchContent_Quiet FALSE)

FetchContent_Declare(
    wxWidgets
    GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets
    GIT_TAG v3.2.2.1
    GIT_PROGRESS TRUE
    SYSTEM
)
FetchContent_GetProperties(wxWidgets)
FetchContent_MakeAvailable(wxWidgets)

FetchContent_Declare(
    GTest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.13.0
    GIT_PROGRESS TRUE
    SYSTEM
)
FetchContent_GetProperties(GTest)
FetchContent_MakeAvailable(GTest)

list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
#list(APPEND CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES lib)

include_directories(${PROJECT_BINARY_DIR})
configure_file(AthenaConfig.h.in AthenaConfig.h)

# windows users might have a hard time running tests
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

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
