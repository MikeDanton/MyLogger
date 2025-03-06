#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "myLogger/logger_core.hpp"
#include "myLogger/logger_config.hpp"
#include <memory>
#include <tuple>
#include <iostream>
#include <unordered_set>
#include <fstream>
#include <mutex>
#include <filesystem>

//------------------------------------------------------------------------------
// LoggerBackends: Manages multiple logging backends
//------------------------------------------------------------------------------
template <typename... Backends>
struct LoggerBackends {
    std::tuple<Backends&...> backends;
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
    static std::unique_ptr<Logger<Backends...>> createLogger(std::shared_ptr<LoggerSettings> s, Backends&... backends);
    Logger(std::shared_ptr<LoggerSettings> s, Backends&... backends);
    ~Logger();
    void log(std::string_view level, std::string_view context, std::string_view message);
    void updateSettings(const std::string& configFile);
    void shutdown();

private:
    std::shared_ptr<LoggerSettings> settings;
    std::tuple<Backends&...> backendStorage;
    LoggerBackends<Backends...> backendsWrapper;
    LoggerCore<LoggerBackends<Backends...>> logCore;

    mutable std::unordered_set<std::string> trackedContexts;
    mutable std::mutex contextMutex;

    bool shouldLog(std::string_view level, std::string_view context) const;
    void updateConfigWithNewContexts();
};

//------------------------------------------------------------------------------
// Factory Method
//------------------------------------------------------------------------------
template <typename... Backends>
std::unique_ptr<Logger<Backends...>> Logger<Backends...>::createLogger(std::shared_ptr<LoggerSettings> s, Backends&... backends) {
    return std::make_unique<Logger<Backends...>>(std::move(s), backends...);
}

//------------------------------------------------------------------------------
// Constructor
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
// Destructor
//------------------------------------------------------------------------------
template <typename... Backends>
Logger<Backends...>::~Logger() {
    shutdown();
}

//------------------------------------------------------------------------------
// Shutdown
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::shutdown() {
    updateConfigWithNewContexts();
    logCore.shutdown();
    backendsWrapper.shutdown();
    std::cout << "[Logger] Shutdown complete.\n";
}

//------------------------------------------------------------------------------
// Update Config with New Contexts
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::updateConfigWithNewContexts() {
    if (trackedContexts.empty()) return;

    std::string configFile = "config/logger.conf";
    toml::table config;

    // ✅ Load existing config if possible
    if (std::filesystem::exists(configFile)) {
        try {
            config = toml::parse_file(configFile);
        } catch (const toml::parse_error& err) {
            std::cerr << "[Logger] Error parsing existing config: " << err.what() << "\n";
            return;
        }
    }

    // ✅ Ensure [contexts] table exists
    if (!config.contains("contexts")) {
        config.insert("contexts", toml::table{});
    }
    auto& contextsTable = *config["contexts"].as_table();

    // ✅ Ensure [colors.context] table exists
    if (!config.contains("colors")) {
        config.insert("colors", toml::table{});
    }
    auto& colorsTable = *config["colors"].as_table();
    if (!colorsTable.contains("context")) {
        colorsTable.insert("context", toml::table{});
    }
    auto& colorsContextTable = *colorsTable["context"].as_table();

    {
        std::lock_guard<std::mutex> lock(contextMutex);

        // ✅ Add missing contexts with default severity and color
        for (const auto& context : trackedContexts) {
            if (!contextsTable.contains(context)) {
                contextsTable.insert(context, "INFO");  // Default log level
            }
            if (!colorsContextTable.contains(context)) {
                colorsContextTable.insert(context, "WHITE");  // Default white color
            }
        }
        trackedContexts.clear();
    }

    // ✅ Write updated config back to file
    std::ofstream outFile(configFile);
    if (!outFile) {
        std::cerr << "[Logger] Failed to open config file for writing.\n";
        return;
    }
    outFile << config;
    outFile.close();

    std::cout << "[Logger] Updated config file with new contexts and colors.\n";
}

//------------------------------------------------------------------------------
// Update Settings
//------------------------------------------------------------------------------
template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadOrGenerateConfig(configFile, *settings);
}

//------------------------------------------------------------------------------
// Should Log?
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
    {
        std::lock_guard<std::mutex> lock(contextMutex);
        auto ctxIt = settings->config.contexts.contextIndexMap.find(std::string(context));
        if (ctxIt == settings->config.contexts.contextIndexMap.end()) {
            trackedContexts.insert(std::string(context));
        } else {
            contextIndex = ctxIt->second;
        }
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
// Log Message
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
