#pragma once

#include "logger.hpp"
#include "loggerAPI.hpp"
#include <tuple>
#include <memory>

template <typename... Backends>
class LoggerInstance {
public:
    explicit LoggerInstance(std::shared_ptr<LoggerSettings> s, Backends&&... backends)
        : settings(std::move(s)),
          backends(std::make_tuple(std::move(backends)...)),  // ✅ Store as tuple (ownership)
          logger(Logger<Backends...>::createLogger(settings, std::get<Backends>(this->backends)...)) {

        api.log = [](void* instance, std::string_view level, std::string_view context, std::string_view message) {
            static_cast<LoggerInstance*>(instance)->log(level, context, message);
        };
    }

    myLogger::LoggerAPI* getAPI() { return &api; }
    void* getInstance() { return this; }

    void shutdown() {
        logger->shutdown();
    }

private:
    std::shared_ptr<LoggerSettings> settings;
    std::tuple<Backends...> backends;  // ✅ Own the backends inside the instance
    std::unique_ptr<Logger<Backends...>> logger;
    myLogger::LoggerAPI api;

    void log(std::string_view level, std::string_view context, std::string_view message) {
        logger->log(level, context, message);
    }
};
