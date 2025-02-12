#ifndef LOGGER_CONTROLLER_HPP
#define LOGGER_CONTROLLER_HPP

#include "logger.hpp"
#include "file_watcher.hpp"
#include <atomic>
#include <thread>

class LoggerController {
public:
    LoggerController();
    ~LoggerController();

    void start();
    void stop();

private:
    void periodicLogging();
    void threadedLogging();

    Logger& logger;
    std::atomic<bool> exitFlag;
    std::thread logThread;
    std::thread multiThreadedLogger;
    std::thread configWatcher;
};

#endif // LOGGER_CONTROLLER_HPP
