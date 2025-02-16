#ifndef LOGGER_CORE_HPP
#define LOGGER_CORE_HPP

#include "logger_config.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <deque>
#include <iomanip>
#include <vector>
#include <iostream>

#define MAX_BATCH_SIZE 256  // ✅ Batching limit for efficiency

#include <cstring>

struct LogMessage {
    char level[16]{};    // ✅ Fixed-size buffer (was `std::string`)
    char context[32]{};  // ✅ Fixed-size buffer
    char message[256]{}; // ✅ Fixed-size buffer
    char timestamp[32]{};

    void set(const std::string& lvl, const std::string& ctx, const std::string& msg) {
        std::strncpy(level, lvl.c_str(), sizeof(level) - 1);
        std::strncpy(context, ctx.c_str(), sizeof(context) - 1);
        std::strncpy(message, msg.c_str(), sizeof(message) - 1);

        // Ensure null termination
        level[sizeof(level) - 1] = '\0';
        context[sizeof(context) - 1] = '\0';
        message[sizeof(message) - 1] = '\0';
    }

    void setTimestamp(const std::string& ts) {
        std::strncpy(timestamp, ts.c_str(), sizeof(timestamp) - 1);
        timestamp[sizeof(timestamp) - 1] = '\0';
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
    std::atomic<int> queueSize{0};  // ✅ Atomic size counter

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
        logQueue.push_back(std::move(logMsg));
        queueSize.fetch_add(1, std::memory_order_release);
    }

    logCondition.notify_one();
}

template <typename Backends>
void LoggerCore<Backends>::processQueue() {
    std::cout << "[LoggerCore] Logging thread started.\n";

    while (!exitFlag.load()) {
        std::vector<LogMessage> batch;
        batch.reserve(MAX_BATCH_SIZE);

        for (int spin = 0; spin < 100; ++spin) {  // ✅ Spin-wait to avoid lock contention
            if (!logQueue.empty()) break;
            std::this_thread::yield();
        }

        {
            std::unique_lock<std::mutex> lock(mutex);
            logCondition.wait_for(lock, std::chrono::milliseconds(10), [this] {
                return !logQueue.empty() || exitFlag.load();
            });

            while (!logQueue.empty() && batch.size() < MAX_BATCH_SIZE) {
                batch.push_back(std::move(logQueue.front()));
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

    if (logThread.joinable()) {
        logThread.join();
    }
}

inline std::string getCurrentTimestamp(const std::string& format) {
    std::ostringstream timestamp;
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    if (format == "ISO") {
        timestamp << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S");
    } else if (format == "short") {
        timestamp << std::put_time(timeinfo, "%H:%M:%S");
    } else if (format == "epoch") {
        timestamp << now;
    }

    return timestamp.str();
}

#endif // LOGGER_CORE_HPP
