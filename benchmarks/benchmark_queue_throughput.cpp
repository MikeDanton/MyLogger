#include <benchmark/benchmark.h>
#include "myLogger/logger.hpp"
#include "myLogger/backends/console_backend.hpp"
#include <memory>

// ✅ Benchmark Log Queue Throughput
static void BM_LogQueueThroughput(benchmark::State& state) {
    auto logger = std::make_unique<Logger<ConsoleBackend>>();

    for (auto _ : state) {
        logger->log("INFO", "TEST", "Logging Performance Test");
    }
}

BENCHMARK(BM_LogQueueThroughput);
BENCHMARK_MAIN();
