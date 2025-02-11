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
    bool inLevels = false;
    bool inColors = false;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        if (line == "[CONTEXTS]") {
            inContextSection = true;
            inLevels = false;
            inColors = false;
            continue;
        }
        if (line == "[LEVELS]") {
            inLevels = true;
            inContextSection = false;
            inColors = false;
            continue;
        }
        if (line == "[LEVEL_COLORS]" || line == "[CONTEXT_COLORS]") {
            inColors = true;
            inContextSection = false;
            inLevels = false;
            continue;
        }

        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);

            if (inContextSection) {
                LoggerSettings::getInstance().setContextLogLevel(key, value);
            } else if (inLevels) {
                LoggerSettings::getInstance().setLogLevel(key, value);
            } else if (inColors) {
                LoggerSettings::getInstance().setLogColor(key, value);  // ✅ Unified color storage
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
    file << "log_timestamps=true\n";
    file << "flush_mode=auto\n";
    file << "color_mode=level\n";


    file << "[LEVELS]\n";
    file << "INFO=ON\n";
    file << "WARN=ON\n";
    file << "ERROR=ON\n";
    file << "DEBUG=ON\n";
    file << "VERBOSE=OFF\n";

    file << "[LEVEL_COLORS]\n";
    file << "INFO=32\n";
    file << "WARN=33\n";
    file << "ERROR=31\n";
    file << "DEBUG=36\n";

    file << "[CONTEXT_COLORS]\n";
    file << "context_GENERAL=37\n";
    file << "context_NETWORK=34\n";
    file << "context_DATABASE=35\n";
    file << "context_UI=36\n";
    file << "context_AUDIO=33\n";
    file << "context_RENDERING=31\n";

    file << "[CONTEXTS]\n";
    file << "GENERAL=INFO\n";
    file << "NETWORK=DEBUG\n";
    file << "DATABASE=WARN\n";
    file << "UI=INFO\n";
    file << "AUDIO=DEBUG\n";
    file << "RENDERING=WARN\n";
    file << "UNKNOWN=WARN\n";
}