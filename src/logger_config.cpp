#include "logger_config.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

LoggerSettings::LoggerSettings() {
    config.levels.enabledArray.fill(true);
    config.levels.severitiesArray.fill(0);
    config.colors.logColorArray.fill(37);
    config.colors.contextColorArray.fill(37);
    config.contexts.contextSeverityArray.fill(0);
}

void LoggerConfig::printConfigState(const LoggerSettings& settings) {
    auto printBool = [](const std::string& name, bool value) {
        std::cerr << "[CONFIG] " << name << " = " << (value ? "true" : "false") << "\n";
    };

    auto printString = [](const std::string& name, const std::string& value) {
        std::cerr << "[CONFIG] " << name << " = " << value << "\n";
    };

    auto& cfg = settings.config;

    std::cerr << "=========== Logger Configuration ===========" << std::endl;

    // ✅ General Settings
    printString("log_directory", cfg.general.logDirectory);
    printString("log_filename_format", cfg.general.logFilenameFormat);
    printString("flush_mode", cfg.general.flushMode);

    // ✅ Format Settings
    printBool("log_timestamps", cfg.format.enableTimestamps);
    printString("timestamp_format", cfg.format.timestampFormat);
    printString("log_format", cfg.format.logFormat);

    // ✅ Backend Settings
    printBool("enable_console", cfg.backends.enableConsole);
    printBool("enable_file", cfg.backends.enableFile);

    // ✅ Display Settings
    printBool("enable_colors", cfg.display.enableColors);
    printBool("hide_level_tag", cfg.display.hideLevelTag);
    printBool("hide_context_tag", cfg.display.hideContextTag);

    // ✅ Log Levels
    std::cerr << "[CONFIG] Log Levels:\n";
    for (size_t i = 0; i < cfg.levels.levelNames.size(); i++) {
        printString("  Level " + cfg.levels.levelNames[i], cfg.levels.enabledArray[i] ? "ON" : "OFF");
    }

    // ✅ Severities
    std::cerr << "[CONFIG] Severities:\n";
    for (size_t i = 0; i < cfg.levels.levelNames.size(); i++) {
        printString("  Severity " + cfg.levels.levelNames[i], std::to_string(cfg.levels.severitiesArray[i]));
    }

    // ✅ Colors
    printString("color_mode", cfg.colors.colorMode);

    std::cerr << "============================================" << std::endl;
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

        loadGeneral(config, settings);
        loadFormat(config, settings);
        loadBackends(config, settings);
        loadDisplay(config, settings);
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

void LoggerConfig::precomputeColors(LoggerSettings& settings) {
    auto& colors = settings.config.colors;

    // Reset all colors to default white (37)
    colors.logColorArray.fill(37);
    colors.contextColorArray.fill(37);

    // Apply log level colors
    for (size_t i = 0; i < settings.config.levels.levelNames.size(); i++) {
        std::string key = "level_" + settings.config.levels.levelNames[i];
        if (colors.parsedLogColors.contains(key)) {
            colors.logColorArray[i] = colors.parsedLogColors[key];
        }
    }

    // Apply context colors
    for (size_t i = 0; i < settings.config.contexts.contextNames.size(); i++) {
        std::string key = "context_" + settings.config.contexts.contextNames[i];
        if (colors.parsedLogColors.contains(key)) {
            colors.contextColorArray[i] = colors.parsedLogColors[key];
        }
    }
}

// ✅ Load General Settings
void LoggerConfig::loadGeneral(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("general")) {
        auto& general = settings.config.general;
        general.logDirectory = config["general"]["log_directory"].value_or(general.logDirectory);
        general.logFilenameFormat = config["general"]["log_filename_format"].value_or(general.logFilenameFormat);
        general.logRotationDays = config["general"]["log_rotation_days"].value_or(general.logRotationDays);
        general.flushMode = config["general"]["flush_mode"].value_or(general.flushMode);
    }
}

// ✅ Load Format Settings
void LoggerConfig::loadFormat(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("format")) {
        auto& format = settings.config.format;
        format.enableTimestamps = config["format"]["enable_timestamps"].value_or(format.enableTimestamps);
        format.timestampFormat = config["format"]["timestamp_format"].value_or(format.timestampFormat);
        format.logFormat = config["format"]["log_format"].value_or(format.logFormat);
    }
}

