#ifndef LOGGER_MANAGER_HPP
#define LOGGER_MANAGER_HPP

#include "logger.hpp"

class LoggerManager {
public:
    static Logger& getInstance();

    static void configureFromConfig(const std::string& configPath);
    static void configure(bool useConsole, bool useFile, LogLevel level = LogLevel::INFO);

private:
    LoggerManager() = default;
    ~LoggerManager() = default;

    // 🔹 Checks if backend already exists (prevents duplicates)
    template <typename T, typename... Args>
    static bool hasBackend(Args&&... args);
};

#endif // LOGGER_MANAGER_HPP
