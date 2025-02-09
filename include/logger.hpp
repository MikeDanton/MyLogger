#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logBackend.hpp"
#include "logLevel.hpp"
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>

class Logger {
public:
    Logger();
    ~Logger();

    void addBackend(std::unique_ptr<LogBackend> backend);
    void setLogLevel(LogLevel level);
    void log(LogLevel level, const std::string& message);
    void flush();

private:
    std::queue<std::pair<LogLevel, std::string>> logQueue;
    std::vector<std::unique_ptr<LogBackend>> backends;
    std::mutex mutex;
    std::condition_variable cv;
    std::thread logThread;
    bool exitFlag;
    LogLevel minLogLevel;

    void processQueue();
};

#endif // LOGGER_HPP
