cmake_minimum_required(VERSION 3.0)
project(breeze)

set(CMAKE_CXX_STANDARD 11)

add_library(breeze_thread_pool INTERFACE)
target_include_directories(breeze_thread_pool
  INTERFACE ${PROJECT_SOURCE_DIR}/include
)
add_library(Breeze::ThreadPool ALIAS breeze_thread_pool)

if(BUILD_TEST)
  add_subdirectory(3rd_party/google-test)
  enable_testing()

  file(GLOB TEST_SOURCES "test/*.cpp")
  add_executable(unit_tests ${TEST_SOURCES})
  target_link_libraries(unit_tests
    Breeze::ThreadPool
    GTest::GTest
    GTest::Main
  )

  add_test(test_all unit_tests)
endif()
