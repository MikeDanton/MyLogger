#include "consoleBackend.hpp"
#include "loggerConfig.hpp"
#include <iostream>

void ConsoleBackend::write(const std::string& message) {
    std::cout << message;
}

void ConsoleBackend::write(const std::string& message, LogLevel level) {
    if (LoggerConfig::isColorEnabled()) {
        std::cout << LoggerConfig::getColorForLevel(level) << message << "\033[0m";  // Apply color and reset
    } else {
        std::cout << message;
    }
}
