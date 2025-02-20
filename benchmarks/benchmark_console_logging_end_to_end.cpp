#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "console_backend.hpp"
#include <memory>

// ✅ Benchmark Console Logging Performance
static void BM_ConsoleLogging(benchmark::State& state) {
    // ✅ Proper instantiation with the new Logger setup
    auto logger = std::make_unique<Logger<ConsoleBackend>>();

    for (auto _ : state) {
        logger->log("INFO", "BENCHMARK", "Testing console logging speed...");
    }
}

BENCHMARK(BM_ConsoleLogging);
BENCHMARK_MAIN();
