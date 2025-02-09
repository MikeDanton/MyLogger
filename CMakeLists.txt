cmake_minimum_required(VERSION 3.30)
project(myLogger)

set(CMAKE_CXX_STANDARD 20)

# Create the logger static library
add_library(myLoggerLib STATIC
        src/logger.cpp
        src/logBackend.cpp
        src/fileBackend.cpp
        include/fileBackend.hpp
)

# Create the executable
add_executable(myLogger main.cpp)
target_link_libraries(myLogger PRIVATE myLoggerLib)

# Include directories for main executable
target_include_directories(myLogger PRIVATE ${CMAKE_SOURCE_DIR})

# Enable testing
enable_testing()

# GoogleTest Integration
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(myLoggerTests tests/test_logger.cpp)

# ✅ Fix: Explicitly add include directories for tests
target_include_directories(myLoggerTests PRIVATE ${CMAKE_SOURCE_DIR})

# Link tests with myLoggerLib and GoogleTest
target_link_libraries(myLoggerTests PRIVATE myLoggerLib gtest gtest_main)

# Register tests
add_test(NAME LoggerTests COMMAND myLoggerTests)
