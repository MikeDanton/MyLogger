#ifndef LOG_BACKEND_HPP
#define LOG_BACKEND_HPP

#include <string>

// Base interface for logging backends
class LogBackend {
public:
    virtual ~LogBackend() = default;
    virtual void write(const std::string& message) = 0;
};

#endif // LOG_BACKEND_HPP
