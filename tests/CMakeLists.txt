cmake_minimum_required(VERSION 3.13)

include(CTest)

if(NOT BUILD_TESTING)
  return()
endif()

include(FetchContent)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(engine_tests
  EcsAssetTests.cpp
)

target_link_libraries(engine_tests
  PRIVATE
    ENTITY
    EVENTS
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(engine_tests)
