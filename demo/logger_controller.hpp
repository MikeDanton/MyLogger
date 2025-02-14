#ifndef LOGGER_CONTROLLER_HPP
#define LOGGER_CONTROLLER_HPP

#include "logger.hpp"
#include "file_watcher.hpp"
#include <atomic>
#include <thread>
#include <iostream>

template <typename LoggerType>
class LoggerController {
public:
    explicit LoggerController(LoggerType& logger);
    ~LoggerController();

    void start();
    void stop();

private:
    void periodicLogging();
    void threadedLogging();

    LoggerType& logger;
    std::atomic<bool> exitFlag;
    std::thread logThread;
    std::thread multiThreadedLogger;
    std::thread configWatcher;
};

// ✅ Move implementations here (instead of a separate .cpp file)

template <typename LoggerType>
LoggerController<LoggerType>::LoggerController(LoggerType& logger)
    : logger(logger), exitFlag(false) {}

template <typename LoggerType>
LoggerController<LoggerType>::~LoggerController() {
    stop();
}

template <typename LoggerType>
void LoggerController<LoggerType>::start() {
    logger.init();
    logThread = std::thread(&LoggerController::periodicLogging, this);
    multiThreadedLogger = std::thread(&LoggerController::threadedLogging, this);
    configWatcher = std::thread(&FileWatcher<LoggerType>::watch,
                                std::ref(logger),
                                "config/logger.conf",
                                std::ref(exitFlag));
}

template <typename LoggerType>
void LoggerController<LoggerType>::stop() {
    exitFlag.store(true);
    if (logThread.joinable()) logThread.join();
    if (multiThreadedLogger.joinable()) multiThreadedLogger.join();
    if (configWatcher.joinable()) configWatcher.join();
}

template <typename LoggerType>
void LoggerController<LoggerType>::periodicLogging() {
    while (!exitFlag.load()) {
        logger.log("INFO", "GENERAL", "Heartbeat log...");
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

template <typename LoggerType>
void LoggerController<LoggerType>::threadedLogging() {
    while (!exitFlag.load()) {
        std::thread logThread([&]() {
            LoggerType& _logger = logger; // ✅ Rename lambda capture variable

            for (int i = 0; i < 5; i++) {
                _logger.log("INFO", "MULTI_THREAD", ("Threaded log #" + std::to_string(i)).c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
        logThread.join();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

#endif // LOGGER_CONTROLLER_HPP
