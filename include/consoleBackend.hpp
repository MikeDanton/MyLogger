#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logBackend.hpp"
#include "logLevel.hpp"
#include <iostream>

// Console Logging Backend
class ConsoleBackend : public LogBackend {
public:
    void write(const std::string& message) override;  // Overrides base class
    void write(const std::string& message, LogLevel level);  // Overloaded version for colored text
};

#endif // CONSOLE_BACKEND_HPP
