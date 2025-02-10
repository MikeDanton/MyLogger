#include "loggerConfig.hpp"
#include "logLevel.hpp"
#include "logContext.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>

// Define default config as a static constant string
const std::string LoggerConfig::defaultConfig = R"(
# Logger Configuration File
log_directory=logs/
log_filename_format=log_%Y-%m-%d_%H-%M-%S.txt
log_rotation_days=7
enable_console=true
enable_file=true
enable_colors=true
flush_mode=auto
log_level=INFO
color_priority=level

enabled_contexts=GENERAL,NETWORK,UI

info_color=32
warn_color=33
error_color=31
debug_color=36

context_general_color=37
context_network_color=34
context_database_color=35
context_ui_color=36
context_audio_color=33
context_rendering_color=31
)";

std::unordered_map<std::string, std::string> LoggerConfig::config;
std::unordered_set<LogContext> LoggerConfig::enabledContexts;

// ✅ Auto-generate config file if missing
void LoggerConfig::generateDefaultConfig(const std::string& filepath) {
    if (std::filesystem::exists(filepath)) {
        return;  // ✅ Config already exists, no need to generate it
    }

    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "[ERROR] Could not create default config file: " << filepath << "\n";
        return;
    }

    std::cerr << "[INFO] Generating default config file: " << filepath << "\n";
    file << defaultConfig;
}

// ✅ Save config (overwrites with defaults)
void LoggerConfig::saveConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "[ERROR] Could not save config file.\n";
        return;
    }

    file << defaultConfig;
}

void LoggerConfig::loadConfig(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "[WARNING] Config file not found. Creating default config: " << filepath << "\n";
        saveConfig(filepath);
    }

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "[ERROR] Unable to read config file. Using defaults.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // Ignore empty lines and comments
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            key = trim(key);
            value = trim(value);
            config[key] = value;
        }
    }

    // ✅ Load enabled contexts
    if (config.contains("enabled_contexts")) {
        std::istringstream contextStream(config["enabled_contexts"]);
        std::string token;
        while (std::getline(contextStream, token, ',')) {
            LogContext parsedContext = logContextFromString(trim(token));
            enabledContexts.insert(parsedContext);
        }
    }

    // ✅ Ensure color priority is set
    if (!config.contains("color_priority")) {
        config["color_priority"] = "level"; // Default to level-based coloring
    }
}

std::string LoggerConfig::getColorPriority() {
    return config.contains("color_priority") ? config["color_priority"] : "level";
}

std::string LoggerConfig::getColorForLog(LogLevel level, LogContext context) {
    if (!isColorEnabled()) return "";

    std::string colorPriority = getColorPriority();
    std::string colorKey;

    // ✅ **Prioritize Context Colors if Set to "context"**
    if (colorPriority == "context") {
        std::string contextKey = "context_" + to_string(context) + "_color";

        // Normalize key to lowercase
        std::transform(contextKey.begin(), contextKey.end(), contextKey.begin(), ::tolower);

        if (config.contains(contextKey)) {
            return "\033[" + config[contextKey] + "m";
        }
    }

    // ✅ **Fallback to Log Level Colors**
    switch (level) {
    case LogLevel::INFO: colorKey = "info_color"; break;
    case LogLevel::WARN: colorKey = "warn_color"; break;
    case LogLevel::ERROR: colorKey = "error_color"; break;
    case LogLevel::DEBUG: colorKey = "debug_color"; break;
    }

    return config.contains(colorKey) ? "\033[" + config[colorKey] + "m" : "\033[37m";  // Default to white
}

bool LoggerConfig::isContextEnabled(LogContext context) {
    return enabledContexts.contains(context);
}

// ✅ Utility function to remove leading/trailing whitespace
std::string LoggerConfig::trim(const std::string& str) {
    std::string s = str;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    return s;
}

// Accessors
std::string LoggerConfig::getLogDirectory() { return config["log_directory"]; }
std::string LoggerConfig::getLogFilenameFormat() { return config["log_filename_format"]; }
int LoggerConfig::getLogRotationDays() { return std::stoi(config["log_rotation_days"]); }
bool LoggerConfig::isConsoleEnabled() { return config["enable_console"] == "true"; }
bool LoggerConfig::isFileEnabled() { return config["enable_file"] == "true"; }
bool LoggerConfig::isColorEnabled() { return config["enable_colors"] == "true"; }
std::string LoggerConfig::getFlushMode() { return config["flush_mode"]; }
std::string LoggerConfig::getLogLevel() { return config["log_level"]; }
