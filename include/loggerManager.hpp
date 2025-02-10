#ifndef LOGGER_MANAGER_HPP
#define LOGGER_MANAGER_HPP

#include "logger.hpp"

class LoggerManager {
public:
    static Logger& getInstance();

private:
    LoggerManager() = default;
    ~LoggerManager() = default;

    static void initialize(Logger& logger);

    template <typename T>
    static bool hasBackend(Logger& logger);
};

#endif // LOGGER_MANAGER_HPP
