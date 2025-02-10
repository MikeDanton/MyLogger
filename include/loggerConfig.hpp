#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <string>

class LoggerConfig {
public:
    static void loadConfig(const std::string& filepath);
    static void generateDefaultConfig(const std::string& filepath);
};

#endif // LOGGER_CONFIG_HPP
