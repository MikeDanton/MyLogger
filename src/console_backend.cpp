#include "console_backend.hpp"
#include <iostream>
#include <cstdio>

void ConsoleBackend::write(const LogMessage* log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    std::string colorCode = "\033[37m";

    if (settings.enableColors) {
        int colorValue = 37;

        int levelIndex   = settings.levelIndexMap.count(log->level)
                           ? settings.levelIndexMap.at(log->level)
                           : -1;
        int contextIndex = settings.contextIndexMap.count(log->context)
                           ? settings.contextIndexMap.at(log->context)
                           : -1;

        if (settings.colorMode == "context" && contextIndex >= 0) {
            colorValue = settings.contextColorArray[contextIndex];
        }
        // 🔹 fallback to level color if context missing
        else if (levelIndex >= 0) {
            colorValue = settings.logColorArray[levelIndex];
        }

        if (colorValue < 30 || colorValue > 37) {
            colorValue = 37;
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

void ConsoleBackend::flush() {
    fflush(stdout);
}
