#include "loggerConfig.hpp"
#include "loggerSettings.hpp"
#include "stringUtils.hpp"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>

void LoggerConfig::loadConfig(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "[WARNING] Config file not found. Creating default config: " << filepath << "\n";
        generateDefaultConfig(filepath);
    }

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "[ERROR] Unable to read config file. Using defaults.\n";
        return;
    }

    std::string line;
    bool inContextSection = false;
    bool inColorSection = false;
    bool inContextColorSection = false;  // ✅ Added flag for context colors

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line == "[CONTEXTS]") {
            inContextSection = true;
            inColorSection = false;
            inContextColorSection = false;
            continue;
        } else if (line == "[COLORS]") {
            inColorSection = true;
            inContextSection = false;
            inContextColorSection = false;
            continue;
        } else if (line == "[CONTEXT_COLORS]") {  // ✅ Detect context colors
            inContextColorSection = true;
            inContextSection = false;
            inColorSection = false;
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);

            if (inContextSection) {
                LoggerSettings::getInstance().setContextLogLevel(key, value);
            } else if (inColorSection) {
                LoggerSettings::getInstance().setColorSetting(key, value);
            } else if (inContextColorSection) {  // ✅ Store context-based colors
                LoggerSettings::getInstance().setContextColor(key, value);
            } else {
                LoggerSettings::getInstance().setGlobalSetting(key, value);
            }
        }
    }
}

void LoggerConfig::generateDefaultConfig(const std::string& filepath) {
    if (std::filesystem::exists(filepath)) return;

    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "[ERROR] Could not create default config file: " << filepath << "\n";
        return;
    }

    std::cerr << "[INFO] Generating default config file: " << filepath << "\n";

    file << "log_directory=logs/\n";
    file << "log_filename_format=log_%Y-%m-%d_%H-%M-%S.txt\n";
    file << "log_rotation_days=7\n";
    file << "enable_console=true\n";
    file << "enable_file=true\n";
    file << "enable_colors=true\n";
    file << "flush_mode=auto\n";
    file << "log_level=INFO\n";
    file << "color_mode=level\n";  // ✅ Supports both "level" and "context"

    file << "[COLORS]\n";
    file << "info_color=32\n";   // Green
    file << "warn_color=33\n";   // Yellow
    file << "error_color=31\n";  // Red
    file << "debug_color=36\n";  // Cyan

    // ✅ Add default context-based colors
    file << "[CONTEXT_COLORS]\n";
    file << "context_GENERAL=37\n";    // White
    file << "context_NETWORK=34\n";    // Blue
    file << "context_DATABASE=35\n";   // Magenta
    file << "context_UI=36\n";         // Cyan
    file << "context_AUDIO=33\n";      // Yellow
    file << "context_RENDERING=31\n";  // Red

    file << "[CONTEXTS]\n";
    file << "GENERAL=INFO\n";
    file << "NETWORK=DEBUG\n";
    file << "DATABASE=WARN\n";
    file << "UI=INFO\n";
    file << "AUDIO=DEBUG\n";
    file << "RENDERING=WARN\n";
}