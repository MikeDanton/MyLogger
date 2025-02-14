#ifndef LOGGER_CORE_HPP
#define LOGGER_CORE_HPP

#include "logger_config.hpp"
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <array>

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

    // 🔹 NEW: Allow setting backends after initialization
    void setBackends(Backends& backends, LoggerSettings& settings);

private:
    void processQueue();

    std::thread logThread;
    std::atomic<bool> exitFlag{false};
    std::condition_variable logCondition;
    std::mutex mutex;

    std::atomic<int> queueHead{0};
    std::atomic<int> queueTail{0};
    std::array<LogMessage, MAX_LOG_ENTRIES> logQueue;

    // 🔹 NEW: Pointers to backends & settings
    Backends* m_backends{nullptr};
    LoggerSettings* m_settings{nullptr};
};

// ✅ Function to get current timestamp
std::string getCurrentTimestamp(const std::string& format);

#endif // LOGGER_CORE_HPP
