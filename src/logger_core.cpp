#include "logger_core.hpp"

#include "logger.hpp"
#include "console_backend.hpp"
#include "logger_config.hpp"
#include "file_backend.hpp"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>

// ✅ LoggerCore Constructor
template <typename Backends>
LoggerCore<Backends>::LoggerCore() {
    logThread = std::thread(&LoggerCore::processQueue, this);
}

// ✅ LoggerCore Destructor
template <typename Backends>
LoggerCore<Backends>::~LoggerCore() {
    exitFlag.store(true);
    logCondition.notify_all();
    if (logThread.joinable()) {
        logThread.join();
    }
}

// ✅ NEW: Set Backends & Settings
template <typename Backends>
void LoggerCore<Backends>::setBackends(Backends& backends, LoggerSettings& settings) {
    m_backends = &backends;
    m_settings = &settings;
}

// ✅ Log Queue Processing (Now Calls Backends)
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

            // 🔹 NEW: Dispatch to backends instead of std::cout
            if (m_backends && m_settings) {
                m_backends->dispatchLog(logMsg, *m_settings);
            }
        }
    }
}

// ✅ Enqueue Log Messages
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

// ✅ Get Current Timestamp
std::string getCurrentTimestamp(const std::string& format) {
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

// 🔹 Explicit instantiation for LoggerCore
template class LoggerCore<LoggerBackends<ConsoleBackend, FileBackend>>;
