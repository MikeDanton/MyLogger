#ifndef LOG_BACKEND_HPP
#define LOG_BACKEND_HPP

#include "logMessage.hpp"

// Base interface for logging backends
class LogBackend {
public:
    virtual ~LogBackend() = default;

    virtual void write(const LogMessage& message) = 0;
    virtual void flush() {}
};

#endif // LOG_BACKEND_HPP
