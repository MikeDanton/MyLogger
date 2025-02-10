#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include <string>

struct LogMessage {
    std::string level;
    std::string context;
    std::string message;
};

#endif // LOG_MESSAGE_HPP
