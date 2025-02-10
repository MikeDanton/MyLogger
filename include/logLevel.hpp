#ifndef LOG_LEVEL_HPP
#define LOG_LEVEL_HPP

#include <string>
#include <iostream>

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

inline std::string logLevelColor(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "\033[32m";  // Green
    case LogLevel::WARN: return "\033[33m";  // Yellow
    case LogLevel::ERROR: return "\033[31m"; // Red
    case LogLevel::DEBUG: return "\033[36m"; // Cyan
    default: return "";
    }
}

inline LogLevel logLevelFromString(const std::string& levelStr) {
    if (levelStr == "INFO") return LogLevel::INFO;
    if (levelStr == "WARN") return LogLevel::WARN;
    if (levelStr == "ERROR") return LogLevel::ERROR;
    if (levelStr == "DEBUG") return LogLevel::DEBUG;
    return LogLevel::INFO;  // Default if unknown
}

inline std::ostream& operator<<(std::ostream& os, LogLevel level) {
    return os << to_string(level);
}

#endif // LOG_LEVEL_HPP
