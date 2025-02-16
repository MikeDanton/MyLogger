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

// ✅ Forward declaration (for LoggerBackends)
template <typename Backends>
class LoggerCore {
public:
    LoggerCore();
    ~LoggerCore();

    void enqueueLog(LogMessage&& logMsg, const LoggerSettings& settings);
    void processQueue();
    void flushQueue();

    void setBackends(Backends& backends, LoggerSettings& settings);

private:

    std::thread logThread;
    std::atomic<bool> exitFlag{false};
    std::condition_variable logCondition;
    std::mutex mutex;

    std::atomic<int> queueHead{0};
    std::atomic<int> queueTail{0};
    std::array<LogMessage, MAX_LOG_ENTRIES> logQueue;

    // 🔹 Pointers to backends & settings
    Backends* m_backends{nullptr};
    LoggerSettings* m_settings{nullptr};
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
    exitFlag.store(true);
    logCondition.notify_all();
    if (logThread.joinable()) {
        logThread.join();
    }
}

template <typename Backends>
void LoggerCore<Backends>::setBackends(Backends& backends, LoggerSettings& settings) {
    m_backends = &backends;
    m_settings = &settings;
}

template <typename Backends>
void LoggerCore<Backends>::processQueue() {
    while (!exitFlag.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] {
            return queueHead.load() != queueTail.load() || exitFlag.load();
        });

        if (exitFlag.load()) break;

        while (queueHead.load() != queueTail.load()) {
            LogMessage logMsg = logQueue[queueTail.load()];
            queueTail.store((queueTail.load() + 1) % MAX_LOG_ENTRIES, std::memory_order_release);
            
            if (m_backends && m_settings) {
                m_backends->dispatchLog(logMsg, *m_settings);
            }
        }
    }
}

template <typename Backends>
void LoggerCore<Backends>::enqueueLog(LogMessage&& logMsg, const LoggerSettings& settings) {
    if (settings.config.format.enableTimestamps) {
        logMsg.timestamp = getCurrentTimestamp(settings.config.format.timestampFormat);
    }

    int next = (queueHead.load() + 1) % MAX_LOG_ENTRIES;
    if (next == queueTail.load()) {
        std::cerr << "[Logger] Log queue is full! Dropping message.\n";
        return;
    }

    logQueue[queueHead.load()] = std::move(logMsg);
    queueHead.store(next, std::memory_order_release);
    logCondition.notify_one();
}

// ✅ Get Current Timestamp Function
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

template <typename Backends>
void LoggerCore<Backends>::flushQueue() {
    std::cout << "[LoggerCore] Flushing log queue...\n";

    std::unique_lock<std::mutex> lock(mutex);
    while (queueHead.load() != queueTail.load()) {
        LogMessage logMsg = logQueue[queueTail.load()];
        queueTail.store((queueTail.load() + 1) % MAX_LOG_ENTRIES, std::memory_order_release);

        if (m_backends && m_settings) {
            m_backends->dispatchLog(logMsg, *m_settings);
        }
    }

    std::cout << "[LoggerCore] Log queue flushed.\n";
}


#endif // LOGGER_CORE_HPP
