#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "console_backend.hpp"
#include <memory>

// ✅ Benchmark Console Logging Performance
static void BM_ConsoleLogging(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend consoleBackend;
    LoggerBackends backends(consoleBackend);
    Logger<decltype(backends)> logger(settings, backends);

    for (auto _ : state) {
        logger.log("INFO", "BENCHMARK", "Testing console logging speed...");
    }
}

BENCHMARK(BM_ConsoleLogging);
