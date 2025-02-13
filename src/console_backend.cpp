#include "console_backend.hpp"
#include <iostream>
#include <cstdio>

void ConsoleBackend::write(const LogMessage* log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    std::string colorCode = "\033[37m";

    if (settings.enableColors) {
        int colorValue = 37;

        auto levelIt = settings.levelIndexMap.find(log->level);
        auto contextIt = settings.contextIndexMap.find(log->context);

        int levelIndex = (levelIt != settings.levelIndexMap.end()) ? levelIt->second : -1;
        int contextIndex = (contextIt != settings.contextIndexMap.end()) ? contextIt->second : -1;

        if (settings.colorMode == "context" && contextIndex >= 0) {
            colorValue = settings.contextColorArray[contextIndex];
        } else if (levelIndex >= 0) {
            colorValue = settings.logColorArray[levelIndex];
        }

        if (colorValue < 30 || colorValue > 37) {
            colorValue = 37;
        }

        colorCode = "\033[" + std::to_string(colorValue) + "m";
    }

    std::cout << colorCode << "[" << log->level << "] " << log->context << ": " << log->message << reset << "\n";
}

void ConsoleBackend::flush() {
    static std::atomic<int> flushCounter = 0;
    if (++flushCounter % 100 == 0) {
        std::cout.flush();
    }
}