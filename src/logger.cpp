#include "logger.h"
#include <logger_config.hpp>
#include "console_backend.h"
#include "file_backend.h"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <condition_variable>
#include <cstring>

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
    LoggerSettings settings = {};
    LoggerConfig::loadConfig(configFile, settings);
    this->settings = settings;

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
    // 1) Check global level switch
    if (!settings.logLevelEnabled[level]) {
        return; // If this level is OFF globally, skip it.
    }

    // 2) Convert log level to numeric severity
    int msgSeverity = getSeverity(level);

    // 3) Find context severity threshold (DEFAULT: no restriction)
    int contextMinSeverity = 0;  // Default: log everything if context is not listed
    auto it = settings.contextLevels.find(context);
    if (it != settings.contextLevels.end()) {
        contextMinSeverity = getSeverity(it->second);
    }

    // 4) Skip if message severity is below the required threshold
    if (msgSeverity < contextMinSeverity) {
        return;
    }

    int currentHead = queueHead.load();
    int next = (currentHead + 1) % MAX_LOG_ENTRIES;

    if (next == queueTail.load()) {
        std::cerr << "Log queue is full! Dropping message.\n";
        return;
    }

    LogMessage& logMsg = logQueue[currentHead];
    strncpy(logMsg.level, level, sizeof(logMsg.level));
    strncpy(logMsg.context, context, sizeof(logMsg.context));
    strncpy(logMsg.message, message, sizeof(logMsg.message));

    if (settings.enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::strftime(logMsg.timestamp, sizeof(logMsg.timestamp),
                      "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    }

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
    auto it = settings.logLevelSeverities.find(level);
    if (it != settings.logLevelSeverities.end()) {
        return it->second;
    }
    // Unknown => treat as severity 0 or skip entirely
    return 0;
}
