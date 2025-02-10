#include "loggerManager.hpp"
#include "loggerConfig.hpp"
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
        initialized = true;  // ✅ Ensure it's set first
        initialize(*globalLogger);  // ✅ Pass reference to logger
    }

    return *globalLogger;
}

// ✅ Auto-initialization logic (ensures config file is created)
void LoggerManager::initialize(Logger& logger) {
    const std::string configPath = "logger.conf";

    // 🔹 If config file doesn’t exist, generate default
    LoggerConfig::generateDefaultConfig(configPath);

    // 🔹 Load config (even if just created)
    LoggerConfig::loadConfig(configPath);

    bool useConsole = LoggerConfig::isConsoleEnabled();
    bool useFile = LoggerConfig::isFileEnabled();
    LogLevel level = logLevelFromString(LoggerConfig::getLogLevel());

    // ✅ Directly modify the existing logger instead of calling `getInstance()`
    logger.setLogLevel(level);

    if (useConsole && !hasBackend<ConsoleBackend>(logger)) {
        logger.addBackend(std::make_unique<ConsoleBackend>());
    }

    if (useFile && !hasBackend<FileBackend>(logger)) {
        logger.addBackend(std::make_unique<FileBackend>());
    }
}

// ✅ Check if a backend already exists (avoid recursion)
template <typename T>
bool LoggerManager::hasBackend(Logger& logger) {
    for (const auto& backend : logger.getBackends()) {
        if (dynamic_cast<T*>(backend.get())) {
            return true;
        }
    }
    return false;
}
