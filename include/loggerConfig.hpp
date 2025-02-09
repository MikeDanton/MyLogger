#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include "logLevel.hpp"
#include <string>
#include <unordered_map>

class LoggerConfig {
public:
    static void loadConfig(const std::string& filepath);
    static void saveConfig(const std::string& filepath);

    static std::string getColorForLevel(LogLevel level);

    static std::string getLogDirectory();
    static std::string getLogFilenameFormat();
    static int getLogRotationDays();
    static bool isConsoleEnabled();
    static bool isFileEnabled();
    static bool isColorEnabled();
    static std::string getFlushMode();
    static std::string getLogLevel();

private:
    static std::unordered_map<std::string, std::string> config;
};

#endif // LOGGER_CONFIG_HPP
