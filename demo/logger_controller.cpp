#include "logger_controller.hpp"
#include "file_watcher.hpp"
#include <chrono>
#include <iostream>

LoggerController::LoggerController()
    : logger(Logger::getInstance()), exitFlag(false) {}

LoggerController::~LoggerController() {
    stop();
}

void LoggerController::start() {
    logger.init();
    std::cout << "🌍 Logger is running. Modify `config/logger.conf` to see changes in real-time!\n";
    std::cout << "Press ENTER to stop.\n";

    logThread = std::thread(&LoggerController::periodicLogging, this);
    multiThreadedLogger = std::thread(&LoggerController::threadedLogging, this);
    configWatcher = std::thread(FileWatcher::watch, std::ref(logger), "config/logger.conf", std::ref(exitFlag));
}

void LoggerController::stop() {
    exitFlag.store(true);

    if (logThread.joinable()) logThread.join();
    if (multiThreadedLogger.joinable()) multiThreadedLogger.join();
    if (configWatcher.joinable()) configWatcher.join();

    std::cout << "Flushing logs before exit...\n";
    logger.flush();
    std::cout << "Logger test completed. Goodbye!\n";
}

void LoggerController::periodicLogging() {
    int counter = 0;
    while (!exitFlag.load()) {
        logger.log("INFO", "GENERAL", ("Heartbeat log #" + std::to_string(counter++)).c_str());
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void LoggerController::threadedLogging() {
    while (!exitFlag.load()) {
        std::thread logThread([&]() {
            for (int i = 0; i < 5; i++) {
                logger.log("INFO", "MULTI_THREAD", ("Threaded log #" + std::to_string(i)).c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
        logThread.join();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}
