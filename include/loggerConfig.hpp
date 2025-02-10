#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include "logLevel.hpp"
#include "logContext.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

class LoggerConfig {
public:
    static void loadConfig(const std::string& filepath);
    static void saveConfig(const std::string& filepath);

    static std::string getColorForLog(LogLevel level, LogContext context);
    static bool isContextEnabled(LogContext context);

    static std::string getLogDirectory();
    static std::string getLogFilenameFormat();
    static std::string getColorPriority();
    static int getLogRotationDays();
    static bool isConsoleEnabled();
    static bool isFileEnabled();
    static bool isColorEnabled();
    static std::string getFlushMode();
    static std::string getLogLevel();
    static std::string getColorMode();

private:
    static std::unordered_map<std::string, std::string> config;
    static std::unordered_set<LogContext> enabledContexts;

    static std::string trim(const std::string& str);
};

#endif // LOGGER_CONFIG_HPP
