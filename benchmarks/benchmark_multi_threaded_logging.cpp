#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "console_backend.hpp"
#include <thread>
#include <vector>
#include <memory>

static void BM_HighThroughputLogging(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend consoleBackend;

    Logger<ConsoleBackend> logger(settings, consoleBackend);

    const int numThreads = std::thread::hardware_concurrency();
    const int logsPerThread = state.range(0); // Logs per thread from benchmark range

    for (auto _ : state) {
        std::vector<std::thread> threads;

        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&]() {
                for (int j = 0; j < logsPerThread; ++j) {
                    logger.log("INFO", "THREAD", "High-Throughput Logging Test");
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }

        state.PauseTiming();  // ✅ Don't measure shutdown
        logger.shutdown();
        state.ResumeTiming();
    }

    state.SetItemsProcessed(state.iterations() * numThreads * logsPerThread);
}

// ✅ Benchmark with different log amounts per thread (10k, 50k, 100k)
BENCHMARK(BM_HighThroughputLogging)->Arg(10000)->Arg(50000)->Arg(100000);

BENCHMARK_MAIN();
