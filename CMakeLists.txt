cmake_minimum_required(VERSION 3.30)
project(myLogger)

set(CMAKE_CXX_STANDARD 20)

# Create the logger library
add_library(myLoggerLib STATIC
        src/logger.cpp
        src/consoleBackend.cpp
        src/fileBackend.cpp
        src/loggerManager.cpp
)

target_include_directories(myLoggerLib PUBLIC include)

# Create the main executable using the library
add_executable(myLoggerApp demo/main.cpp
        demo/taskManager.cpp)
target_link_libraries(myLoggerApp PRIVATE myLoggerLib)

# Add Google Test for unit testing
enable_testing()
add_executable(myLoggerTests tests/test_logger.cpp
        demo/taskManager.cpp)
target_link_libraries(myLoggerTests PRIVATE myLoggerLib gtest gtest_main)
add_test(NAME LoggerTests COMMAND myLoggerTests)
