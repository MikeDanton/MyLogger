#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "file_backend.hpp"
#include <memory>

// ✅ Benchmark File Logging Performance
static void BM_FileLogging(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    FileBackend fileBackend;

    // ✅ Use correct Logger instantiation
    Logger<FileBackend> logger(settings, fileBackend);

    for (auto _ : state) {
        logger.log("INFO", "BENCHMARK", "Testing file logging speed...");
    }

    logger.flush();
}

BENCHMARK(BM_FileLogging);
