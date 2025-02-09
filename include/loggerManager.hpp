#ifndef LOGGER_MANAGER_HPP
#define LOGGER_MANAGER_HPP

#include "logger.hpp"

class LoggerManager {
public:
    static Logger& getInstance();
    static void configure(bool useConsole, bool useFile, LogLevel level = LogLevel::INFO);

private:
    LoggerManager() = default;
    ~LoggerManager() = default;
};

#endif // LOGGER_MANAGER_HPP
