#include <console_backend.hpp>
#include <file_backend.hpp>
#include <iostream>

#include "logger.hpp"
#include "logger_controller.hpp"

int main() {
    auto settings = std::make_shared<LoggerSettings>();  // ✅ Use shared_ptr
    ConsoleBackend consoleBackend;
    FileBackend fileBackend;

    Logger<ConsoleBackend, FileBackend> logger(settings, consoleBackend, fileBackend);
    LoggerController<Logger<ConsoleBackend, FileBackend>> controller(logger);

    controller.start();

    std::cin.get();  // Wait for user input to stop

    controller.stop();
    return 0;
}