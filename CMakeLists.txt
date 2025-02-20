cmake_minimum_required(VERSION 3.16)
project(myLogger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ Find toml++ globally
find_package(tomlplusplus REQUIRED)

# ✅ Generate Config Package for `find_package(myLoggerLib)`
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        DESTINATION lib/cmake/myLoggerLib
)

option(ENABLE_TESTS "Build unit tests" ON)
option(ENABLE_BENCHMARKS "Build benchmarks" ON)

enable_testing()

# 🔹 Compiler warnings (Only for GCC/Clang)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ✅ Logger Library
add_library(myLoggerLib STATIC
        src/logger_config.cpp
        src/backends/console_backend.cpp
        src/backends/file_backend.cpp
)

# ✅ Proper Include Directories (Public API for external projects)
target_include_directories(myLoggerLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>  # Internal use
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/backends>  # for internal use
        $<INSTALL_INTERFACE:include>  # External use after installation
        $<INSTALL_INTERFACE:include/backends>  # External projects need this
)


# ✅ Link to globally installed `toml++`
target_link_libraries(myLoggerLib PUBLIC tomlplusplus::tomlplusplus)

# ✅ Install Rules (Fixes external usage)
install(TARGETS myLoggerLib EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT myLoggerLibConfig NAMESPACE myLogger:: DESTINATION lib/cmake/myLoggerLib)

# ✅ Generate Config Package for `find_package(myLoggerLib)`
write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
)
install(FILES "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        DESTINATION lib/cmake/myLoggerLib
)

# ✅ Add Subdirectories
add_subdirectory(demo)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()

if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

message(STATUS "Logging enabled: ${LOGGING_ENABLED}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Install command:
# sudo cmake --install cmake-build-release --prefix /usr/local
