#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logBackend.hpp"
#include <iostream>

// Console Logging Backend
class ConsoleBackend : public LogBackend {
public:
    void write(const std::string& message) override;
};

#endif // CONSOLE_BACKEND_HPP
