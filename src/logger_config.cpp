#include "logger_config.hpp"
#include "logger.hpp"
#include <toml++/toml.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>

LoggerSettings::LoggerSettings() {
    logLevelEnabledArray.fill(true);
    logLevelSeveritiesArray.fill(0);
    logColorArray.fill(37);
    contextColorArray.fill(37);
    contextSeverityArray.fill(0);
}

void LoggerConfig::precomputeColors(LoggerSettings& settings) {
    settings.logColorArray.fill(37);   // Default white
    settings.contextColorArray.fill(37); // Default white

    for (size_t i = 0; i < settings.logLevelNames.size(); i++) {
        auto it = settings.parsedLogColors.find("level_" + settings.logLevelNames[i]);
        if (it != settings.parsedLogColors.end()) {
            settings.logColorArray[i] = it->second;
        }
    }

    for (size_t i = 0; i < settings.contextNames.size(); i++) {
        auto it = settings.parsedLogColors.find("context_" + settings.contextNames[i]);
        if (it != settings.parsedLogColors.end()) {
            settings.contextColorArray[i] = it->second;
        }
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

        loadGeneralSettings(config, settings);
        loadToggles(config, settings);
        loadLevels(config, settings);
        loadSeverities(config, settings);
        loadColors(config, settings);
        loadContexts(config, settings);
        precomputeColors(settings);

        std::cerr << "[Logger] Config loaded successfully!\n";
    } catch (const toml::parse_error& err) {
        std::cerr << "[LoggerConfig] Failed to parse TOML config: " << err.what() << "\n";
        generateDefaultConfig(filepath);
    }
}

void LoggerConfig::loadGeneralSettings(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("general")) {
        settings.enableTimestamps = config["general"]["log_timestamps"].value_or(settings.enableTimestamps);
        settings.timestampFormat = config["general"]["timestamp_format"].value_or(settings.timestampFormat);
        settings.logFormat = config["general"]["log_format"].value_or(settings.logFormat);
    }
}

void LoggerConfig::loadToggles(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("toggles")) {
        settings.enableConsole = config["toggles"]["enable_console"].value_or(settings.enableConsole);
        settings.enableFile = config["toggles"]["enable_file"].value_or(settings.enableFile);
        settings.enableColors = config["toggles"]["enable_colors"].value_or(settings.enableColors);
        settings.hideLevelTag = config["toggles"]["hide_level_tag"].value_or(settings.hideLevelTag);
        settings.hideContextTag = config["toggles"]["hide_context_tag"].value_or(settings.hideContextTag);
    }
}

void LoggerConfig::loadLevels(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("levels")) {
        settings.logLevelNames.clear();
        settings.levelIndexMap.clear();
        settings.logLevelEnabledArray.fill(false);

        int index = 0;
        for (auto&& [key, node] : *config["levels"].as_table()) {
            if (index >= MAX_LEVELS) break;
            std::string levelName = std::string(key);
            settings.logLevelNames.push_back(levelName);
            settings.levelIndexMap[levelName] = index;
            settings.logLevelEnabledArray[index] = (node.as_string()->get() == "ON");

            std::cerr << "[DEBUG] Loaded Level: " << levelName << " = " << (settings.logLevelEnabledArray[index] ? "ON" : "OFF") << "\n";

            ++index;
        }
    }
}

void LoggerConfig::loadSeverities(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("severities")) {
        settings.logLevelSeveritiesArray.fill(0);
        for (size_t i = 0; i < settings.logLevelNames.size(); i++) {
            auto it = config["severities"].as_table()->find(settings.logLevelNames[i]);
            if (it != config["severities"].as_table()->end()) {
                settings.logLevelSeveritiesArray[i] = it->second.as_integer()->get();
                std::cerr << "[DEBUG] Loaded Severity: " << settings.logLevelNames[i] << " = " << settings.logLevelSeveritiesArray[i] << "\n";
            }
        }
    }
}

void LoggerConfig::loadColors(const toml::table& config, LoggerSettings& settings) {
    if (!config.contains("colors")) return;

    auto colorsSection = config["colors"].as_table();
    if (!colorsSection) return;

    if (auto cm = colorsSection->get("color_mode"); cm && cm->is_string()) {
        settings.colorMode = cm->as_string()->get();
    }

    settings.parsedLogColors.clear();

    for (const auto& category : {"level", "context"}) {
        if (auto categoryColors = colorsSection->get(category); categoryColors && categoryColors->is_table()) {
            for (auto&& [key, node] : *categoryColors->as_table()) {
                std::string fullKey = std::string(category) + "_" + std::string(key);
                settings.parsedLogColors[fullKey] = node.is_integer() ? node.as_integer()->get() : 37;
            }
        }
    }
}

void LoggerConfig::loadContexts(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("contexts")) {
        settings.contextNames.clear();
        settings.contextIndexMap.clear();
        settings.contextSeverityArray.fill(0);

        int ctxIndex = 0;
        for (auto&& [key, node] : *config["contexts"].as_table()) {
            if (ctxIndex >= MAX_CONTEXTS) break;
            std::string contextName = std::string(key);
            settings.contextNames.push_back(contextName);
            settings.contextIndexMap[contextName] = ctxIndex;

            auto severityIt = settings.levelIndexMap.find(node.as_string()->get());
            settings.contextSeverityArray[ctxIndex] = (severityIt != settings.levelIndexMap.end())
                                                          ? settings.logLevelSeveritiesArray[severityIt->second]
                                                          : 0;

            std::cerr << "[DEBUG] Loaded Context: " << contextName << " -> Severity: " << settings.contextSeverityArray[ctxIndex] << "\n";

            ++ctxIndex;
        }
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
[general]
log_directory = "logs/"
log_filename_format = "log_%Y-%m-%d_%H-%M-%S.txt"
log_rotation_days = 7
flush_mode = "auto"
log_timestamps = true
timestamp_format = "ISO" # Options: ISO, short, epoch
log_format = "plain"  # Options: plain, json, key-value

[toggles]
enable_console = true
enable_file = true
enable_colors = true
hide_level_tag = false
hide_context_tag = false

[levels]
VERBOSE = "OFF"
DEBUG   = "ON"
INFO    = "ON"
WARN    = "ON"
ERROR   = "ON"
CRITICAL = "ON"

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
