#include "logger_config.hpp"
#include "logger.h"
#include <toml++/toml.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cassert>

void LoggerConfig::loadConfig(const std::string& filepath, LoggerSettings& settings) {
    assert(!filepath.empty());

    if (!std::filesystem::exists(filepath)) {
        generateDefaultConfig(filepath);
    }

    try {
        toml::table config = toml::parse_file(filepath);

        // ------ general section ------
        if (auto general = config["general"].as_table()) {
            if (auto val = general->get_as<bool>("log_timestamps")) {
                settings.enableTimestamps = val->get();
            } else {
                settings.enableTimestamps = true;
            }
        }

        // ------ toggles section ------
        if (auto toggles = config["toggles"].as_table()) {
            if (auto val = toggles->get_as<bool>("enable_console")) {
                settings.enableConsole = val->get();
            } else {
                settings.enableConsole = true;
            }
            if (auto val = toggles->get_as<bool>("enable_file")) {
                settings.enableFile = val->get();
            } else {
                settings.enableFile = true;
            }
            if (auto val = toggles->get_as<bool>("enable_colors")) {
                settings.enableColors = val->get();
            } else {
                settings.enableColors = false;
            }
        }

        // ------ levels (ON/OFF) section ------
        if (auto levels = config["levels"].as_table()) {
            for (auto&& [key, node] : *levels) {
                std::string levelName = std::string(key);
                bool enabled = (node.is_string() && node.as_string()->get() == "ON");
                settings.logLevelEnabled[levelName] = enabled;
            }
        }

        // ------ severities section (NEW) ------
        // e.g. [severities] DEBUG=2, INFO=3, etc.
        if (auto severities = config["severities"].as_table()) {
            for (auto&& [key, node] : *severities) {
                // key = "DEBUG", "ERROR", etc.
                if (node.is_integer()) {
                    int severityNum = static_cast<int>(node.as_integer()->get());
                    settings.logLevelSeverities[std::string(key)] = severityNum;
                }
            }
        }

        // ------ colors section ------
        if (auto colors = config["colors"].as_table()) {
            // color_mode
            if (auto ptr = colors->get_as<std::string>("color_mode")) {
                settings.colorMode = ptr->get();
            } else {
                settings.colorMode = "level";
            }

            // level colors
            if (auto levelColors = colors->get("level"); levelColors && levelColors->is_table()) {
                for (auto&& [key, node] : *levelColors->as_table()) {
                    std::string levelKey = std::string(key);
                    int colorValue = node.is_integer() ? static_cast<int>(node.as_integer()->get()) : 37;
                    settings.logColors["level_" + levelKey] = colorValue;
                }
            }

            // context colors
            if (auto contextColors = colors->get("context"); contextColors && contextColors->is_table()) {
                for (auto&& [key, node] : *contextColors->as_table()) {
                    std::string ctxKey = std::string(key);
                    int colorValue = node.is_integer() ? static_cast<int>(node.as_integer()->get()) : 37;
                    settings.logColors["context_" + ctxKey] = colorValue;
                }
            }
        } else {
            // default color mode
            settings.colorMode = "level";
        }

        // ------ contexts section ------
        if (auto contexts = config["contexts"].as_table()) {
            for (auto&& [key, node] : *contexts) {
                std::string ctxName = std::string(key);
                std::string lvl = node.is_string() ? node.as_string()->get() : "INFO";
                settings.contextLevels[ctxName] = lvl;
            }
        }

    } catch (const toml::parse_error& err) {
        std::cerr << "[ERROR] Failed to parse TOML config: " << err.what() << "\n";
    }
}

void LoggerConfig::generateDefaultConfig(const std::string& filepath) {
    std::filesystem::path configPath(filepath);
    std::filesystem::create_directories(configPath.parent_path());
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "Could not create default config file: " << filepath << "\n";
        return;
    }

    file << R"(
# Generated example logger.conf
[general]
log_directory = "logs/"
log_filename_format = "log_%Y-%m-%d_%H-%M-%S.txt"
log_rotation_days = 7
flush_mode = "auto"
log_timestamps = true

[toggles]
enable_console = true
enable_file = true
enable_colors = true

# This section determines if a level is globally ON/OFF
[levels]
VERBOSE = "OFF"
DEBUG   = "ON"
INFO    = "ON"
WARN    = "ON"
ERROR   = "ON"
CRITICAL = "ON"

# The higher (or lower) the number, the more (or less) severe it is.
[severities]
VERBOSE   = 1
DEBUG     = 2
INFO      = 3
WARN      = 4
ERROR     = 5
CRITICAL  = 6

[colors]
color_mode = "level"
[colors.level]
VERBOSE = 36
DEBUG   = 36
INFO    = 32
WARN    = 33
ERROR   = 31
CRITICAL= 31

[colors.context]
GENERAL    = 37
NETWORK    = 34
DATABASE   = 35
UI         = 36
AUDIO      = 33
RENDERING  = 31
UNKNOWN    = 32

[contexts]
GENERAL    = "INFO"
NETWORK    = "DEBUG"
DATABASE   = "WARN"
UI         = "INFO"
AUDIO      = "DEBUG"
RENDERING  = "WARN"
)";
    std::cerr << "Default configuration created: " << filepath << "\n";
}
