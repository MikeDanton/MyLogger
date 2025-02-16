#include "console_backend.hpp"
#include "logger_config.hpp"
#include "format_module.hpp"
#include "color_module.hpp"
#include <iostream>

void ConsoleBackend::write(const LogMessage& log, const LoggerSettings& settings) {
    const std::string reset = "\033[0m";
    std::string colorCode = "\033[37m"; // Default white

    auto& display = settings.config.display; // ✅ Use the new display struct
    auto& colors = settings.config.colors;

    std::string key = (colors.colorMode == "context")
                      ? ("context_" + log.context)
                      : ("level_"   + log.level);

    if (colors.parsedLogColors.contains(key)) {
        colorCode = ColorModule::getColorCode(key, colors.parsedLogColors);
    }

    std::string formattedMessage;

    // ✅ Hide level or context if toggles are enabled
    if (display.hideLevelTag && display.hideContextTag) {
        formattedMessage = log.message;
    } else if (display.hideLevelTag) {
        formattedMessage = log.context + ": " + log.message;
    } else if (display.hideContextTag) {
        formattedMessage = "[" + log.level + "] " + log.message;
    } else {
        formattedMessage = "[" + log.level + "] " + log.context + ": " + log.message;
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