#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include "logLevel.hpp"
#include <string>

struct LogMessage {
    LogLevel level;
    std::string context;  // ✅ Use `std::string` instead of `LogContext` enum
    std::string message;
};

#endif // LOG_MESSAGE_HPP
