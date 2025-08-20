#include "myLogger/logger_config.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>

//------------------------------------------------------------------------------
// LoggerSettings Constructor
//------------------------------------------------------------------------------
LoggerSettings::LoggerSettings() {
    // Default all levels to "true" and "severity=0"
    config.levels.enabledArray.fill(true);
    config.levels.severitiesArray.fill(0);

    // Default to white for level/context colors
    config.colors.logColorArray.fill("#FFFFFFFF");
    config.colors.contextColorArray.fill("#FFFFFFFF");

    // Default context severity = 0
    config.contexts.contextSeverityArray.fill(0);
}

//------------------------------------------------------------------------------
// Print Config
//------------------------------------------------------------------------------
void LoggerConfig::printConfigState(const LoggerSettings& settings) {
    auto printBool = [](const std::string& name, bool value) {
        std::cerr << "[CONFIG] " << name << " = " << (value ? "true" : "false") << "\n";
    };

    auto printString = [](const std::string& name, const std::string& value) {
        std::cerr << "[CONFIG] " << name << " = " << value << "\n";
    };

    const auto& cfg = settings.config;

    std::cerr << "=========== Logger Configuration ===========" << std::endl;

    // ✅ General
    printString("log_directory",        cfg.general.logDirectory);
    printString("log_filename_format",  cfg.general.logFilenameFormat);
    printString("flush_mode",           cfg.general.flushMode);

    // ✅ Format
    printBool("log_timestamps",         cfg.format.enableTimestamps);
    printString("timestamp_format",     cfg.format.timestampFormat);
    printString("log_format",           cfg.format.logFormat);

    // ✅ Backends
    printBool("enable_console",         cfg.backends.enableConsole);
    printBool("enable_file",            cfg.backends.enableFile);

    // ✅ Display
    printBool("enable_colors",          cfg.display.enableColors);
    printBool("hide_level_tag",         cfg.display.hideLevelTag);
    printBool("hide_context_tag",       cfg.display.hideContextTag);

    // ✅ Levels
    std::cerr << "[CONFIG] Log Levels:\n";
    for (size_t i = 0; i < cfg.levels.levelNames.size(); i++) {
        printString("  Level " + cfg.levels.levelNames[i],
                    (cfg.levels.enabledArray[i] ? "ON" : "OFF"));
    }

    // ✅ Severities
    std::cerr << "[CONFIG] Severities:\n";
    for (size_t i = 0; i < cfg.levels.levelNames.size(); i++) {
        printString("  Severity " + cfg.levels.levelNames[i],
                    std::to_string(cfg.levels.severitiesArray[i]));
    }

    // ✅ Colors
    printString("color_mode",           cfg.colors.colorMode);

    std::cerr << "============================================" << std::endl;
}

//------------------------------------------------------------------------------
// loadOrGenerateConfig
//------------------------------------------------------------------------------
void LoggerConfig::loadOrGenerateConfig(const std::string& filepath, LoggerSettings& settings) {
    if (!std::filesystem::exists(filepath)) {
        generateDefaultConfig(filepath);
    }
    loadConfig(filepath, settings);

    // ✅ Example: If "BENCHMARK_MODE" env var is set, disable console
    if (std::getenv("BENCHMARK_MODE")) {
        settings.config.backends.enableConsole = false;
    }
}

//------------------------------------------------------------------------------
// loadConfig
//------------------------------------------------------------------------------
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

    } catch (const toml::parse_error& err) {
        std::cerr << "[LoggerConfig] Failed to parse TOML config: " << err.what() << "\n";
        generateDefaultConfig(filepath);
    }
}

//------------------------------------------------------------------------------
// loadGeneral
//------------------------------------------------------------------------------
void LoggerConfig::loadGeneral(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("general")) {
        auto& general = settings.config.general;
        general.logDirectory      = config["general"]["log_directory"]        .value_or(general.logDirectory);
        general.logFilenameFormat = config["general"]["log_filename_format"]  .value_or(general.logFilenameFormat);
        general.logRotationDays   = config["general"]["log_rotation_days"]    .value_or(general.logRotationDays);
        general.flushMode         = config["general"]["flush_mode"]           .value_or(general.flushMode);
    }
}

//------------------------------------------------------------------------------
// loadFormat
//------------------------------------------------------------------------------
void LoggerConfig::loadFormat(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("format")) {
        auto& format = settings.config.format;
        format.enableTimestamps  = config["format"]["log_timestamps"]    .value_or(format.enableTimestamps);
        format.timestampFormat   = config["format"]["timestamp_format"]  .value_or(format.timestampFormat);
        format.logFormat         = config["format"]["log_format"]        .value_or(format.logFormat);
    }
}

//------------------------------------------------------------------------------
// loadBackends
//------------------------------------------------------------------------------
void LoggerConfig::loadBackends(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("backends")) {
        auto& back = settings.config.backends;
        back.enableConsole = config["backends"]["enable_console"].value_or(back.enableConsole);
        back.enableFile    = config["backends"]["enable_file"]   .value_or(back.enableFile);
    }
}

