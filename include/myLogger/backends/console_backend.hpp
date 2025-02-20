#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "myLogger/logger_core.hpp"
#include "myLogger/logger_config.hpp"
#include <iostream>

class ConsoleBackend {
public:
    ConsoleBackend() = default;

    void setup(const LoggerSettings& settings);
    void write(const LogMessage& log, const LoggerSettings& settings);
    void flush();
    void shutdown();

private:
    static int hexToAnsiColor(const std::string& hexColor);

    // ✅ Missing member variables
    std::string colorMode = "level";   // Default mode
    bool hideLevelTag = false;         // Default: show level tags
    bool hideContextTag = false;       // Default: show context tags
};

#endif // CONSOLE_BACKEND_HPP
