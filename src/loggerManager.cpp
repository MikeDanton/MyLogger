#include "loggerManager.hpp"
#include "loggerConfig.hpp"
#include "loggerSettings.hpp"
#include "logLevel.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <memory>
#include <filesystem>

// ✅ Singleton Logger instance (auto-configured on first use)
Logger& LoggerManager::getInstance() {
    static std::unique_ptr<Logger> globalLogger = std::make_unique<Logger>();
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        initialize(*globalLogger);  // ✅ Pass reference to logger
    }

    return *globalLogger;
}

void LoggerManager::initialize(Logger& logger) {
    const std::string configPath = "logger.conf";

    LoggerSettings::getInstance();  // ✅ Ensure settings are initialized first
    LoggerConfig::generateDefaultConfig(configPath);
    LoggerConfig::loadConfig(configPath);

    bool useConsole = LoggerSettings::getInstance().isConsoleEnabled();
    bool useFile = LoggerSettings::getInstance().isFileEnabled();
    LogLevel level = logLevelFromString(LoggerSettings::getInstance().getLogLevel());

    logger.setLogLevel(level);

    if (useConsole && !hasBackend<ConsoleBackend>(logger)) {
        logger.addBackend(std::make_unique<ConsoleBackend>());
    }

    if (useFile && !hasBackend<FileBackend>(logger)) {
        logger.addBackend(std::make_unique<FileBackend>());
    }
}

template <typename T>
bool LoggerManager::hasBackend(Logger& logger) {
    for (const auto& backend : logger.getBackends()) {
        if (dynamic_cast<T*>(backend.get())) {
            return true;
        }
    }
    return false;
}
