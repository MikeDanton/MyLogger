#include "logger.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <format>

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
    logThread.join();
}

void Logger::addBackend(std::unique_ptr<LogBackend> backend) {
    std::lock_guard<std::mutex> lock(mutex);
    backends.push_back(std::move(backend));
}

// ✅ Allow setting the log level dynamically
void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    minLogLevel = level;
}

void Logger::log(LogLevel level, const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (level < minLogLevel) return;  // ✅ Apply filtering before adding to queue

        std::string formattedMessage = std::format("[{}] {}\n", to_string(level), message);
        logQueue.push({level, formattedMessage});
    }
    cv.notify_one();
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return !logQueue.empty() || exitFlag; });

        while (!logQueue.empty()) {
            const auto& [level, message] = logQueue.front();
            for (const auto& backend : backends) {
                backend->write(message);
            }
            logQueue.pop();
        }

        if (exitFlag) break;
    }
}

void Logger::flush() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return logQueue.empty() && !exitFlag; });
}
