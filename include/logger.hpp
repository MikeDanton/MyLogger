#ifndef LOGGER_HPP
#define LOGGER_HPP

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
#include "logger_config.hpp"

#define MAX_LOG_ENTRIES 1024

// ✅ Log message structure
struct LogMessage {
    char level[16];
    char context[64];
    char message[256];
    char timestamp[20];
};

// ✅ Templated Logger class
template <typename... Backends>
class Logger {
public:
    explicit Logger(LoggerSettings settings, Backends&... backends);

    void init(const std::string& configFile = "config/logger.conf");
    void log(const char* level, const char* context, const char* message);
    void flush();
    void updateSettings(const std::string& configFile);
    void processQueue();

private:
    LoggerSettings settings;
    std::tuple<Backends&...> backends;
    std::atomic<int> queueHead, queueTail;
    LogMessage logQueue[MAX_LOG_ENTRIES];
    std::mutex mutex;
    std::condition_variable logCondition;
    std::thread logThread;
    std::atomic<bool> exitFlag;

    template <size_t... I>
    void dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>);
};

// ✅ Logger Constructor
template <typename... Backends>
Logger<Backends...>::Logger(LoggerSettings settings, Backends&... backends)
    : settings(settings), backends(std::tie(backends...)), queueHead(0), queueTail(0), exitFlag(false) {
    logThread = std::thread(&Logger::processQueue, this);
}

// ✅ Logging function with level and context severity filtering
template <typename... Backends>
void Logger<Backends...>::log(const char* level, const char* context, const char* message) {
    int levelIndex = settings.levelIndexMap.count(level) ? settings.levelIndexMap[level] : -1;
    if (levelIndex == -1 || !settings.logLevelEnabledArray[levelIndex]) {
        return; // ✅ Skip logging if level is unknown or disabled
    }

    int msgSeverity = settings.logLevelSeveritiesArray[levelIndex];

    int contextIndex = settings.contextIndexMap.count(context) ? settings.contextIndexMap[context] : -1;
    int minContextSeverity = (contextIndex != -1) ? settings.contextSeverityArray[contextIndex] : 0;

    if (msgSeverity < minContextSeverity) {
        return; // ✅ Skip if log severity is too low for the context
    }

    int currentHead = queueHead.load();
    int next = (currentHead + 1) % MAX_LOG_ENTRIES;

    if (next == queueTail.load()) {
        std::cerr << "[Logger] Log queue is full! Dropping message.\n";
        return;
    }

    LogMessage& logMsg = logQueue[currentHead];
    strncpy(logMsg.level, level, sizeof(logMsg.level));
    strncpy(logMsg.context, context, sizeof(logMsg.context));
    strncpy(logMsg.message, message, sizeof(logMsg.message));

    if (settings.enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::strftime(logMsg.timestamp, sizeof(logMsg.timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    }

    queueHead.store(next, std::memory_order_release);
    logCondition.notify_one();
}

// ✅ Initialization function to load configurations
template <typename... Backends>
void Logger<Backends...>::init(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, settings);

    std::apply([&](auto&... backend) {
        ((backend.setup(settings)), ...);  // ✅ Call setup() on each backend
    }, backends);
}

// ✅ Process the log queue and dispatch logs to backends
template <typename... Backends>
void Logger<Backends...>::processQueue() {
    while (!exitFlag.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] { return queueHead.load() != queueTail.load() || exitFlag.load(); });

        if (exitFlag.load()) break;

        LogMessage logs[MAX_LOG_ENTRIES];
        int count = 0;

        while (queueHead.load() != queueTail.load() && count < MAX_LOG_ENTRIES) {
            logs[count++] = logQueue[queueTail.load()];
            queueTail.store((queueTail.load() + 1) % MAX_LOG_ENTRIES, std::memory_order_release);
        }

        lock.unlock();

        for (int i = 0; i < count; i++) {
            dispatchLog(logs[i], std::index_sequence_for<Backends...>{});
        }
    }
}

// ✅ Update logger settings dynamically on file change
template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    static std::filesystem::file_time_type lastWriteTime;
    std::filesystem::path configPath = configFile;

    auto currentWriteTime = std::filesystem::last_write_time(configPath);
    if (currentWriteTime != lastWriteTime) {
        lastWriteTime = currentWriteTime;
        LoggerConfig::loadConfig(configFile, settings);  // ✅ Reload all settings
        std::cerr << "[Logger] Config updated. New settings applied.\n";
    }
}

// ✅ Dispatch logs to all registered backends
template <typename... Backends>
template <size_t... I>
void Logger<Backends...>::dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>) {
    (std::get<I>(backends).write(&logMsg, settings), ...);
}

// ✅ Flush logs in all backends
template <typename... Backends>
void Logger<Backends...>::flush() {
    std::unique_lock<std::mutex> lock(mutex);

    logCondition.wait(lock, [this] {
        return queueHead.load() == queueTail.load();
    });

    std::apply([](auto&... backend) {
        ((backend.flush()), ...);
    }, backends);

    exitFlag.store(true);
    logCondition.notify_all();
}

#endif // LOGGER_HPP
