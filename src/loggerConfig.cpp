#include "loggerConfig.hpp"
#include "loggerSettings.hpp"
#include "stringUtils.hpp"
#include <toml++/toml.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>

void LoggerConfig::loadConfig(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "[WARNING] Config file not found. Creating default config: " << filepath << "\n";
        generateDefaultConfig(filepath);
    }

    try {
        toml::table config = toml::parse_file(filepath);
        std::cerr << "[DEBUG] Successfully parsed TOML config: " << filepath << "\n";

        // ✅ Read general settings
        if (auto general = config["general"].as_table()) {
            for (const auto& [key, value] : *general) {
                std::string keyStr = std::string(key);
                std::string valStr = value.is_string() ? value.as_string()->get() : "";
                LoggerSettings::getInstance().setGlobalSetting(keyStr, valStr);
                std::cerr << "[DEBUG] Global Setting: " << keyStr << " = " << valStr << "\n";
            }
        } else {
            std::cerr << "[ERROR] Missing [general] section in config!\n";
        }

        // ✅ Read toggles
        if (auto toggles = config["toggles"].as_table()) {
            for (const auto& [key, value] : *toggles) {
                std::string keyStr = std::string(key);
                std::string valStr = value.is_boolean() ? (value.as_boolean()->get() ? "true" : "false") : "false";
                LoggerSettings::getInstance().setGlobalSetting(keyStr, valStr);
                std::cerr << "[DEBUG] Toggle: " << keyStr << " = " << valStr << "\n";
            }
        } else {
            std::cerr << "[ERROR] Missing [toggles] section in config!\n";
        }

        // ✅ Read log levels
        if (auto levels = config["levels"].as_table()) {
            for (const auto& [key, value] : *levels) {
                std::string keyStr = std::string(key);
                std::string valStr = value.is_string() ? value.as_string()->get() : "OFF";
                LoggerSettings::getInstance().setLogLevel(keyStr, valStr);
                std::cerr << "[DEBUG] Log Level: " << keyStr << " = " << valStr << "\n";
            }
        } else {
            std::cerr << "[ERROR] Missing [levels] section in config!\n";
        }

        // ✅ Read colors (context + level)
        if (auto colors = config["colors"].as_table()) {
            if (auto level_colors_node = colors->get("level")) {  // Check existence first
                if (auto level_colors = level_colors_node->as_table()) {
                    for (const auto& [key, value] : *level_colors) {
                        std::string keyStr = std::string(key);
                        std::string valStr = std::to_string(value.is_integer() ? value.as_integer()->get() : 37);
                        LoggerSettings::getInstance().setLogColor("level_" + keyStr, valStr);
                        std::cerr << "[DEBUG] Level Color: " << keyStr << " -> " << valStr << "\n";
                    }
                } else {
                    std::cerr << "[ERROR] [colors.level] is not a table!\n";
                }
            } else {
                std::cerr << "[ERROR] Missing [colors.level] section in config!\n";
            }

            if (auto context_colors_node = colors->get("context")) {  // Check existence first
                if (auto context_colors = context_colors_node->as_table()) {
                    for (const auto& [key, value] : *context_colors) {
                        std::string keyStr = std::string(key);
                        std::string valStr = std::to_string(value.is_integer() ? value.as_integer()->get() : 37);
                        LoggerSettings::getInstance().setLogColor("context_" + keyStr, valStr);
                        std::cerr << "[DEBUG] Context Color: " << keyStr << " -> " << valStr << "\n";
                    }
                } else {
                    std::cerr << "[ERROR] [colors.context] is not a table!\n";
                }
            } else {
                std::cerr << "[ERROR] Missing [colors.context] section in config!\n";
            }
        } else {
            std::cerr << "[ERROR] Missing [colors] section in config!\n";
        }

        // ✅ Read context-based log levels
        if (auto contexts = config["contexts"].as_table()) {
            for (const auto& [key, value] : *contexts) {
                std::string keyStr = std::string(key);
                std::string valStr = value.is_string() ? value.as_string()->get() : "INFO";
                LoggerSettings::getInstance().setContextLogLevel(keyStr, valStr);
                std::cerr << "[DEBUG] Context Log Level: " << keyStr << " -> " << valStr << "\n";
            }
        } else {
            std::cerr << "[ERROR] Missing [contexts] section in config!\n";
        }

    } catch (const toml::parse_error& err) {
        std::cerr << "[ERROR] Failed to parse TOML config: " << err.what() << "\n";
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

    file << R"(
[general]
log_directory = "logs/"
log_filename_format = "log_%Y-%m-%d_%H-%M-%S.txt"
log_rotation_days = 7
flush_mode = "auto"
color_mode = "level"

[toggles]
enable_console = true
enable_file = true
enable_colors = true
log_timestamps = true

[levels]
INFO = "ON"
WARN = "ON"
ERROR = "ON"
DEBUG = "ON"
VERBOSE = "OFF"

[colors]
color_mode = "level"
[colors.level]
INFO = 32
WARN = 33
ERROR = 31
DEBUG = 36

[colors.context]
GENERAL = 37
NETWORK = 34
DATABASE = 35
UI = 36
AUDIO = 33
RENDERING = 31
UNKNOWN = 32

[contexts]
GENERAL = "INFO"
NETWORK = "DEBUG"
DATABASE = "WARN"
UI = "INFO"
AUDIO = "DEBUG"
RENDERING = "WARN"
UNKNOWN = "WARN"

[backends.console]
enabled = true
log_levels = ["INFO", "WARN", "ERROR"]
use_colors = true

[backends.file]
enabled = true
log_levels = ["DEBUG", "INFO", "ERROR"]
log_directory = "logs/"
rotation_days = 7
)";
}
