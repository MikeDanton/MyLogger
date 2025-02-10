#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include "logLevel.hpp"
#include "logContext.hpp"
#include <string>

struct LogMessage {
    LogLevel level;
    LogContext context;
    std::string message;

    LogMessage(LogLevel lvl, LogContext ctx, const std::string& msg)
        : level(lvl), context(ctx), message(msg) {}
};

#endif // LOG_MESSAGE_HPP
