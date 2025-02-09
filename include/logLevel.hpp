#ifndef LOG_LEVEL_HPP
#define LOG_LEVEL_HPP

#include <string>

enum class LogLevel { INFO, WARN, ERROR, DEBUG };

inline std::string to_string(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARN: return "WARN";
    case LogLevel::ERROR: return "ERROR";
    case LogLevel::DEBUG: return "DEBUG";
    default: return "UNKNOWN";
    }
}

inline LogLevel logLevelFromString(const std::string& levelStr) {
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARN") return LogLevel::WARN;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    return LogLevel::INFO;  // Default if unknown
}

#endif // LOG_LEVEL_HPP