//------------------------------------------------------------------------------
// loadDisplay
//------------------------------------------------------------------------------
void LoggerConfig::loadDisplay(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("display")) {
        auto& display = settings.config.display;
        display.enableColors   = config["display"]["enable_colors"]     .value_or(display.enableColors);
        display.hideLevelTag   = config["display"]["hide_level_tag"]    .value_or(display.hideLevelTag);
        display.hideContextTag = config["display"]["hide_context_tag"]  .value_or(display.hideContextTag);
    }
}

//------------------------------------------------------------------------------
// loadLevels
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// loadSeverities
//------------------------------------------------------------------------------
void LoggerConfig::loadSeverities(const toml::table& config, LoggerSettings& settings) {
    if (config.contains("severities")) {
        auto& levels = settings.config.levels;
        levels.severitiesArray.fill(0);

        for (size_t i = 0; i < levels.levelNames.size(); i++) {
            const std::string& lvlName = levels.levelNames[i];
            auto it = config["severities"].as_table()->find(lvlName);
            if (it != config["severities"].as_table()->end()) {
                levels.severitiesArray[i] = it->second.as_integer()->get();
            }
        }
    }
}

//------------------------------------------------------------------------------
// loadColors
//------------------------------------------------------------------------------
void LoggerConfig::loadColors(const toml::table& config, LoggerSettings& settings) {
    if (!config.contains("colors")) return;
    auto& colors = settings.config.colors;

    // ✅ Map ANSI color names to hex codes
    static const std::unordered_map<std::string, std::string> ANSI_COLOR_MAP = {
        {"BLACK", "#000000FF"},
        {"RED", "#FF0000FF"},
        {"GREEN", "#00FF00FF"},
        {"YELLOW", "#FFD700FF"},
        {"BLUE", "#0000FFFF"},
        {"MAGENTA", "#8A2BE2FF"},
        {"CYAN", "#87CEEBFF"},
        {"WHITE", "#FFFFFFFF"},
        {"LIGHT_GRAY", "#D3D3D3FF"},
        {"DARK_GRAY", "#A9A9A9FF"}
    };

    if (auto mode = config["colors"]["color_mode"].value<std::string>()) {
        colors.colorMode = *mode;
    }

    if (auto tableLvl = config["colors"]["level"].as_table()) {
        for (auto&& [key, node] : *tableLvl) {
            std::string value = node.value_or("#FFFFFF"); // Default to white

            // Convert ANSI color names to hex if applicable
            if (ANSI_COLOR_MAP.contains(value)) {
                value = ANSI_COLOR_MAP.at(value);
            }

            std::string fullKey = "level_" + std::string(key);
            colors.parsedLogColors[fullKey] = value;
        }
    }

    if (auto tableCtx = config["colors"]["context"].as_table()) {
        for (auto&& [key, node] : *tableCtx) {
            std::string value = node.value_or("#FFFFFF"); // Default to white

            // Convert ANSI color names to hex if applicable
            if (ANSI_COLOR_MAP.contains(value)) {
                value = ANSI_COLOR_MAP.at(value);
            }

            std::string fullKey = "context_" + std::string(key);
            colors.parsedLogColors[fullKey] = value;
        }
    }
}

//------------------------------------------------------------------------------
// loadContexts
//------------------------------------------------------------------------------
void LoggerConfig::loadContexts(const toml::table& config, LoggerSettings& settings) {
    if (!config.contains("contexts")) return;
    auto& ctxs = settings.config.contexts;
    ctxs.contextNames.clear();
    ctxs.contextIndexMap.clear();
    ctxs.contextSeverityArray.fill(0);

    int ctxIndex = 0;
    for (auto&& [key, node] : *config["contexts"].as_table()) {
        if (ctxIndex >= MAX_CONTEXTS) break;
        std::string contextName = std::string(key);
        ctxs.contextNames.push_back(contextName);
        ctxs.contextIndexMap[contextName] = ctxIndex;

        // For each context, find the level’s severity
        auto severityIt = settings.config.levels.levelIndexMap.find(node.as_string()->get());
        ctxs.contextSeverityArray[ctxIndex] = (severityIt != settings.config.levels.levelIndexMap.end())
                                                 ? settings.config.levels.severitiesArray[severityIt->second]
                                                 : 0;
        ++ctxIndex;
    }
}

//------------------------------------------------------------------------------
// generateDefaultConfig
//------------------------------------------------------------------------------
void LoggerConfig::generateDefaultConfig(const std::string& filepath) {
    std::filesystem::path configPath(filepath);
    if (!configPath.parent_path().empty()) {
        std::filesystem::create_directories(configPath.parent_path());
    }

    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "Could not create default config file: " << filepath << "\n";
        return;
    }

    // Minimal example (can be expanded as needed):
    file << R"(
[general]
log_directory = "logs/"
log_filename_format = "log_%Y-%m-%d_%H-%M-%S.txt"
log_rotation_days = 7
flush_mode = "instant"

[format]
log_timestamps = true
timestamp_format = "ISO"
log_format = "plain"

[backends]
enable_console = true
enable_file = true

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
VERBOSE   = "#87CEEBFF"
DEBUG     = "#87CEEBFF"
INFO      = "#32CD32FF"
WARN      = "#FFD700FF"
ERROR     = "#FF0000FF"
CRITICAL  = "#FF0000FF"

[colors.context]

[contexts]
)";

    std::cerr << "Default configuration created: " << filepath << "\n";
}
