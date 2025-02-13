#include "console_backend.hpp"
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <ctime>

void ConsoleBackend::write(const LogMessage* log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    std::string colorCode = "\033[37m";

    // Determine color based on context or level
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

        // Validate color range
        if (colorValue < 30 || colorValue > 37) {
            colorValue = 37;
        }

        colorCode = "\033[" + std::to_string(colorValue) + "m";
    }

    std::ostringstream logEntry;

    // Timestamp Handling
    if (settings.enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&now);

        if (settings.timestampFormat == "ISO") {
            logEntry << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S") << " ";
        } else if (settings.timestampFormat == "short") {
            logEntry << std::put_time(timeinfo, "%H:%M:%S") << " ";
        } else if (settings.timestampFormat == "epoch") {
            logEntry << now << " ";
        }
    }

    // Level & Context Handling
    if (!settings.hideLevelTag) {
        logEntry << "[" << log->level << "] ";
    }
    if (!settings.hideContextTag) {
        logEntry << log->context << ": ";
    }

    logEntry << log->message;

    // Print log with color
    std::cout << colorCode << logEntry.str() << reset << "\n";
}

void ConsoleBackend::flush() {
    static std::atomic<int> flushCounter = 0;
    if (++flushCounter % 100 == 0) {
        std::cout.flush();
    }
}