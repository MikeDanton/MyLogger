#include "loggerManager.hpp"
#include "loggerConfig.hpp"
#include "logLevel.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <memory>

// ✅ Singleton Logger instance
Logger& LoggerManager::getInstance() {
    static std::unique_ptr<Logger> globalLogger = std::make_unique<Logger>();
    return *globalLogger;
}

// ✅ Configure Logger from config file (auto-loads or creates default)
void LoggerManager::configureFromConfig(const std::string& configPath) {
    LoggerConfig::loadConfig(configPath);  // 🔹 Loads existing config or generates default

    bool useConsole = LoggerConfig::isConsoleEnabled();
    bool useFile = LoggerConfig::isFileEnabled();
    LogLevel level = logLevelFromString(LoggerConfig::getLogLevel());

    configure(useConsole, useFile, level);
}

void LoggerManager::configure(bool useConsole, bool useFile, LogLevel level) {
    Logger& logger = getInstance();
    logger.setLogLevel(level);

    if (useConsole && !hasBackend<ConsoleBackend>()) {
        logger.addBackend(std::make_unique<ConsoleBackend>());
    }

    if (useFile && !hasBackend<FileBackend>()) {
        logger.addBackend(std::make_unique<FileBackend>());
    }
}

// ✅ Check if backend of a given type is already added
template <typename T, typename... Args>
bool LoggerManager::hasBackend(Args&&... args) {
    Logger& logger = getInstance();
    for (const auto& backend : logger.getBackends()) {
        if (dynamic_cast<T*>(backend.get())) {
            return true;
        }
    }
    return false;
}
