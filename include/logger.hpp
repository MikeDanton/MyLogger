#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logger_core.hpp"
#include "logger_config.hpp"
#include "console_backend.hpp"
#include "file_backend.hpp"
#include <memory>
#include <tuple>
#include <iostream>

//------------------------------------------------------------------------------
// LoggerBackends: Manages multiple logging backends
//------------------------------------------------------------------------------
template <typename... Backends>
struct LoggerBackends {
    std::tuple<Backends&...> backends; // References to actual backend objects

    explicit LoggerBackends(Backends&... b) : backends(std::tie(b...)) {}

    void dispatchLog(const LogMessage& logMsg, const LoggerSettings& settings) {
        std::apply([&](auto&... backend) { ((backend.write(logMsg, settings)), ...); }, backends);
    }

    void setup(const LoggerSettings& settings) {
        std::apply([&](auto&... backend) { ((backend.setup(settings)), ...); }, backends);
    }

    void shutdown() {
        std::apply([&](auto&... backend) { ((shutdownIfAvailable(backend)), ...); }, backends);
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
// Logger: Owns backend objects and settings
//------------------------------------------------------------------------------
template <typename... Backends>
class Logger {
public:
    // ✅ Factory method for creating a logger instance
    static std::unique_ptr<Logger<Backends...>> createLogger(std::shared_ptr<LoggerSettings> s, Backends&... backends);

    // ✅ Constructor that accepts backends explicitly
    Logger(std::shared_ptr<LoggerSettings> s, Backends&... backends);

    // ✅ Destructor & logging methods
    ~Logger();
    void log(std::string_view level, std::string_view context, std::string_view message);
    void updateSettings(const std::string& configFile);
    void shutdown();

private:
    std::shared_ptr<LoggerSettings> settings;
    std::tuple<Backends&...> backendStorage;
    LoggerBackends<Backends...> backendsWrapper;
    LoggerCore<LoggerBackends<Backends...>> logCore;

    bool shouldLog(std::string_view level, std::string_view context) const;
};

//------------------------------------------------------------------------------
// ✅ Factory Method: Creates Logger with Provided Backends
//------------------------------------------------------------------------------
template <typename... Backends>
std::unique_ptr<Logger<Backends...>> Logger<Backends...>::createLogger(std::shared_ptr<LoggerSettings> s, Backends&... backends) {
    return std::make_unique<Logger<Backends...>>(std::move(s), backends...);
}

//------------------------------------------------------------------------------
// ✅ Constructor: Accepts Backends Explicitly
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::Logger(std::shared_ptr<LoggerSettings> s, Backends&... backends)
    : settings(std::move(s))
    , backendStorage(backends...)
    , backendsWrapper(backends...)
    , logCore()
{
    LoggerConfig::loadOrGenerateConfig("config/logger.conf", *settings);
    logCore.setBackends(backendsWrapper, *settings);
    backendsWrapper.setup(*settings);
}

//------------------------------------------------------------------------------
// ✅ Destructor
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::~Logger() {
    shutdown();
}

//------------------------------------------------------------------------------
// ✅ Shutdown
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::shutdown() {
    std::cout << "[Logger] Initiating shutdown..." << std::endl;
    logCore.shutdown();
    backendsWrapper.shutdown();
    std::cout << "[Logger] Shutdown complete." << std::endl;
}

//------------------------------------------------------------------------------
// ✅ updateSettings
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, *settings);
}

//------------------------------------------------------------------------------
// ✅ shouldLog
//------------------------------------------------------------------------------
template <typename... Backends>
bool Logger<Backends...>::shouldLog(std::string_view level, std::string_view context) const {
    if (!settings) return false;

    int levelIndex = -1;
    auto lvlIt = settings->config.levels.levelIndexMap.find(std::string(level));
    if (lvlIt != settings->config.levels.levelIndexMap.end()) {
        levelIndex = lvlIt->second;
    }

    int contextIndex = -1;
    auto ctxIt = settings->config.contexts.contextIndexMap.find(std::string(context));
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
// ✅ log
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::log(std::string_view level, std::string_view context, std::string_view message) {
    if (!shouldLog(level, context)) {
        return;
    }

    LogMessage logMsg{std::string(level), std::string(context), std::string(message)};

    if (settings->config.format.enableTimestamps) {
        logMsg.setTimestamp(getCurrentTimestamp(settings->config.format.timestampFormat));
    }

    logCore.enqueueLog(std::move(logMsg), *settings);
}

#endif // LOGGER_HPP
