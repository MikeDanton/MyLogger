#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logMessage.hpp"
#include "logBackend.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include "loggerSettings.hpp"
#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>
#include <string>

template <bool EnableLogging>
class Logger {
public:
    Logger();
    ~Logger();

    void addBackend(std::unique_ptr<LogBackend> backend);
    void log(const std::string& level, const std::string& context, const std::string& message);
    void log(const std::string& level, const std::string& message);

    void setLogLevel(const std::string& level);
    void flush();

    const std::vector<std::unique_ptr<LogBackend>>& getBackends() const;

private:
    void processQueue();

    std::vector<std::unique_ptr<LogBackend>> backends;
    std::queue<LogMessage> logQueue;
    std::mutex mutex;
    std::condition_variable cv;
    bool exitFlag;
    std::thread logThread;
    std::string minLogLevel;
};

// 🔹 Constructor
template <bool EnableLogging>
Logger<EnableLogging>::Logger() : exitFlag(false), minLogLevel("INFO") {
    if constexpr (EnableLogging) {
        if (LoggerSettings::getInstance().isConsoleEnabled()) {
            backends.push_back(std::make_unique<ConsoleBackend>());
        }

        if (LoggerSettings::getInstance().isFileEnabled()) {
            backends.push_back(std::make_unique<FileBackend>());
        }

        logThread = std::thread(&Logger::processQueue, this);
    }
}

// 🔹 Destructor
template <bool EnableLogging>
Logger<EnableLogging>::~Logger() {
    if constexpr (EnableLogging) {
        {
            std::lock_guard<std::mutex> lock(mutex);
            exitFlag = true;
        }

        cv.notify_one();
        if (logThread.joinable()) {
            logThread.join();
        }
    }
}

// 🔹 Add Backend
template <bool EnableLogging>
void Logger<EnableLogging>::addBackend(std::unique_ptr<LogBackend> backend) {
    if constexpr (!EnableLogging) return;
    std::lock_guard<std::mutex> lock(mutex);

    for (const auto& existingBackend : backends) {
        if (typeid(*existingBackend) == typeid(*backend)) {
            return;  // Prevent duplicate backend
        }
    }
    backends.push_back(std::move(backend));
}

// 🔹 Logging Function (Removes Logs in Release Mode)
template <bool EnableLogging>
void Logger<EnableLogging>::log(const std::string& level, const std::string& context, const std::string& message) {
    if constexpr (!EnableLogging) return;  // ✅ Removes logging at compile-time

    if (!LoggerSettings::getInstance().shouldLog(level, context)) {
        return;
    }

    LogMessage logMessage{level, context, message};

    {
        std::lock_guard<std::mutex> lock(mutex);
        logQueue.push(logMessage);
    }
    cv.notify_one();
}

template <bool EnableLogging>
void Logger<EnableLogging>::log(const std::string& level, const std::string& message) {
    log(level, "GENERAL", message);
}

// 🔹 Set Log Level
template <bool EnableLogging>
void Logger<EnableLogging>::setLogLevel(const std::string& level) {
    if constexpr (!EnableLogging) return;
    std::lock_guard<std::mutex> lock(mutex);
    minLogLevel = level;
}

// 🔹 Process Queue (Runs in Background Thread)
template <bool EnableLogging>
void Logger<EnableLogging>::processQueue() {
    if constexpr (!EnableLogging) return;

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

// 🔹 Flush
template <bool EnableLogging>
void Logger<EnableLogging>::flush() {
    if constexpr (!EnableLogging) return;

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

// 🔹 Get Backends
template <bool EnableLogging>
const std::vector<std::unique_ptr<LogBackend>>& Logger<EnableLogging>::getBackends() const {
    return backends;
}

#endif // LOGGER_HPP
