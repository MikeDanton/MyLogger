cmake_minimum_required(VERSION 3.16)
project(myLogger)

set(CMAKE_CXX_STANDARD 20)

option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)

# 🔹 Handle Logging flag (default: enabled in Debug mode, disabled in Release)
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-DLOGGING_ENABLED=0)
else()
    add_definitions(-DLOGGING_ENABLED=1)
endif()

# 🔹 Add toml++ as a header-only dependency
add_library(tomlplusplus INTERFACE)
target_include_directories(tomlplusplus INTERFACE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/tomlplusplus/include>
        $<INSTALL_INTERFACE:include>
)

# 🔹 Define the myLoggerLib library
add_library(myLoggerLib STATIC
        src/logger.cpp
        src/logger_config.cpp
        src/console_backend.cpp
        src/file_backend.cpp
)

target_include_directories(myLoggerLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(myLoggerLib PRIVATE tomlplusplus)

# 🔹 Install rules
install(TARGETS myLoggerLib tomlplusplus EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)
install(EXPORT myLoggerLibConfig DESTINATION lib/cmake/myLoggerLib)

# 🔹 Demo (Example Usage)
add_subdirectory(demo)

# 🔹 Tests (if enabled)
if (ENABLE_TESTS)
    enable_testing()
    add_executable(myLoggerTests tests/test_logger.cpp)
    target_link_libraries(myLoggerTests PRIVATE myLoggerLib gtest gtest_main)
    add_test(NAME LoggerTests COMMAND myLoggerTests)
endif()

# 🔹 Benchmarks (if enabled)
if (ENABLE_BENCHMARKS)
    find_package(benchmark REQUIRED)

    add_executable(benchmark_logger benchmarks/benchmark_logger.cpp)
    target_link_libraries(benchmark_logger PRIVATE myLoggerLib benchmark::benchmark)

    add_executable(benchmark_config benchmarks/benchmark_config.cpp)
    target_link_libraries(benchmark_config PRIVATE myLoggerLib benchmark::benchmark)
endif()

# 🔹 Debugging Information
message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")