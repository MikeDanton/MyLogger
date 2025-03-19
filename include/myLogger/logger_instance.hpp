#pragma once

#include "logger.hpp"
#include "loggerAPI.hpp"

template <typename... Backends>
class LoggerInstance {
public:
    explicit LoggerInstance(std::shared_ptr<LoggerSettings> s, Backends... backends)
        : settings(std::move(s)), backendsWrapper(backends...), logCore() {

        LoggerConfig::loadOrGenerateConfig("config/logger.conf", *settings);
        logCore.setBackends(backendsWrapper, *settings);
        backendsWrapper.setup(*settings);

        api.log = [](void* instance, const char* level, const char* context, const char* message) {
            static_cast<LoggerInstance*>(instance)->log(level, context, message);
        };

        api.updateSettings = [](void* instance, const char* configFile) {
            static_cast<LoggerInstance*>(instance)->updateSettings(configFile);
        };

        api.shutdown = [](void* instance) {
            static_cast<LoggerInstance*>(instance)->shutdown();
        };
    }

    LoggerAPI* getAPI() { return &api; }
    void* getInstance() { return this; }

private:
    std::shared_ptr<LoggerSettings> settings;
    LoggerBackends<Backends...> backendsWrapper;
    LoggerCore<LoggerBackends<Backends...>> logCore;

    LoggerAPI api;  // Function pointer table

    void log(std::string_view level, std::string_view context, std::string_view message) {
        if (shouldLog(level, context)) {
            LogMessage logMsg{std::string(level), std::string(context), std::string(message)};
            logCore.enqueueLog(std::move(logMsg), *settings);
        }
    }

    void updateSettings(const std::string& configFile) {
        LoggerConfig::loadOrGenerateConfig(configFile, *settings);
    }

    void shutdown() {
        logCore.shutdown();
        backendsWrapper.shutdown();
    }

    bool shouldLog(std::string_view level, std::string_view context) const {
        return true;  // Simplified logic for filtering
    }
};
