#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <string>
#include "logger.h"

class LoggerConfig {
public:
    static void loadConfig(const std::string& filepath, LoggerSettings& settings);
    static void generateDefaultConfig(const std::string& filepath);
};

#endif // LOGGER_CONFIG_HPP
