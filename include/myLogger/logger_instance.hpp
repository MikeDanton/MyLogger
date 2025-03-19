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

        // Expose only `log()`
        api.log = [](void* instance, const char* level, const char* context, const char* message) {
            static_cast<LoggerInstance*>(instance)->log(level, context, message);
        };
    }

    myLogger::LoggerAPI* getAPI() { return &api; }
    void* getInstance() { return this; }

    void shutdown() {
        logCore.shutdown();
        backendsWrapper.shutdown();
    }

private:
    std::shared_ptr<LoggerSettings> settings;
    LoggerBackends<Backends...> backendsWrapper;
    LoggerCore<LoggerBackends<Backends...>> logCore;

    myLogger::LoggerAPI api;  // Function pointer table

    void log(std::string_view level, std::string_view context, std::string_view message) {
        if (shouldLog(level, context)) {
            LogMessage logMsg{std::string(level), std::string(context), std::string(message)};
            logCore.enqueueLog(std::move(logMsg), *settings);
        }
    }

    bool shouldLog(std::string_view level, std::string_view context) const {
        return true;  // Simplified logic for filtering
    }
};
