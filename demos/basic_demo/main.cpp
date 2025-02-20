#include "myLogger/my_logger.hpp"
#include "logger_controller.hpp"
#include <iostream>

int main() {
    // ✅ Create LoggerSettings
    auto settings = std::make_shared<LoggerSettings>();

    // ✅ Create ConsoleBackend & FileBackend instances
    ConsoleBackend consoleBackend;
    FileBackend fileBackend;

    // ✅ Create Logger with explicit settings & backends
    auto logger = Logger<ConsoleBackend, FileBackend>::createLogger(settings, consoleBackend, fileBackend);

    // ✅ Create LoggerController and start logging
    LoggerController loggerController(*logger);
    loggerController.start();

    std::cout << "[Main] LoggerController started. Press Enter to stop...\n";
    std::cin.get();  // Wait for user input

    std::cout << "[Main] Stopping logger...\n";
    loggerController.stop();

    return 0;
}
