#include "logger.h"
#include <thread>
#include <chrono>
#include <iostream>

void logTestMessages(Logger& logger) {
    logger.log("INFO", "GENERAL", "Logger started successfully.");
    logger.log("DEBUG", "NETWORK", "Initializing network module...");
    logger.log("WARN", "DATABASE", "Database connection is slow.");
    logger.log("ERROR", "UI", "Failed to load UI assets!");
    logger.log("INFO", "AUDIO", "Audio system initialized.");
    logger.log("DEBUG", "RENDERING", "Loading textures...");
    logger.log("ERROR", "DATABASE", "Database query failed!");
    logger.log("WARN", "GENERAL", "Low disk space warning.");
    logger.log("INFO", "GENERAL", "Application is running.");
}

void threadedLogging(Logger& logger) {
    std::thread logThread([&]() {
        for (int i = 0; i < 10; i++) {
            logger.log("INFO", "MULTI_THREAD", ("Threaded log entry #" + std::to_string(i)).c_str());
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
    logThread.join();
}

int main() {
    Logger& logger = Logger::getInstance();
    logger.init();

    std::cout << "Logging messages with different levels & contexts...\n";
    logTestMessages(logger);

    std::cout << "Testing threaded logging...\n";
    threadedLogging(logger);

    std::cout << "Flushing logs before exit...\n";
    logger.flush();  // ✅ Ensures all logs are written before exiting

    std::cout << "Logger test completed.\n";
    return 0;
}
