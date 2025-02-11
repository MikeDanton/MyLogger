#include "loggerManager.hpp"
#include "loggerConfig.hpp"
#include "loggerSettings.hpp"
#include "consoleBackend.hpp"
#include "fileBackend.hpp"
#include <memory>
#include <filesystem>

Logger<LOGGING_ENABLED>& LoggerManager::getInstance() {
    static Logger<LOGGING_ENABLED> globalLogger;
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        initialize(globalLogger);
    }

    return globalLogger;
}

void LoggerManager::initialize(Logger<LOGGING_ENABLED>& logger) {
    const std::string configPath = "logger.conf";

    LoggerSettings::getInstance();
    LoggerConfig::loadConfig(configPath);

    bool useConsole = LoggerSettings::getInstance().isConsoleEnabled();
    bool useFile = LoggerSettings::getInstance().isFileEnabled();

    std::string level = LoggerSettings::getInstance().getContextLogLevel("GENERAL");

    logger.setLogLevel(level);

    if (useConsole && !hasBackend<ConsoleBackend>(logger)) {
        logger.addBackend(std::make_unique<ConsoleBackend>());
    }

    if constexpr (LOGGING_ENABLED) {
        if (useFile && !hasBackend<FileBackend>(logger)) {
            logger.addBackend(std::make_unique<FileBackend>());
        }
    }
}

template <typename T>
bool LoggerManager::hasBackend(Logger<LOGGING_ENABLED>& logger) {
    for (const auto& backend : logger.getBackends()) {
        if (dynamic_cast<T*>(backend.get())) {
            return true;
        }
    }
    return false;
}
