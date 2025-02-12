#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <string>
#include "logger.hpp"

class LoggerConfig {
public:
    static void precomputeColors(LoggerSettings& settings);
    static void loadConfig(const std::string& filepath, LoggerSettings& settings);
    static void generateDefaultConfig(const std::string& filepath);
    static void loadOrGenerateConfig(const std::string& filepath, LoggerSettings& settings);
};

#endif // LOGGER_CONFIG_HPP
