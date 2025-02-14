#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "console_backend.hpp"
#include <memory>

// ✅ Benchmark Log Queue Throughput
static void BM_LogQueueThroughput(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend consoleBackend;
    LoggerBackends backends(consoleBackend);
    Logger<decltype(backends)> logger(settings, backends);

    for (auto _ : state) {
        logger.log("INFO", "TEST", "Logging Performance Test");
    }
}

BENCHMARK(BM_LogQueueThroughput);
