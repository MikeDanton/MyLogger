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
#include <sstream>
#include <cstring>
#include <filesystem>
#include "logger_config.hpp"

#define MAX_LOG_ENTRIES 1024
#define FLUSH_THRESHOLD 100

// Log message structure
struct LogMessage {
    char level[16];
    char context[64];
    char message[256];
    char timestamp[20];
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
    std::atomic<int> queueHead, queueTail;
    LogMessage logQueue[MAX_LOG_ENTRIES];
    std::mutex mutex;
    std::condition_variable logCondition;
    std::thread logThread;
    std::atomic<bool> exitFlag;
    static std::atomic<int> flushCounter;

    template <size_t... I>
    void dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>);
};

// Logger Constructor
template <typename... Backends>
Logger<Backends...>::Logger(std::shared_ptr<LoggerSettings> settings, Backends&... backends)
    : settings(std::move(settings)),
      backends(std::tie(backends...)),
      queueHead(0),
      queueTail(0),
      logQueue{},
      mutex(),
      logCondition(),
      logThread(&Logger::processQueue, this),
      exitFlag(false) {}

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

// Logging function with optimized lookup and batch processing
template <typename... Backends>
void Logger<Backends...>::log(const char* level, const char* context, const char* message) {
    int levelIndex = (level[0] != '\0' && settings->levelIndexMap.find(level) != settings->levelIndexMap.end())
                     ? settings->levelIndexMap.at(level) : -1;
    int contextIndex = (context[0] != '\0' && settings->contextIndexMap.find(context) != settings->contextIndexMap.end())
                        ? settings->contextIndexMap.at(context) : -1;

    if (levelIndex == -1 || !settings->logLevelEnabledArray[levelIndex]) {
        return;
    }

    int msgSeverity = settings->logLevelSeveritiesArray[levelIndex];
    int minContextSeverity = (contextIndex != -1) ? settings->contextSeverityArray[contextIndex] : 0;

    if (msgSeverity < minContextSeverity) {
        return;
    }

    int colorValue = (settings->colorMode == "context" && contextIndex >= 0)
                     ? settings->contextColorArray[contextIndex]
                     : settings->logColorArray[levelIndex];

    if (colorValue < 30 || colorValue > 37) {
        colorValue = 37;
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

    if (settings->enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::strftime(logMsg.timestamp, sizeof(logMsg.timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    }

    queueHead.store(next, std::memory_order_release);
    logCondition.notify_one();
}

// Process the log queue efficiently
template <typename... Backends>
void Logger<Backends...>::processQueue() {
    while (!exitFlag.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] { return queueHead.load() != queueTail.load() || exitFlag.load(); });

        if (exitFlag.load()) break;

        std::vector<LogMessage> logs;
        while (queueHead.load() != queueTail.load() && logs.size() < 10) {
            logs.push_back(logQueue[queueTail.load()]);
            queueTail.store((queueTail.load() + 1) % MAX_LOG_ENTRIES, std::memory_order_release);
        }
        lock.unlock();

        for (const auto& log : logs) {
            dispatchLog(log, std::index_sequence_for<Backends...>{});
        }
    }
}

// Dispatch logs to all registered backends
template <typename... Backends>
template <size_t... I>
void Logger<Backends...>::dispatchLog(const LogMessage& logMsg, std::index_sequence<I...>) {
    (std::get<I>(backends).write(&logMsg, *settings), ...);
}

// Flush logs periodically
template <typename... Backends>
void Logger<Backends...>::flush() {
    if (++flushCounter % FLUSH_THRESHOLD == 0) {
        std::cout.flush();
    }
}

template <typename... Backends>
std::atomic<int> Logger<Backends...>::flushCounter = 0;

#endif // LOGGER_HPP
