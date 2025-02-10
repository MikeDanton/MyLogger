#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logMessage.hpp"
#include "logBackend.hpp"
#include <vector>
#include <memory>
#include <queue>
#include <thread>
#include <condition_variable>
#include <string>

class Logger {
public:
    Logger();
    ~Logger();

    void addBackend(std::unique_ptr<LogBackend> backend);
    void log(const std::string& level, const std::string& context, const std::string& message);
    void log(const std::string& level, const std::string& message);  // Default context

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

#endif // LOGGER_HPP
