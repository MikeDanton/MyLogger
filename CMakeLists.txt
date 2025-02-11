cmake_minimum_required(VERSION 3.16)
project(myLogger)

set(CMAKE_CXX_STANDARD 20)

# ✅ Add build options
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)

add_subdirectory(demo)

# ✅ Automatically disable logging in Release mode and enable it in Debug mode
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DLOGGING_ENABLED=0)
else()
    add_definitions(-DLOGGING_ENABLED=1)
endif()

# ✅ Create the logger library
add_library(myLoggerLib STATIC
        src/consoleBackend.cpp
        src/fileBackend.cpp
        src/loggerManager.cpp
        src/loggerConfig.cpp
        src/loggerSettings.cpp)

target_include_directories(myLoggerLib PUBLIC include)

# ✅ Export the library to be found by external projects (e.g., `demo/`)
install(TARGETS myLoggerLib EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

export(EXPORT myLoggerLibConfig FILE "${CMAKE_BINARY_DIR}/myLoggerLibConfig.cmake")

# ✅ Build tests only if enabled
if (ENABLE_TESTS)
    enable_testing()
    add_executable(myLoggerTests tests/test_logger.cpp)
    target_link_libraries(myLoggerTests PRIVATE myLoggerLib gtest gtest_main)
    add_test(NAME LoggerTests COMMAND myLoggerTests)
endif()

# ✅ Build benchmarks only if enabled
if (ENABLE_BENCHMARKS)
    find_package(benchmark REQUIRED)

    add_executable(benchmark_logger benchmarks/benchmark_logger.cpp)
    target_link_libraries(benchmark_logger PRIVATE myLoggerLib benchmark::benchmark)

    add_executable(benchmark_config benchmarks/benchmark_config.cpp)
    target_link_libraries(benchmark_config PRIVATE myLoggerLib benchmark::benchmark)
endif()

# ✅ Print status messages
message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
