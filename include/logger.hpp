#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logger_core.hpp"
#include "logger_config.hpp"
#include <memory>
#include <tuple>
#include <iostream>

// ✅ Struct to Hold Multiple Backends
template <typename... Backends>
struct LoggerBackends {
    std::tuple<Backends&...> backends;

    explicit LoggerBackends(Backends&... backends) : backends(std::tie(backends...)) {}

    void dispatchLog(const LogMessage& logMsg, const LoggerSettings& settings) {
        std::apply([&](auto&... backend) {
            ((backend.write(logMsg, settings)), ...);
        }, backends);
    }
};

// ✅ Logger Template (Now Uses LoggerCore & Struct-Based Backends)
template <typename Backends>
class Logger {
public:
    explicit Logger(std::shared_ptr<LoggerSettings> settings, Backends& backends);

    void init(const std::string& configFile = "config/logger.conf");
    void log(const char* level, const char* context, const char* message);
    void flush();
    void updateSettings(const std::string& configFile);

private:
    std::shared_ptr<LoggerSettings> settings;
    Backends& backends;
    LoggerCore<Backends> logCore;

    bool shouldLog(const char* level, const char* context) const;
};

// ✅ Constructor Implementation
template <typename Backends>
Logger<Backends>::Logger(std::shared_ptr<LoggerSettings> settings, Backends& backends)
    : settings(std::move(settings)), backends(backends), logCore() {}

// ✅ Initialize Logger and Pass Settings to Core
template <typename Backends>
void Logger<Backends>::init(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, *settings);
    logCore.setBackends(backends, *settings); // Ensure backends are linked to LoggerCore

    std::apply([&](auto&... backend) {
        ((backend.setup(*settings)), ...);
    }, backends.backends);
}

// ✅ Update Configuration
template <typename Backends>
void Logger<Backends>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadConfig(configFile, *settings);
}

// ✅ Determine if a Log Message Should Be Logged
template <typename Backends>
bool Logger<Backends>::shouldLog(const char* level, const char* context) const {
    if (!settings) return false;

    int levelIndex = settings->config.levels.levelIndexMap.contains(level)
                        ? settings->config.levels.levelIndexMap.at(level)
                        : -1;

    int contextIndex = settings->config.contexts.contextIndexMap.contains(context)
                        ? settings->config.contexts.contextIndexMap.at(context)
                        : -1;

    if (levelIndex == -1 || !settings->config.levels.enabledArray[levelIndex]) return false;

    int msgSeverity = settings->config.levels.severitiesArray[levelIndex];
    int minContextSeverity = (contextIndex != -1)
                                ? settings->config.contexts.contextSeverityArray[contextIndex]
                                : 0;

    return msgSeverity >= minContextSeverity;
}

// ✅ Logging Function
template <typename Backends>
void Logger<Backends>::log(const char* level, const char* context, const char* message) {
    if (!shouldLog(level, context)) return;

    LogMessage logMsg{
        level,
        context,
        message,
        settings->config.format.enableTimestamps ?
            getCurrentTimestamp(settings->config.format.timestampFormat) : ""
    };

    logCore.enqueueLog(std::move(logMsg), *settings);
}

// ✅ Flush Logs and Process Queue
template <typename Backends>
void Logger<Backends>::flush() {
    logCore.processQueue();
}

#endif // LOGGER_HPP
