#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logBackend.hpp"
#include "logMessage.hpp"
#include "loggerConfig.hpp"
#include <iostream>

// Console Logging Backend
class ConsoleBackend : public LogBackend {
public:
    void write(const LogMessage& logMessage) override;
};

#endif // CONSOLE_BACKEND_HPP
