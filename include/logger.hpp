#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logger_config.hpp"
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <unordered_map>
#include <array>
#include <condition_variable>
#include <tuple>
#include <iostream>
#include <cstring>
#include <filesystem>

#define MAX_LOG_ENTRIES 1024
#define FLUSH_THRESHOLD 100

// Log message structure
struct LogMessage {
    char level[16]{};
    char context[64]{};
    char message[256]{};
    char timestamp[20]{};
};

// Templated Logger class
template <typename... Backends>
class Logger {
public:
    explicit Logger(std::shared_ptr<LoggerSettings> settings, Backends&... backends);

    void init(const std::string& configFile = "config/logger.conf");
    void log(const char* level, const char* context, const char* message);
    void flush();
    void updateSettings(const std::string& configFile);
    void processQueue();

private:
    std::shared_ptr<LoggerSettings> settings;
    std::tuple<Backends&...> backends;
    std::atomic<int> queueHead{0}, queueTail{0};
    LogMessage logQueue[MAX_LOG_ENTRIES]{};
    std::mutex mutex;
    std::condition_variable logCondition;
    std::thread logThread;
    std::atomic<bool> exitFlag{false};
    static std::atomic_flag flushFlag;

    template <size_t... I>
    void dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>);
};

// Logger Constructor
template <typename... Backends>
Logger<Backends...>::Logger(std::shared_ptr<LoggerSettings> settings, Backends&... backends)
    : settings(std::move(settings)),
      backends(std::tie(backends...)),
      logThread(&Logger::processQueue, this) {}

template <typename... Backends>
void Logger<Backends...>::init(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, *settings);

    std::apply([&](auto&... backend) {
        ((backend.setup(*settings)), ...);
    }, backends);
}

template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadConfig(configFile, *settings);
}

// Optimized log function
template <typename... Backends>
void Logger<Backends...>::log(const char* level, const char* context, const char* message) {
    int levelIndex = (level[0] != '\0' && settings->levelIndexMap.contains(level))
                     ? settings->levelIndexMap.at(level) : -1;
    int contextIndex = (context[0] != '\0' && settings->contextIndexMap.contains(context))
                       ? settings->contextIndexMap.at(context) : -1;

    if (levelIndex == -1 || !settings->logLevelEnabledArray[levelIndex]) return;

    int msgSeverity = settings->logLevelSeveritiesArray[levelIndex];
    int minContextSeverity = (contextIndex != -1) ? settings->contextSeverityArray[contextIndex] : 0;
    if (msgSeverity < minContextSeverity) return;

    int colorValue = (settings->colorMode == "context" && contextIndex >= 0)
                     ? settings->contextColorArray[contextIndex]
                     : settings->logColorArray[levelIndex];

    if (colorValue < 30 || colorValue > 37) colorValue = 37;

    int currentHead = queueHead.load();
    int next = (currentHead + 1) % MAX_LOG_ENTRIES;
    if (next == queueTail.load()) {
        std::cerr << "[Logger] Log queue is full! Dropping message.\n";
        return;
    }

    LogMessage& logMsg = logQueue[currentHead];
    std::copy_n(level, std::min(sizeof(logMsg.level) - 1, std::strlen(level)), logMsg.level);
    logMsg.level[sizeof(logMsg.level) - 1] = '\0';

    std::copy_n(context, std::min(sizeof(logMsg.context) - 1, std::strlen(context)), logMsg.context);
    logMsg.context[sizeof(logMsg.context) - 1] = '\0';

    std::copy_n(message, std::min(sizeof(logMsg.message) - 1, std::strlen(message)), logMsg.message);
    logMsg.message[sizeof(logMsg.message) - 1] = '\0';

    if (settings->enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::strftime(logMsg.timestamp, sizeof(logMsg.timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    }

    queueHead.store(next, std::memory_order_release);
    logCondition.notify_one();
}

// Optimized processQueue()
template <typename... Backends>
void Logger<Backends...>::processQueue() {
    while (!exitFlag.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] { return queueHead.load() != queueTail.load() || exitFlag.load(); });

        if (exitFlag.load()) break;

        std::array<LogMessage, 10> logs;
        size_t count = 0;

        int tail = queueTail.load();
        while (queueHead.load() != tail && count < logs.size()) {
            logs[count++] = logQueue[tail];
            tail = (tail + 1) % MAX_LOG_ENTRIES;
        }
        queueTail.store(tail, std::memory_order_release);
        lock.unlock();

        for (size_t i = 0; i < count; ++i) {
            dispatchLog(logs[i], std::index_sequence_for<Backends...>{});
        }
    }
}

// Dispatch logs to all registered backends
template <typename... Backends>
template <size_t... I>
void Logger<Backends...>::dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>) {
    (std::get<I>(backends).write(&logMsg, *settings), ...);
}

// Optimized flush() using atomic_flag
template <typename... Backends>
void Logger<Backends...>::flush() {
    if (!flushFlag.test_and_set(std::memory_order_acquire)) {
        std::cout.flush();
        flushFlag.clear(std::memory_order_release);
    }
}

// Initialize static atomic_flag
template <typename... Backends>
std::atomic_flag Logger<Backends...>::flushFlag = ATOMIC_FLAG_INIT;

#endif // LOGGER_HPP
