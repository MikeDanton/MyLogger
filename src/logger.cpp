#include "logger.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <format>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now_time);
#else
    localtime_r(&now_time, &localTime);
#endif
    std::ostringstream oss;
    oss << "[" << (localTime.tm_year + 1900) << "-"
        << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1) << "-"
        << std::setw(2) << std::setfill('0') << localTime.tm_mday << " "
        << std::setw(2) << std::setfill('0') << localTime.tm_hour << ":"
        << std::setw(2) << std::setfill('0') << localTime.tm_min << ":"
        << std::setw(2) << std::setfill('0') << localTime.tm_sec << "]";
    return oss.str();
}

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

    for (const auto& existingBackend : backends) {
        if (typeid(*existingBackend) == typeid(*backend)) {
            return;  // Prevent duplicate backend
        }
    }
    backends.push_back(std::move(backend));
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < minLogLevel) return;

    std::string formattedMessage = std::format("{} [{}] {}\n",
                                               getCurrentTimestamp(),
                                               to_string(level),
                                               message);

    {
        std::lock_guard<std::mutex> lock(mutex);
        logQueue.emplace(level, formattedMessage);
    }
    cv.notify_one();
}

void Logger::setLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex);
    minLogLevel = level;
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return !logQueue.empty() || exitFlag; });

        std::vector<std::pair<LogLevel, std::string>> logs;
        while (!logQueue.empty()) {
            logs.push_back(std::move(logQueue.front()));
            logQueue.pop();
        }
        lock.unlock();

        for (const auto& [level, message] : logs) {
            for (const auto& backend : backends) {
                if (auto consoleBackend = dynamic_cast<ConsoleBackend*>(backend.get())) {
                    consoleBackend->write(message, level);
                } else {
                    backend->write(message);
                }
            }
        }

        if (exitFlag) break;
    }
}

void Logger::flush() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return logQueue.empty() && !exitFlag; });
}

const std::vector<std::unique_ptr<LogBackend>>& Logger::getBackends() const {
    return backends;
}