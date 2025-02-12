#include "logger_config.hpp"
#include "logger.hpp"
#include <toml++/toml.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <cassert>

void LoggerConfig::precomputeColors(LoggerSettings& settings) {
    settings.logColorArray.fill(37);   // Default white
    settings.contextColorArray.fill(37); // Default white

    for (size_t i = 0; i < settings.logLevelNames.size(); i++) {
        std::string levelKey = "level_" + settings.logLevelNames[i];
        auto it = settings.parsedLogColors.find(levelKey);
        settings.logColorArray[i] = (it != settings.parsedLogColors.end()) ? it->second : 37;
    }

    for (size_t i = 0; i < settings.contextNames.size(); i++) {
        std::string contextKey = "context_" + settings.contextNames[i];
        auto it = settings.parsedLogColors.find(contextKey);
        settings.contextColorArray[i] = (it != settings.parsedLogColors.end()) ? it->second : 37;
    }
}

void LoggerConfig::loadOrGenerateConfig(const std::string& filepath, LoggerSettings& settings) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "[LoggerConfig] Config file missing. Generating default: " << filepath << "\n";
        generateDefaultConfig(filepath);
    }

    loadConfig(filepath, settings);
}

void LoggerConfig::loadConfig(const std::string& filepath, LoggerSettings& settings) {
    try {
        toml::table config = toml::parse_file(filepath);

        // ✅ General settings
        settings.enableTimestamps = config["general"]["log_timestamps"].value_or(true);
        settings.enableConsole = config["toggles"]["enable_console"].value_or(true);
        settings.enableFile = config["toggles"]["enable_file"].value_or(true);
        settings.enableColors = config["toggles"]["enable_colors"].value_or(false);
        settings.colorMode = config["colors"]["color_mode"].value_or("level");

        // ✅ Load log levels (precompute array)
        settings.logLevelNames.clear();
        settings.levelIndexMap.clear();
        settings.logLevelEnabledArray.fill(false);

        int index = 0;
        for (auto&& [key, node] : *config["levels"].as_table()) {
            if (index >= MAX_LEVELS) break;
            std::string levelName = std::string(key); // Convert toml::key to std::string
            settings.logLevelNames.push_back(levelName);
            settings.levelIndexMap[levelName] = index;
            settings.logLevelEnabledArray[index] = (node.as_string()->get() == "ON");
            ++index;
        }

        // ✅ Load severities (precompute array)
        settings.logLevelSeveritiesArray.fill(0);
        for (size_t i = 0; i < settings.logLevelNames.size(); i++) {
            auto it = config["severities"].as_table()->find(settings.logLevelNames[i]);
            settings.logLevelSeveritiesArray[i] = (it != config["severities"].as_table()->end()) ? it->second.as_integer()->get() : 0;
        }

        // ✅ Load colors into temporary storage (`parsedLogColors`)
        settings.parsedLogColors.clear();
        if (auto colors = config["colors"].as_table()) {
            if (auto levelColors = colors->get("level"); levelColors && levelColors->is_table()) {
                for (auto&& [key, node] : *levelColors->as_table()) {
                    std::string levelKey = "level_" + std::string(key);
                    settings.parsedLogColors[levelKey] = node.is_integer() ? static_cast<int>(node.as_integer()->get()) : 37;
                }
            }
            if (auto contextColors = colors->get("context"); contextColors && contextColors->is_table()) {
                for (auto&& [key, node] : *contextColors->as_table()) {
                    std::string contextKey = "context_" + std::string(key);
                    settings.parsedLogColors[contextKey] = node.is_integer() ? static_cast<int>(node.as_integer()->get()) : 37;
                }
            }
        }

        // ✅ Load contexts (precompute array)
        settings.contextNames.clear();
        settings.contextIndexMap.clear();
        settings.contextSeverityArray.fill(0);

        int ctxIndex = 0;
        for (auto&& [key, node] : *config["contexts"].as_table()) {
            if (ctxIndex >= MAX_CONTEXTS) break;
            std::string contextName = std::string(key);
            settings.contextNames.push_back(contextName);
            settings.contextIndexMap[contextName] = ctxIndex;
            settings.contextSeverityArray[ctxIndex] = Logger::getInstance().getSeverity(node.as_string()->get());
            ++ctxIndex;
        }

        // ✅ Apply precomputed colors
        precomputeColors(settings);

    } catch (const toml::parse_error& err) {
        std::cerr << "[LoggerConfig] Failed to parse TOML config: " << err.what() << "\n";
        generateDefaultConfig(filepath);
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
