cmake_minimum_required(VERSION 3.14)
project(GroundDBMemoryPool)

include_directories(${CMAKE_SOURCE_DIR}/src/include)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  memory_pool
  src/main.cc
  src/add.cc
)

add_executable(
  test_add
  src/add.cc
  src/test/add.test.cc
)

target_link_libraries(
  test_add
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_add)