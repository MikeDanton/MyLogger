#include "my_logger.hpp"
#include "logger_controller.hpp"

int main() {
    ConsoleBackend consoleBackend;
    FileBackend fileBackend;

    auto logger = createLogger(consoleBackend, fileBackend);
    LoggerController loggerController(logger);
    loggerController.start();

    std::cout << "[Main] LoggerController started. Press Enter to stop...\n";

    std::cin.get();  // Wait for user input to stop

    std::cout << "[Main] Stopping logger...\n";
    loggerController.stop();

    return 0;
}