// ✅ Load Backend Settings
void LoggerConfig::loadBackends(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("backends")) {
        auto& backends = settings.config.backends;
        backends.enableConsole = config["backends"]["enable_console"].value_or(backends.enableConsole);
        backends.enableFile = config["backends"]["enable_file"].value_or(backends.enableFile);
    }
}

// ✅ Load Display Settings
void LoggerConfig::loadDisplay(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("display")) {
        auto& display = settings.config.display;
        display.enableColors = config["display"]["enable_colors"].value_or(display.enableColors);
        display.hideLevelTag = config["display"]["hide_level_tag"].value_or(display.hideLevelTag);
        display.hideContextTag = config["display"]["hide_context_tag"].value_or(display.hideContextTag);
    }
}

// ✅ Load Levels
void LoggerConfig::loadLevels(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("levels")) {
        auto& levels = settings.config.levels;
        levels.levelNames.clear();
        levels.levelIndexMap.clear();
        levels.enabledArray.fill(false);

        int index = 0;
        for (auto&& [key, node] : *config["levels"].as_table()) {
            if (index >= MAX_LEVELS) break;
            std::string levelName = std::string(key);
            levels.levelNames.push_back(levelName);
            levels.levelIndexMap[levelName] = index;
            levels.enabledArray[index] = (node.as_string()->get() == "ON");

            ++index;
        }
    }
}

// ✅ Load Severities
void LoggerConfig::loadSeverities(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("severities")) {
        auto& levels = settings.config.levels;
        levels.severitiesArray.fill(0);
        for (size_t i = 0; i < levels.levelNames.size(); i++) {
            auto it = config["severities"].as_table()->find(levels.levelNames[i]);
            if (it != config["severities"].as_table()->end()) {
                levels.severitiesArray[i] = it->second.as_integer()->get();
            }
        }
    }
}

// ✅ Load Colors
void LoggerConfig::loadColors(const toml::table& config, LoggerSettings& settings) {
    if (!config.contains("colors")) return;
    auto& colors = settings.config.colors;

    if (auto cm = config["colors"]["color_mode"].value<std::string>()) {
        colors.colorMode = *cm;
    }

    for (const auto& category : {"level", "context"}) {
        if (auto categoryColors = config["colors"][category].as_table()) {
            for (auto&& [key, node] : *categoryColors) {
                std::string fullKey = std::string(category) + "_" + std::string(key);
                colors.parsedLogColors[fullKey] = node.as_integer()->get();
            }
        }
    }
}

// ✅ Load Contexts
void LoggerConfig::loadContexts(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("contexts")) {
        auto& contexts = settings.config.contexts;
        contexts.contextNames.clear();
        contexts.contextIndexMap.clear();
        contexts.contextSeverityArray.fill(0);

        int ctxIndex = 0;
        for (auto&& [key, node] : *config["contexts"].as_table()) {
            if (ctxIndex >= MAX_CONTEXTS) break;
            std::string contextName = std::string(key);
            contexts.contextNames.push_back(contextName);
            contexts.contextIndexMap[contextName] = ctxIndex;

            auto severityIt = settings.config.levels.levelIndexMap.find(node.as_string()->get());
            contexts.contextSeverityArray[ctxIndex] = (severityIt != settings.config.levels.levelIndexMap.end())
                                                          ? settings.config.levels.severitiesArray[severityIt->second]
                                                          : 0;
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

# Formatting Options
[format]
log_timestamps = true
timestamp_format = "ISO"  # Options: ISO, short, epoch
log_format = "plain"       # Options: plain, json, key-value

# Needs reboot to work
[backends]
enable_console = true
enable_file = true

# Display Toggles (Affects Output Behavior)
[display]
enable_colors = true
hide_level_tag = false
hide_context_tag = false

[levels]
VERBOSE   = "OFF"
DEBUG     = "ON"
INFO      = "ON"
WARN      = "ON"
ERROR     = "ON"
CRITICAL  = "ON"

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
VERBOSE   = 36
DEBUG     = 36
INFO      = 32
WARN      = 33
ERROR     = 31
CRITICAL  = 31

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
