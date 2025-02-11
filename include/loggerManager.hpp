#ifndef LOGGER_MANAGER_HPP
#define LOGGER_MANAGER_HPP

#include "logger.hpp"
#include "loggerSettings.hpp"

class LoggerManager {
public:
    static Logger<LOGGING_ENABLED>& getInstance();

private:
    LoggerManager() = default;
    ~LoggerManager() = default;

    static void initialize(Logger<LOGGING_ENABLED>& logger);

    template <typename T>
    static bool hasBackend(Logger<LOGGING_ENABLED>& logger);
};

#endif // LOGGER_MANAGER_HPP
