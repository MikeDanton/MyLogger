cmake_minimum_required(VERSION 3.20)
project(myLogger VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(GNUInstallDirs)

# --- toml++ include dir discovery (pkg-config -> vcpkg -> vendored) ---
set(TOMLPP_INCLUDE_DIR "")

find_package(PkgConfig QUIET)
if (PkgConfig_FOUND)
    pkg_check_modules(tomlplusplus QUIET IMPORTED_TARGET tomlplusplus)
    if (TARGET PkgConfig::tomlplusplus)
        # pkg-config provides include dirs
        set(TOMLPP_INCLUDE_DIR "${tomlplusplus_INCLUDE_DIRS}")
    endif()
endif()

if (NOT TOMLPP_INCLUDE_DIR)
    # vcpkg toolchain exposes VCPKG_INSTALLED_DIR + VCPKG_TARGET_TRIPLET
    if (DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET AND
            EXISTS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/toml++/toml.hpp")
        set(TOMLPP_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    elseif (EXISTS "${CMAKE_SOURCE_DIR}/external/tomlplusplus/include/toml++/toml.hpp")
        # fallback to vendored submodule
        set(TOMLPP_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/tomlplusplus/include")
    else()
        message(FATAL_ERROR
                "tomlplusplus not found.\n"
                "Install via vcpkg (recommended) or add submodule at external/tomlplusplus.\n"
                "If using vcpkg, configure with:\n"
                "  -DCMAKE_TOOLCHAIN_FILE=C:/Koodaus/vcpkg/scripts/buildsystems/vcpkg.cmake")
    endif()
endif()

message(STATUS "toml++ include dir: ${TOMLPP_INCLUDE_DIR}")

# --- sources ---
set(SRC_COMMON
        src/logger_config.cpp
        src/backends/file_backend.cpp
)

if (WIN32)
    set(SRC_CONSOLE src/backends/console_backend_win.cpp)  # Windows-specific TU
else()
    set(SRC_CONSOLE src/backends/console_backend.cpp)      # Linux/original TU
endif()

# Give debug libs a 'd' suffix: myLoggerLibd.lib
set(CMAKE_DEBUG_POSTFIX d)
add_library(myLoggerLib STATIC
        ${SRC_COMMON}
        ${SRC_CONSOLE}
)

# Public headers of your library
target_include_directories(myLoggerLib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${TOMLPP_INCLUDE_DIR}>   # ← build-only; not exported
)

# No link to toml++ (header-only) -> avoids export errors

if (MSVC)
    target_compile_definitions(myLoggerLib PRIVATE _WIN32_WINNT=0x0A00 NOMINMAX _CRT_SECURE_NO_WARNINGS)
    target_compile_options(myLoggerLib PRIVATE /W4 /permissive- /EHsc /Zc:preprocessor /utf-8)
else()
    target_compile_definitions(myLoggerLib PRIVATE _POSIX_C_SOURCE=200809L)
    target_compile_options(myLoggerLib PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- install/export ---
install(TARGETS myLoggerLib EXPORT myLoggerLibConfig
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/myLogger/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/myLogger
        FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT myLoggerLibConfig
        NAMESPACE myLogger::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/myLoggerLib
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
install(FILES "${CMAKE_BINARY_DIR}/myLoggerLibConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/myLoggerLib
)

# Options for your subdirs
option(ENABLE_TESTS "Build unit tests" OFF)
option(ENABLE_BENCHMARKS "Build benchmarks" OFF)
option(ENABLE_DEMOS "Build demo applications" OFF)
if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
if (ENABLE_DEMOS)
    add_subdirectory(demos)
endif()
