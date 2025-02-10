#include "consoleBackend.hpp"
#include "loggerSettings.hpp"
#include <iostream>

void ConsoleBackend::write(const LogMessage& logMessage) {
    if (!LoggerSettings::getInstance().isConsoleEnabled()) return;

    std::string colorMode = LoggerSettings::getInstance().getGlobalSetting("color_mode");
    std::string color;

    if (colorMode == "context") {
        color = LoggerSettings::getInstance().getContextColor(logMessage.context);
    } else {
        color = LoggerSettings::getInstance().getColorSetting(to_string(logMessage.level));
    }

    std::string resetColor = "\033[0m";  // ✅ Reset terminal color after message

    std::cout << color  // ✅ Apply color
              << "[" << logMessage.level << "] "
              << "[" << logMessage.context << "] "
              << logMessage.message
              << resetColor
              << std::endl;
}

void ConsoleBackend::flush() {
    std::cout << std::flush;
}