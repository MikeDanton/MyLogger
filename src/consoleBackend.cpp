#include "consoleBackend.hpp"
#include "loggerSettings.hpp"
#include <iostream>

void ConsoleBackend::write(const LogMessage& logMessage) {
    if (!LoggerSettings::getInstance().isConsoleEnabled()) return;

    std::string color = LoggerSettings::getInstance().getLogColor(logMessage.level, logMessage.context);
    std::string resetColor = "\033[0m";

    std::cout << color;

    if (LoggerSettings::getInstance().isTimestampEnabled()) {
        std::cout << "[" << logMessage.timestamp << "] ";
    }

    std::cout << "[" << logMessage.level << "] "
              << "[" << logMessage.context << "] "
              << logMessage.message
              << resetColor
              << std::endl;
}

void ConsoleBackend::flush() {
    std::cout << std::flush;
}
