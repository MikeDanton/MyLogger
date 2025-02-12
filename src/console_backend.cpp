#include "console_backend.h"
#include "logger.hpp"
#include <cstdio>
#include <iostream>

void console_write(const LogMessage* log) {
    Logger& logger = Logger::getInstance();
    const LoggerSettings& settings = logger.getSettings();

    const char* reset = "\033[0m";
    std::string colorCode = "\033[37m";  // Default white

    if (settings.enableColors) {
        int colorValue = 37; // Default white
        int levelIndex = logger.getLevelIndex(log->level);
        int contextIndex = logger.getContextIndex(log->context);

        if (settings.colorMode == "context" && contextIndex >= 0) {
            colorValue = settings.contextColorArray[contextIndex];
        } else if (levelIndex >= 0) {
            colorValue = settings.logColorArray[levelIndex];
        }

        colorCode = "\033[" + std::to_string(colorValue) + "m";
    }

    printf("%s[%s] %s: %s%s\n",
           colorCode.c_str(),
           log->level,
           log->context,
           log->message,
           reset);
}

void console_flush() {
    fflush(stdout);
}

// ✅ Define ConsoleBackend
LogBackend ConsoleBackend = { console_write, console_flush };
