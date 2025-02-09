#include "loggerManager.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <memory>

Logger& LoggerManager::getInstance() {
    static std::unique_ptr<Logger> globalLogger = std::make_unique<Logger>();
    return *globalLogger;
}

void LoggerManager::configure(bool useConsole, bool useFile, LogLevel level) {
    Logger& logger = getInstance();
    logger.setLogLevel(level);

    static bool configured = false;
    if (configured) return;
    configured = true;

    if (useConsole) {
        logger.addBackend(std::make_unique<ConsoleBackend>());
    }

    if (useFile) {
        logger.addBackend(std::make_unique<FileBackend>());
    }
}
