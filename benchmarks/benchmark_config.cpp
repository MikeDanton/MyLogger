#include <benchmark/benchmark.h>
#include "loggerConfig.hpp"

// ✅ Benchmark Config File Loading
static void BM_LoadConfig(benchmark::State& state) {
    for (auto _ : state) {
        LoggerConfig::loadConfig("logger.conf");
    }
}

BENCHMARK(BM_LoadConfig);
BENCHMARK_MAIN();
