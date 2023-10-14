# CMakeList.txt : CMake project for chesspeer test
#
# Parent project does not export its library target, so this CML implicitly
# depends on being added from it, i.e. the testing is done only from the build
# tree and is not feasible from an install location

project(AthenaTests LANGUAGES CXX)

# ---- Dependencies ----

find_package(Catch2 REQUIRED)
include(Catch)

# ---- Tests ----

add_executable(athena_test athenaTests.cpp)
target_link_libraries(
    athena_test PRIVATE
    athena_lib
    Catch2::Catch2WithMain
)
target_compile_features(athena_test PRIVATE cxx_std_20)

catch_discover_tests(athena_test)

# ---- End-of-file commands ----

add_folders(Test)
