#include <benchmark/benchmark.h>
#include "loggerLib.hpp"

// ✅ Benchmark Log Writing Performance
static void BM_LoggingPerformance(benchmark::State& state) {
    Logger logger;
    logger.addBackend(std::make_unique<ConsoleBackend>());
    logger.setLogLevel(LogLevel::INFO);

    for (auto _ : state) {
        logger.log(LogLevel::INFO, "Benchmarking log performance...");
    }
}

BENCHMARK(BM_LoggingPerformance);
BENCHMARK_MAIN();
