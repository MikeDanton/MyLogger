#include "logger.hpp"
#include <logger_config.hpp>
#include "console_backend.h"
#include "file_backend.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <condition_variable>
#include <cstring>
#include <filesystem>

// Inside Logger class:
std::condition_variable logCondition;


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : backendCount(0), queueHead(0), queueTail(0), exitFlag(false) {
    std::cerr << "Starting logger thread\n";
    logThread = std::thread(&Logger::processQueue, this);
}

Logger::~Logger() {
    flush();
    exitFlag.store(true);
    logCondition.notify_all();
    if (logThread.joinable()) {
        logThread.join();
    }
}

void Logger::init(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, settings);

    // ✅ Add backends based on config
    if (settings.enableConsole) {
        addBackend(&ConsoleBackend);
    }
    if (settings.enableFile) {
        addBackend(&FileBackend);
    }
}

void Logger::addBackend(LogBackend* backend) {
    std::lock_guard<std::mutex> lock(mutex);
    if (backendCount < MAX_BACKENDS) {
        backends[backendCount++] = backend;
    }
}

void Logger::log(const char* level, const char* context, const char* message) {
    // 1️⃣ Convert log level to index
    int levelIndex = getLevelIndex(level);
    if (levelIndex == -1 || !settings.logLevelEnabledArray[levelIndex]) {
        return; // Skip if level is unknown or disabled
    }

    // 2️⃣ Get severity for this log level
    int msgSeverity = settings.logLevelSeveritiesArray[levelIndex];

    // 3️⃣ Find context severity threshold (DEFAULT: no restriction)
    int contextIndex = getContextIndex(context);
    int contextMinSeverity = (contextIndex == -1) ? 0 : settings.contextSeverityArray[contextIndex];

    // 4️⃣ Skip if message severity is below required threshold
    if (msgSeverity < contextMinSeverity) {
        return;
    }

    // 5️⃣ Prepare to enqueue log message
    int currentHead = queueHead.load();
    int next = (currentHead + 1) % MAX_LOG_ENTRIES;

    if (next == queueTail.load()) {
        std::cerr << "[Logger] Log queue is full! Dropping message.\n";
        return;
    }

    // 6️⃣ Write log message to queue
    LogMessage& logMsg = logQueue[currentHead];
    strncpy(logMsg.level, level, sizeof(logMsg.level));
    strncpy(logMsg.context, context, sizeof(logMsg.context));
    strncpy(logMsg.message, message, sizeof(logMsg.message));

    // 7️⃣ Apply timestamp if enabled
    if (settings.enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::strftime(logMsg.timestamp, sizeof(logMsg.timestamp), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    }

    // 8️⃣ Move queue forward and notify processing thread
    queueHead.store(next, std::memory_order_release);
    logCondition.notify_one();
}

void Logger::processQueue() {
    std::cerr << "processQueue() started\n";

    while (!exitFlag.load()) {
        std::unique_lock<std::mutex> lock(mutex);
        logCondition.wait(lock, [this] { return queueHead.load() != queueTail.load() || exitFlag.load(); });

        if (exitFlag.load()) break; // Exit if stopping

        LogMessage logs[MAX_LOG_ENTRIES];
        int count = 0;

        while (queueHead.load() != queueTail.load() && count < MAX_LOG_ENTRIES) {
            logs[count++] = logQueue[queueTail.load()];
            queueTail.store((queueTail.load() + 1) % MAX_LOG_ENTRIES, std::memory_order_release);
            logCondition.notify_all();  // ✅ Wake up flush()

        }

        lock.unlock(); // Unlock before processing logs

        for (int i = 0; i < count; i++) {
            for (int j = 0; j < backendCount; j++) {
                backends[j]->write(&logs[i]);
            }
        }
    }
}

void Logger::flush() {
    std::unique_lock<std::mutex> lock(mutex);

    logCondition.wait(lock, [this] {
        return queueHead.load() == queueTail.load();
    });

    for (int i = 0; i < backendCount; i++) {
        backends[i]->flush();
    }

    exitFlag.store(true);
    logCondition.notify_all();
}

int Logger::getSeverity(const std::string& level) const {
    int index = getLevelIndex(level);
    return (index != -1) ? settings.logLevelSeveritiesArray[index] : 0;
}

int Logger::getLevelIndex(const std::string& level) const {
    for (size_t i = 0; i < settings.logLevelNames.size(); i++) {
        if (settings.logLevelNames[i] == level) return i;
    }
    return -1;
}

int Logger::getContextIndex(const std::string& context) const {
    for (size_t i = 0; i < settings.contextNames.size(); i++) {
        if (settings.contextNames[i] == context) return i;
    }
    return -1;
}

void Logger::updateSettings(const std::string& configFile) {
    static std::filesystem::file_time_type lastWriteTime;

    std::filesystem::path configPath = configFile;

    auto currentWriteTime = std::filesystem::last_write_time(configPath);
    if (currentWriteTime != lastWriteTime) {
        lastWriteTime = currentWriteTime;
        std::cerr << "[Logger] Detected config change, reloading...\n";
        LoggerConfig::loadConfig(configFile, settings);
    }
}