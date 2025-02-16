cmake_minimum_required(VERSION 3.16)
project(myLogger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" ON)

# 🔹 Force Benchmark not to use or fetch GTest
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)
set(BENCHMARK_USE_BUNDLED_GTEST OFF CACHE BOOL "" FORCE)
set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "" FORCE)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(LOGGING_ENABLED=0)
else()
    add_compile_definitions(LOGGING_ENABLED=1)
endif()

add_library(myLoggerLib STATIC
        src/logger_config.cpp
        src/backends/console_backend.cpp
        src/backends/file_backend.cpp
        src/format_module.cpp
        src/color_module.cpp
)

target_include_directories(myLoggerLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/backends>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/external>
        $<INSTALL_INTERFACE:include>
)

# 🔹 If not using toml++, remove it or set up as needed
set(TOML_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/external/tomlplusplus)
add_library(tomlplusplus INTERFACE)
target_include_directories(tomlplusplus INTERFACE
        $<BUILD_INTERFACE:${TOML_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(myLoggerLib PUBLIC tomlplusplus)

install(TARGETS myLoggerLib tomlplusplus EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT myLoggerLibConfig DESTINATION lib/cmake/myLoggerLib)

add_subdirectory(demo)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()

if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
