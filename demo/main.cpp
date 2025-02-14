#include "logger.hpp"
#include "logger_controller.hpp"
#include "console_backend.hpp"
#include "file_backend.hpp"

int main() {
    auto settings = std::make_shared<LoggerSettings>();

    ConsoleBackend consoleBackend;
    FileBackend fileBackend;

    // ✅ Structured Backend Management
    LoggerBackends<ConsoleBackend, FileBackend> backends(consoleBackend, fileBackend);

    // ✅ Logger Now Uses a Struct for Backends
    Logger<decltype(backends)> logger(settings, backends);

    // ✅ Use `LoggerController` for better testing
    LoggerController loggerController(logger);
    loggerController.start();

    std::cin.get();  // Wait for user input to stop

    loggerController.stop();
    return 0;
}
