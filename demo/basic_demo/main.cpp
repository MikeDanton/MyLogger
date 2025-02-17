#include "my_logger.hpp"
#include "logger_controller.hpp"
#include <iostream>

int main() {
    // ✅ Create LoggerModules instance
    LoggerModules modules;

    // ✅ Create backends with modules
    ConsoleBackend consoleBackend(modules);
    FileBackend fileBackend(modules);

    // ✅ Create Logger using the factory function
    auto logger = Logger<ConsoleBackend, FileBackend>::createLogger();

    // ✅ Pass logger by reference
    LoggerController loggerController(*logger);
    loggerController.start();

    std::cout << "[Main] LoggerController started. Press Enter to stop...\n";
    std::cin.get();  // Wait for user input

    std::cout << "[Main] Stopping logger...\n";
    loggerController.stop();

    return 0;
}
