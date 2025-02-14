#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "file_backend.hpp"
#include <memory>

// ✅ Benchmark File Logging Performance
static void BM_FileLogging(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    FileBackend fileBackend;
    LoggerBackends backends(fileBackend);
    Logger<decltype(backends)> logger(settings, backends);

    logger.init();  // Ensure file is created

    for (auto _ : state) {
        logger.log("INFO", "BENCHMARK", "Testing file logging speed...");
    }

    logger.flush();
}

BENCHMARK(BM_FileLogging);
