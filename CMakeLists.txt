cmake_minimum_required(VERSION 3.16)
project(myLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔹 Options
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)

# 🔹 Warnings & Compiler Flags
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    #add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 🔹 Logging Flag: Enable in Debug mode, disable in Release
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(LOGGING_ENABLED=0)
else()
    add_compile_definitions(LOGGING_ENABLED=1)
endif()

# 🔹 External Dependencies (toml++)
set(TOML_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/tomlplusplus/include)
add_library(tomlplusplus INTERFACE)
target_include_directories(tomlplusplus INTERFACE
        $<BUILD_INTERFACE:${TOML_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)

# 🔹 myLoggerLib Library
add_library(myLoggerLib STATIC
        src/logger_config.cpp
        src/console_backend.cpp
        src/file_backend.cpp
        src/file_watcher.cpp
        include/file_watcher.hpp
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

# 🔹 Subdirectories
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

# 🔹 Debugging Info
message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Toml++ include directory: ${TOML_INCLUDE_DIR}")
