#include <benchmark/benchmark.h>
#include "logger.hpp"
#include "console_backend.hpp"
#include <thread>
#include <vector>
#include <memory>

// ✅ Benchmark Multi-Threaded Logging
static void BM_MultiThreadedLogging(benchmark::State& state) {
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend consoleBackend;
    LoggerBackends backends(consoleBackend);
    Logger<decltype(backends)> logger(settings, backends);

    for (auto _ : state) {
        std::vector<std::thread> threads;
        for (int i = 0; i < 4; ++i) {  // 4 threads
            threads.emplace_back([&logger]() {
                for (int j = 0; j < 100; ++j) {
                    logger.log("INFO", "THREAD", "Threaded Logging Performance Test");
                }
            });
        }

        for (auto& thread : threads) {
            thread.join();
        }
    }

}

BENCHMARK(BM_MultiThreadedLogging);
