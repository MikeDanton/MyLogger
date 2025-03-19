cmake_minimum_required(VERSION 3.16)
project(myLogger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find toml++ globally
find_package(tomlplusplus REQUIRED)

# Logger Library
add_library(myLoggerLib STATIC
        src/logger_config.cpp
        src/backends/console_backend.cpp
        src/backends/file_backend.cpp
)

# Proper Include Directories (Local and Installed)
target_include_directories(myLoggerLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>    # Local builds (fixes IDE include errors)
        $<INSTALL_INTERFACE:include>                      # External use after install (`/usr/local/include/myLogger/`)
)

# Ensure toml++ propagates correctly
target_link_libraries(myLoggerLib PUBLIC tomlplusplus::tomlplusplus)

# Export toml++ in package config (so it's properly linked in projects using myLoggerLib)
install(TARGETS myLoggerLib EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)

install(DIRECTORY include/myLogger/ DESTINATION include/myLogger FILES_MATCHING PATTERN "*.hpp")

# Ensure toml++ is correctly passed to dependent projects
install(EXPORT myLoggerLibConfig
        NAMESPACE myLogger::
        DESTINATION lib/cmake/myLoggerLib
)

# Generate CMake Config Package
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        DESTINATION lib/cmake/myLoggerLib
)

# Print Debug Information
message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "toml++ linked to myLoggerLib")

# Optional Subdirectories (Unit Tests, Benchmarks, Demos)
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)
option(ENABLE_DEMOS "Build demo applications" ON)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()

if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if (ENABLE_DEMOS)
    add_subdirectory(demos)
endif()

# Install command:
# sudo cmake --install cmake-build-release --prefix /usr/local
