#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "logger_core.hpp"
#include "logger_config.hpp"
#include <memory>
#include <tuple>
#include <iostream>

template <typename... Backends>
struct LoggerBackends {
    static_assert(sizeof...(Backends) > 0, "Logger must have at least one backend.");
    std::tuple<Backends&...> backends;

    explicit LoggerBackends(Backends&... backends) : backends(std::tie(backends...)) {}

    void dispatchLog(const LogMessage& logMsg, const LoggerSettings& settings) {
        std::apply([&](auto&... backend) { ((backend.write(logMsg, settings)), ...); }, backends);
    }

    void setup(const LoggerSettings& settings) {
        std::apply([&](auto&... backend) { ((backend.setup(settings)), ...); }, backends);
    }

    // ✅ Calls shutdown() **only** on backends that support it
    template <typename T>
    using has_shutdown = decltype(std::declval<T>().shutdown(), std::true_type{});

    template <typename T>
    static constexpr bool has_shutdown_v = std::is_same_v<has_shutdown<T>, std::true_type>;

    void shutdown() {
        std::apply([&](auto&... backend) {
            ((has_shutdown_v<std::decay_t<decltype(backend)>> ? backend.shutdown() : void()), ...);
        }, backends);
    }
};

template <typename... Backends>
class Logger {
public:
    explicit Logger(std::shared_ptr<LoggerSettings> settings, Backends&... backends);
    ~Logger();

    void log(const char* level, const char* context, const char* message);
    void updateSettings(const std::string& configFile);
    void shutdown();

    static constexpr const char* CONFIG_FILE = "config/logger.conf";

private:
    std::shared_ptr<LoggerSettings> settings;
    LoggerBackends<Backends...> backends;
    LoggerCore<LoggerBackends<Backends...>> logCore;

    bool shouldLog(const char* level, const char* context) const;
};

template <typename... Backends>
Logger<Backends...> createLogger(Backends&... backends) {
    auto settings = std::make_shared<LoggerSettings>();
    return Logger<Backends...>(std::move(settings), backends...);
}

template <typename... Backends>
Logger<Backends...>::Logger(std::shared_ptr<LoggerSettings> settings, Backends&... backends)
    : settings(std::move(settings)), backends(backends...), logCore() {
    LoggerConfig::loadOrGenerateConfig(CONFIG_FILE, *this->settings);
    logCore.setBackends(this->backends, *this->settings);

    std::apply([&](auto&... backend) {
        ((backend.setup(*this->settings)), ...);
    }, this->backends.backends);
}

template <typename... Backends>
Logger<Backends...>::~Logger() {
    shutdown();
}

template <typename... Backends>
void Logger<Backends...>::shutdown() {
    std::cout << "[Logger] Initiating shutdown..." << std::endl;
    logCore.shutdown();  // Properly stops the logging thread
    backends.shutdown();
    std::cout << "[Logger] Shutdown complete." << std::endl;
}

template <typename... Backends>
void Logger<Backends...>::updateSettings(const std::string& configFile) {
    LoggerConfig::loadConfig(configFile, *settings);
}

template <typename... Backends>
bool Logger<Backends...>::shouldLog(const char* level, const char* context) const {
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

template <typename... Backends>
void Logger<Backends...>::log(const char* level, const char* context, const char* message) {
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

#endif // LOGGER_HPP
