#include "console_backend.hpp"
#include "logger_config.hpp"
#include "format_module.hpp"
#include "color_module.hpp"
#include <iostream>
#include <cstdio>  // ✅ Required for snprintf()

void ConsoleBackend::write(const LogMessage& log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    const char* defaultColor = "\033[37m"; // Default white
    const char* colorCode = defaultColor;

    auto& display = settings.config.display;
    auto& colors = settings.config.colors;

    char key[64];  // ✅ Preallocated buffer for color key
    if (std::strcmp(colors.colorMode.c_str(), "context") == 0) {
        std::snprintf(key, sizeof(key), "context_%s", log.context);
    } else {
        std::snprintf(key, sizeof(key), "level_%s", log.level);
    }

    if (colors.parsedLogColors.contains(std::string(key))) {
        colorCode = ColorModule::getColorCode(std::string(key), colors.parsedLogColors).c_str();
    }

    char formattedMessage[512];  // ✅ Preallocated buffer for log message

    // ✅ Hide level or context if toggles are enabled
    if (display.hideLevelTag && display.hideContextTag) {
        std::snprintf(formattedMessage, sizeof(formattedMessage), "%s", log.message);
    } else if (display.hideLevelTag) {
        std::snprintf(formattedMessage, sizeof(formattedMessage), "%s: %s", log.context, log.message);
    } else if (display.hideContextTag) {
        std::snprintf(formattedMessage, sizeof(formattedMessage), "[%s] %s", log.level, log.message);
    } else {
        std::snprintf(formattedMessage, sizeof(formattedMessage), "[%s] %s: %s", log.level, log.context, log.message);
    }

    std::cout << (display.enableColors ? colorCode : "")
              << formattedMessage
              << (display.enableColors ? reset : "")
              << "\n";
}

void ConsoleBackend::flush() {
    std::cout.flush();
}

void ConsoleBackend::shutdown() {
    flush();
}
