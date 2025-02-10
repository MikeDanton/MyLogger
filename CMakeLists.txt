cmake_minimum_required(VERSION 3.16)
project(myLogger)

set(CMAKE_CXX_STANDARD 20)

# ✅ Add build options to disable tests and benchmarks by default
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)

# ✅ Create the logger library
add_library(myLoggerLib STATIC
        src/logger.cpp
        src/consoleBackend.cpp
        src/fileBackend.cpp
        src/loggerManager.cpp
        src/loggerConfig.cpp)

target_include_directories(myLoggerLib PUBLIC include)

# ✅ Install the library
install(TARGETS myLoggerLib
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# ✅ Only build main executable if needed
add_executable(myLoggerApp demo/main.cpp demo/taskManager.cpp)
target_link_libraries(myLoggerApp PRIVATE myLoggerLib)

# ✅ Only build tests if ENABLE_TESTS is ON
if (ENABLE_TESTS)
    enable_testing()
    add_executable(myLoggerTests tests/test_logger.cpp demo/taskManager.cpp)
    target_link_libraries(myLoggerTests PRIVATE myLoggerLib gtest gtest_main)
    add_test(NAME LoggerTests COMMAND myLoggerTests)
endif()

# ✅ Only build benchmarks if ENABLE_BENCHMARKS is ON
if (ENABLE_BENCHMARKS)
    find_package(benchmark REQUIRED)

    add_executable(benchmark_logger benchmarks/benchmark_logger.cpp)
    target_link_libraries(benchmark_logger PRIVATE myLoggerLib benchmark::benchmark)

    add_executable(benchmark_config benchmarks/benchmark_config.cpp)
    target_link_libraries(benchmark_config PRIVATE myLoggerLib benchmark::benchmark)
endif()
