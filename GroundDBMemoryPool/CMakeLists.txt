cmake_minimum_required(VERSION 3.14)
project(GroundDBMemoryPool)

include_directories(${CMAKE_SOURCE_DIR}/src/include)
include_directories(/usr/include/rdma)
# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Use verbose output
set(CMAKE_VERBOSE_MAKEFILE ON)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# An example of a client that uses the memory pool
add_executable(
  memory_pool_client
  src/main_client.cc
)

# An example of a server that uses the memory pool
add_executable(
  memory_pool_server
  src/main_server.cc
)

# Static library used by client to interact with server
add_library(
  rdma_client_lib
  src/rdma/rdma_client/connect.cc
  src/rdma/util.cc
  src/rdma/allocate_page.cc
  src/rdma/rdma_client/read.cc
  src/rdma/rdma_client/write.cc
)

# Static library used by server
add_library(
  rdma_server_lib
  src/rdma/rdma_server/init_server.cc
  src/rdma/util.cc
  src/rdma/allocate_page.cc
)

# Compile with -DSERVER for server
target_compile_definitions(
  rdma_server_lib PRIVATE SERVER
)
target_compile_definitions(
  memory_pool_server PRIVATE SERVER
)

target_link_libraries(rdma_client_lib PRIVATE ibverbs)
target_link_libraries(memory_pool_client PRIVATE ibverbs rdma_client_lib)

target_link_libraries(rdma_server_lib PRIVATE ibverbs)
target_link_libraries(memory_pool_server PRIVATE ibverbs rdma_server_lib)

add_executable(
  test_add
  src/add.cc
  src/test/add.test.cc
)

add_executable(
    test_lru
    src/test/lru/lru.test.cc
)

target_link_libraries(
  test_add
  GTest::gtest_main
)

target_link_libraries(
    test_lru
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(test_add)
gtest_discover_tests(test_lru)