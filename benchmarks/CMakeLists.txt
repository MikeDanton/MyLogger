cmake_minimum_required(VERSION 3.16)
project(myLoggerBenchmarks LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 🔹 Google Benchmark Setup
include(FetchContent)
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
)
FetchContent_MakeAvailable(googlebenchmark)

# 🔹 Benchmark Executables
set(BENCHMARK_SOURCES
        benchmark_config_file_loading.cpp
        benchmark_console_logging_end_to_end.cpp
        benchmark_multi_threaded_logging.cpp
        benchmark_queue_throughput.cpp
)

foreach(benchmark_file ${BENCHMARK_SOURCES})
    get_filename_component(benchmark_name ${benchmark_file} NAME_WE)
    add_executable(${benchmark_name} ${benchmark_file})
    target_link_libraries(${benchmark_name} PRIVATE myLoggerLib benchmark::benchmark benchmark::benchmark_main)
endforeach()
