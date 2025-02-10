#include "logger.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <chrono>
#include <loggerSettings.hpp>

Logger::Logger() : exitFlag(false), minLogLevel(LogLevel::INFO) {
    backends.push_back(std::make_unique<ConsoleBackend>());
    logThread = std::thread(&Logger::processQueue, this);
}

Logger::~Logger() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        exitFlag = true;
    }

    cv.notify_one();
    if (logThread.joinable()) {
        logThread.join();
    }
}

void Logger::addBackend(std::unique_ptr<LogBackend> backend) {
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& existingBackend : backends) {
        if (typeid(*existingBackend) == typeid(*backend)) {
            return;  // Prevent duplicate backend
        }
    }
    backends.push_back(std::move(backend));
}

void Logger::log(LogLevel level, const std::string& context, const std::string& message) {
    if (level < minLogLevel) return;

    std::string resolvedContext = LoggerSettings::getInstance().getContextLogLevel(context);

    LogMessage logMessage{level, resolvedContext, message};
    {
        std::lock_guard<std::mutex> lock(mutex);
        logQueue.push(logMessage);
    }
    cv.notify_one();
}

void Logger::log(LogLevel level, const std::string& message) {
    log(level, "GENERAL", message);
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    minLogLevel = level;
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return !logQueue.empty() || exitFlag; });

        if (exitFlag && logQueue.empty()) {
            return;
        }

        std::vector<LogMessage> logs;
        while (!logQueue.empty()) {
            logs.push_back(std::move(logQueue.front()));
            logQueue.pop();
        }
        lock.unlock();

        for (const auto& logMessage : logs) {
            for (const auto& backend : backends) {
                backend->write(logMessage);
            }
        }
    }
}

void Logger::flush() {
    std::unique_lock<std::mutex> lock(mutex);

    while (!logQueue.empty()) {
        std::vector<LogMessage> logs;
        while (!logQueue.empty()) {
            logs.push_back(std::move(logQueue.front()));
            logQueue.pop();
        }
        lock.unlock();

        for (const auto& logMessage : logs) {
            for (const auto& backend : backends) {
                backend->write(logMessage);
            }
        }
        lock.lock();
    }

    for (const auto& backend : backends) {
        backend->flush();
    }

    cv.notify_all();
}

const std::vector<std::unique_ptr<LogBackend>>& Logger::getBackends() const {
    return backends;
}
