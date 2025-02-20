#ifndef LOGGER_CONTROLLER_HPP
#define LOGGER_CONTROLLER_HPP

#include "myLogger/my_logger.hpp"
#include <atomic>
#include <thread>

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

    LoggerType& logger;  // ✅ Fix: Explicit reference type
    std::atomic<bool> exitFlag;
    std::thread logThread;
    std::thread multiThreadedLogger;
    std::thread configWatcher;
};

// ✅ Constructor Implementation
template <typename LoggerType>
LoggerController<LoggerType>::LoggerController(LoggerType& logger)
    : logger(logger), exitFlag(false) {}

// ✅ Destructor (RAII-based cleanup)
template <typename LoggerType>
LoggerController<LoggerType>::~LoggerController() {
    stop();
}

template <typename LoggerType>
void LoggerController<LoggerType>::start() {
    std::cout << "[LoggerController] Starting logger threads...\n";

    logThread = std::thread(&LoggerController::periodicLogging, this);
    multiThreadedLogger = std::thread(&LoggerController::threadedLogging, this);

    std::cout << "[LoggerController] Starting FileWatcher thread...\n";

    // 🔹 Fetch config file from settings (instead of using a non-existent static variable)
    std::string configFilePath = "config/logger.conf";

    configWatcher = std::thread(&FileWatcher<std::decay_t<LoggerType>>::watch,
                                std::ref(logger),
                                configFilePath, // ✅ Now correctly fetches the config path
                                std::ref(exitFlag));

    std::cout << "[LoggerController] All threads started!\n";
}

// ✅ Stop Logging Threads Safely
template <typename LoggerType>
void LoggerController<LoggerType>::stop() {
    exitFlag.store(true);

    if (logThread.joinable()) logThread.join();
    if (multiThreadedLogger.joinable()) multiThreadedLogger.join();
    if (configWatcher.joinable()) configWatcher.join();
}

// ✅ Periodic Logging Thread (Heartbeat Logs)
template <typename LoggerType>
void LoggerController<LoggerType>::periodicLogging() {
    while (!exitFlag.load()) {
        logger.log("INFO", "GENERAL", "Heartbeat log..."); // ✅ Check if this prints
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

// ✅ Multi-threaded Logging Simulation
template <typename LoggerType>
void LoggerController<LoggerType>::threadedLogging() {
    while (!exitFlag.load()) {
        std::thread logThread([&]() {
            for (int i = 0; i < 5; i++) {
                logger.log("INFO", "MULTI_THREAD",
                           ("Threaded log #" + std::to_string(i)).c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });

        logThread.join();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

#endif // LOGGER_CONTROLLER_HPP
