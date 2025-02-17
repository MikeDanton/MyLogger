#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logger_core.hpp"
#include "logger_config.hpp"
#include "logger_modules.hpp"
#include "console_backend.hpp"
#include "file_backend.hpp"
#include <memory>
#include <tuple>
#include <iostream>

//------------------------------------------------------------------------------
// LoggerBackends: Wraps references to actual backend objects
//------------------------------------------------------------------------------
template <typename... Backends>
struct LoggerBackends {
    std::tuple<Backends&...> backends; // references to the actual backend objects

    explicit LoggerBackends(Backends&... b) : backends(std::tie(b...)) {}

    void dispatchLog(const LogMessage& logMsg, const LoggerSettings& settings) {
        std::apply(
            [&](auto&... backend) { ((backend.write(logMsg, settings)), ...); },
            backends
        );
    }

    void setup(const LoggerSettings& settings) {
        std::apply(
            [&](auto&... backend) { ((backend.setup(settings)), ...); },
            backends
        );
    }

    void shutdown() {
        std::apply(
            [&](auto&... backend) { ((shutdownIfAvailable(backend)), ...); },
            backends
        );
    }

private:
    template <typename B>
    void shutdownIfAvailable(B& backend) {
        if constexpr (requires { backend.shutdown(); }) {
            backend.shutdown();
        }
    }
};

//------------------------------------------------------------------------------
// Logger: Owns modules + actual backend objects
//------------------------------------------------------------------------------
template <typename... Backends>
class Logger {
public:
    // 1) Factory Method for creating a Logger instance
    static std::unique_ptr<Logger<Backends...>> createLogger();

    // 2) Public default constructor (now accessible by make_unique)
    Logger();

    // 3) Destructor & standard methods
    ~Logger();
    void log(const char* level, const char* context, const char* message);
    void updateSettings(const std::string& configFile);
    void shutdown();

    static constexpr const char* CONFIG_FILE = "config/logger.conf";

private:
    // We store everything needed for logger's lifetime
    std::shared_ptr<LoggerSettings> settings;
    std::unique_ptr<LoggerModules> modules;

    // Own the actual backend objects in a tuple
    std::tuple<Backends...> backendStorage;

    // Then create references to them for LoggerCore
    LoggerBackends<Backends...> backends;

    LoggerCore<LoggerBackends<Backends...>> logCore;

    // Optional additional constructor if passing custom settings or modules
    Logger(std::shared_ptr<LoggerSettings> s, std::unique_ptr<LoggerModules> m);

    bool shouldLog(const char* level, const char* context) const;
};

//------------------------------------------------------------------------------
// 1) Factory Method
//------------------------------------------------------------------------------
template <typename... Backends>
std::unique_ptr<Logger<Backends...>> Logger<Backends...>::createLogger() {
    // Calls public default constructor
    return std::make_unique<Logger<Backends...>>();
}

//------------------------------------------------------------------------------
// 2) Public Default Constructor
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::Logger()
    : settings(std::make_shared<LoggerSettings>())
    , modules(std::make_unique<LoggerModules>())
    , backendStorage(Backends(*modules)...)
    , backends(std::apply([&](auto&... bk) { return LoggerBackends<Backends...>(bk...); }, backendStorage))
    , logCore()
{
    LoggerConfig::loadOrGenerateConfig(CONFIG_FILE, *settings, *modules);
    logCore.setBackends(backends, *settings);
    backends.setup(*settings);
}

//------------------------------------------------------------------------------
// Optional: Constructor that allows custom settings/modules if needed
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::Logger(std::shared_ptr<LoggerSettings> s, std::unique_ptr<LoggerModules> m)
    : settings(std::move(s))
    , modules(std::move(m))
    , backendStorage(Backends(*modules)...)
    , backends(std::apply([&](auto&... bk) { return LoggerBackends<Backends...>(bk...); }, backendStorage))
    , logCore()
{
    LoggerConfig::loadOrGenerateConfig(CONFIG_FILE, *settings, *modules);
    logCore.setBackends(backends, *settings);
    backends.setup(*settings);
}

//------------------------------------------------------------------------------
// 3) Destructor
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::~Logger() {
    shutdown();
}

//------------------------------------------------------------------------------
// shutdown
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::shutdown() {
    std::cout << "[Logger] Initiating shutdown..." << std::endl;
    logCore.shutdown();
    backends.shutdown();
    std::cout << "[Logger] Shutdown complete." << std::endl;
}

//------------------------------------------------------------------------------
// updateSettings
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadConfig(configFile, *settings, *modules);
}

//------------------------------------------------------------------------------
// shouldLog
//------------------------------------------------------------------------------
template <typename... Backends>
bool Logger<Backends...>::shouldLog(const char* level, const char* context) const {
    if (!settings) return false;

    int levelIndex = -1;
    auto lvlIt = settings->config.levels.levelIndexMap.find(level);
    if (lvlIt != settings->config.levels.levelIndexMap.end()) {
        levelIndex = lvlIt->second;
    }

    int contextIndex = -1;
    auto ctxIt = settings->config.contexts.contextIndexMap.find(context);
    if (ctxIt != settings->config.contexts.contextIndexMap.end()) {
        contextIndex = ctxIt->second;
    }

    if (levelIndex == -1 || !settings->config.levels.enabledArray[levelIndex]) {
        return false;
    }

    int msgSeverity = settings->config.levels.severitiesArray[levelIndex];
    int minContextSeverity = (contextIndex != -1)
                               ? settings->config.contexts.contextSeverityArray[contextIndex]
                               : 0;

    return msgSeverity >= minContextSeverity;
}

//------------------------------------------------------------------------------
// log
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::log(const char* level, const char* context, const char* message) {
    if (!shouldLog(level, context)) {
        return;
    }

    LogMessage logMsg{};
    std::strncpy(logMsg.level,   level,   sizeof(logMsg.level)   - 1);
    std::strncpy(logMsg.context, context, sizeof(logMsg.context) - 1);
    std::strncpy(logMsg.message, message, sizeof(logMsg.message) - 1);

    if (settings->config.format.enableTimestamps) {
        std::string timestampStr = getCurrentTimestamp(settings->config.format.timestampFormat);
        std::strncpy(logMsg.timestamp, timestampStr.c_str(), sizeof(logMsg.timestamp) - 1);
    } else {
        logMsg.timestamp[0] = '\0';
    }

    logCore.enqueueLog(std::move(logMsg), *settings);
}

#endif // LOGGER_HPP
