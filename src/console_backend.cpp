#include "console_backend.h"
#include "logger.h"
#include <cstdio>
#include <iostream>

void console_write(const LogMessage* log) {
    Logger& logger = Logger::getInstance();
    const LoggerSettings& settings = logger.getSettings();

    const char* reset = "\033[0m";
    std::string colorCode;

    if (settings.enableColors) {
        // Decide if we color by level or context:
        std::string colorKey;
        if (settings.colorMode == "context") {
            colorKey = "context_" + std::string(log->context);
        } else {
            // default to "level" mode
            colorKey = "level_" + std::string(log->level);
        }

        auto colorIt = settings.logColors.find(colorKey);
        if (colorIt != settings.logColors.end()) {
            colorCode = "\033[" + std::to_string(colorIt->second) + "m";
        } else {
            colorCode = "\033[37m"; // white
        }
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
