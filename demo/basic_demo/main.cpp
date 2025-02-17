#include "my_logger.hpp"
#include "logger_controller.hpp"
#include <iostream>

int main() {
    // ✅ Create logger with ConsoleBackend and FileBackend
    auto logger = Logger<ConsoleBackend, FileBackend>::createLogger();

    // ✅ Create LoggerController and start logging
    LoggerController loggerController(*logger);
    loggerController.start();

    std::cout << "[Main] LoggerController started. Press Enter to stop...\n";
    std::cin.get();  // Wait for user input

    std::cout << "[Main] Stopping logger...\n";
    loggerController.stop();

    return 0;
}
