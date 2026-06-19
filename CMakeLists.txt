cmake_minimum_required(VERSION 3.14)
project(OnlineVotingSystemTests CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Google Test
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Prevent GTest from overriding compiler settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Fetch Catch2 v2 (v2.13.10 is the latest stable v2 release, which is simple and headers-friendly)
FetchContent_Declare(
  catch2
  URL https://github.com/catchorg/Catch2/archive/refs/tags/v2.13.10.zip
)
FetchContent_MakeAvailable(catch2)

enable_testing()

# Add Catch2 tests executable
add_executable(catch2_tests catch2_tests.cpp)
target_include_directories(catch2_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(catch2_tests PRIVATE Catch2::Catch2)
add_test(NAME catch2_tests COMMAND catch2_tests)

# Add GTest tests executable (links to gtest since we defined our own main in gtest_tests.cpp)
add_executable(gtest_tests gtest_tests.cpp)
target_include_directories(gtest_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(gtest_tests PRIVATE gtest)
add_test(NAME gtest_tests COMMAND gtest_tests)
