#ifndef LOGGER_CORE_HPP
#define LOGGER_CORE_HPP

#include "logger_config.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <array>
#include <iomanip>
#include <iostream>

// Maximum log entries before flushing
#define MAX_LOG_ENTRIES 1024

// ✅ Log message structure
struct LogMessage {
    std::string level;
    std::string context;
    std::string message;
    std::string timestamp;
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

    int queueHead = 0;
    int queueTail = 0;
    std::array<LogMessage, MAX_LOG_ENTRIES> logQueue;

    Backends* m_backends{nullptr};
    LoggerSettings* m_settings{nullptr};

    void processQueue();  // Background thread loop
};

// ✅ Function to get current timestamp
std::string getCurrentTimestamp(const std::string& format);

// ==========================
// ✅ IMPLEMENTATION SECTION
// ==========================

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
        logMsg.timestamp = getCurrentTimestamp(settings.config.format.timestampFormat);
    }

    {
        std::unique_lock<std::mutex> lock(mutex);

        int next = (queueHead + 1) % MAX_LOG_ENTRIES;
        if (next == queueTail) {
            std::cerr << "[Logger] Log queue is full! Dropping message.\n";
            return;
        }

        logQueue[queueHead] = std::move(logMsg);
        queueHead = next;
    }

    logCondition.notify_one();
}

template <typename Backends>
void LoggerCore<Backends>::processQueue() {
    std::cout << "[LoggerCore] Logging thread started.\n";

    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] {
            return queueHead != queueTail || exitFlag.load();
        });

        if (exitFlag.load() && queueHead == queueTail) {
            std::cout << "[LoggerCore] Exiting logging thread.\n";
            break;
        }

        while (queueHead != queueTail) {
            LogMessage logMsg = std::move(logQueue[queueTail]);
            queueTail = (queueTail + 1) % MAX_LOG_ENTRIES;

            lock.unlock(); // 🔥 Unlock before dispatch to avoid blocking other loggers
            if (m_backends && m_settings) {
                m_backends->dispatchLog(logMsg, *m_settings);
            }
            lock.lock();
        }
    }
}

template <typename Backends>
void LoggerCore<Backends>::shutdown() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        exitFlag.store(true);
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
