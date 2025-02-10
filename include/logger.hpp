#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logLevel.hpp"
#include "logMessage.hpp"
#include "logBackend.hpp"
#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>

class Logger {
public:
    Logger();
    ~Logger();

    void addBackend(std::unique_ptr<LogBackend> backend);
    void log(LogLevel level, const std::string& context, const std::string& message);
    void log(LogLevel level, const std::string& message);  // Default to GENERAL

    void setLogLevel(LogLevel level);
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
    LogLevel minLogLevel;
};

#endif // LOGGER_HPP
