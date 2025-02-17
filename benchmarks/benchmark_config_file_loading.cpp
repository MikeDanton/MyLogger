#include <benchmark/benchmark.h>
#include "logger_config.hpp"

// ✅ Benchmark Config File Loading
static void BM_LoadConfig(benchmark::State& state) {
    for (auto _ : state) {
        LoggerSettings settings;
        LoggerConfig::loadOrGenerateConfig("config/logger.conf", settings);
    }
}

BENCHMARK(BM_LoadConfig);
