#ifndef LOGGER_CORE_HPP
#define LOGGER_CORE_HPP

#include "myLogger/logger_config.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

#define MAX_BATCH_SIZE 256  // Batching limit for efficiency

struct LogMessage {
    std::string level;
    std::string context;
    std::string message;
    std::string timestamp;

    LogMessage(std::string_view lvl, std::string_view ctx, std::string_view msg)
        : level(lvl), context(ctx), message(msg) {}

    void setTimestamp(std::string_view ts) {
        timestamp = ts;
    }
};

template <typename Backends>
class LoggerCore {
public:
    LoggerCore();
    ~LoggerCore();

    void enqueueLog(LogMessage&& logMsg, const LoggerSettings& settings);
    void shutdown();
    void setBackends(Backends& backends, LoggerSettings& settings);

private:
    std::thread logThread;
    std::atomic<bool> exitFlag{false};
    std::condition_variable logCondition;
    std::mutex mutex;
    std::atomic<int> queueSize{0};

    std::deque<LogMessage> logQueue;
    Backends* m_backends{nullptr};
    LoggerSettings* m_settings{nullptr};

    void processQueue();
};

// ✅ Function to get current timestamp
std::string getCurrentTimestamp(const std::string& format);

template <typename Backends>
LoggerCore<Backends>::LoggerCore() {
    logThread = std::thread(&LoggerCore::processQueue, this);
}

template <typename Backends>
LoggerCore<Backends>::~LoggerCore() {
    shutdown();
}

template <typename Backends>
void LoggerCore<Backends>::setBackends(Backends& backends, LoggerSettings& settings) {
    m_backends = &backends;
    m_settings = &settings;
}

template <typename Backends>
void LoggerCore<Backends>::enqueueLog(LogMessage&& logMsg, const LoggerSettings& settings) {
    if (settings.config.format.enableTimestamps) {
        logMsg.setTimestamp(getCurrentTimestamp(settings.config.format.timestampFormat));
    }

    {
        std::unique_lock<std::mutex> lock(mutex);
        logQueue.emplace_back(std::move(logMsg));
        queueSize.fetch_add(1, std::memory_order_release);
    }

    logCondition.notify_one();
}

template <typename Backends>
void LoggerCore<Backends>::processQueue() {
    std::cout << "[LoggerCore] Logging thread started.\n";

    while (!exitFlag.load(std::memory_order_acquire)) {
        std::vector<LogMessage> batch;
        batch.reserve(MAX_BATCH_SIZE);

        {
            std::unique_lock<std::mutex> lock(mutex);
            logCondition.wait_for(lock, std::chrono::milliseconds(10), [this] {
                return !logQueue.empty() || exitFlag.load();
            });

            while (!logQueue.empty() && batch.size() < MAX_BATCH_SIZE) {
                batch.emplace_back(std::move(logQueue.front()));
                logQueue.pop_front();
            }
        }

        for (auto& logMsg : batch) {
            if (m_backends && m_settings) {
                m_backends->dispatchLog(logMsg, *m_settings);
            }
        }
    }
}

template <typename Backends>
void LoggerCore<Backends>::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        exitFlag.store(true, std::memory_order_release);
    }

    logCondition.notify_all();

    while (!logQueue.empty()) {
        std::vector<LogMessage> batch;
        batch.reserve(MAX_BATCH_SIZE);

        {
            std::unique_lock<std::mutex> lock(mutex);
            while (!logQueue.empty() && batch.size() < MAX_BATCH_SIZE) {
                batch.emplace_back(std::move(logQueue.front()));
                logQueue.pop_front();
            }
        }

        for (auto& logMsg : batch) {
            if (m_backends && m_settings) {
                m_backends->dispatchLog(logMsg, *m_settings);
            }
        }
    }

    if (logThread.joinable()) {
        logThread.join();
    }
}

inline std::string getCurrentTimestamp(const std::string& format) {
    std::ostringstream timestamp;
    auto now = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo{};
    localtime_r(&timeT, &timeinfo);

    if (format == "ISO") {
        timestamp << std::put_time(&timeinfo, "%Y-%m-%dT%H:%M:%S");
    } else if (format == "short") {
        timestamp << std::put_time(&timeinfo, "%H:%M:%S");
    } else if (format == "epoch") {
        timestamp << timeT;
    }

    return timestamp.str();
}

#endif // LOGGER_CORE_HPP
