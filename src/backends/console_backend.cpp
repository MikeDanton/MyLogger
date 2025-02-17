#include "console_backend.hpp"
#include <iostream>

ConsoleBackend::ConsoleBackend(LoggerModules& modules)
    : colorModule(modules.getColorModule()) {  // ✅ Initialize reference in initializer list
    std::cerr << "[ConsoleBackend] Using color module from LoggerModules.\n";
}

void ConsoleBackend::setup(const LoggerSettings& settings) {
    std::cerr << "[ConsoleBackend] Loading colors...\n";
    colorModule.setColorMap(settings.config.colors.parsedLogColors);
}

void ConsoleBackend::write(const LogMessage& log, const LoggerSettings& settings) {
    const char* reset = "\033[0m";
    auto& colors = settings.config.colors;

    std::string key = (colors.colorMode == "context")
                        ? "context_" + std::string(log.context)
                        : "level_" + std::string(log.level);

    std::string hexColor = colorModule.getColor(key);

    // ✅ Convert hex to ANSI foreground color
    int ansiColor = 37; // Default: White
    if (hexColor == "#FF0000FF") ansiColor = 31;  // Red
    else if (hexColor == "#00FF00FF") ansiColor = 32;  // Green
    else if (hexColor == "#FFD700FF") ansiColor = 33;  // Yellow
    else if (hexColor == "#0000FFFF") ansiColor = 34;  // Blue
    else if (hexColor == "#8A2BE2FF") ansiColor = 35;  // Magenta
    else if (hexColor == "#87CEEBFF") ansiColor = 36;  // Cyan

    std::string ansiCode = "\033[" + std::to_string(ansiColor) + "m";

    std::cout << ansiCode << log.message << reset << "\n";
}

void ConsoleBackend::flush() {
    std::cout.flush();
}

void ConsoleBackend::shutdown() {
    flush();
}