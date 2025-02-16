cmake_minimum_required(VERSION 3.16)

find_package(benchmark REQUIRED)

set(BENCHMARK_SOURCES
        benchmark_config_file_loading.cpp
        benchmark_console_logging_end_to_end.cpp
        benchmark_multi_threaded_logging.cpp
        benchmark_queue_throughput.cpp
)

foreach(bench_file ${BENCHMARK_SOURCES})
    get_filename_component(bench_name ${bench_file} NAME_WE)

    # 🔹 Ensure the target doesn't already exist
    if (NOT TARGET ${bench_name}_benchmark)
        add_executable(${bench_name}_benchmark ${bench_file})
        target_link_libraries(${bench_name}_benchmark PRIVATE myLoggerLib benchmark::benchmark benchmark::benchmark_main)
    endif()
endforeach()
