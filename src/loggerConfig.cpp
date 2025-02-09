#include "loggerConfig.hpp"
#include "logLevel.hpp"
#include "logContext.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

std::unordered_map<std::string, std::string> LoggerConfig::config;
std::unordered_set<LogContext> LoggerConfig::enabledContexts;  // ✅ Stores enabled contexts

void LoggerConfig::loadConfig(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Warning: Config file not found. Creating default config: " << filepath << "\n";
        saveConfig(filepath);
    }

    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error: Unable to read config file. Using defaults.\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // Ignore comments
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value)) {
            config[key] = value;
        }
    }

    // ✅ Parse enabled contexts
    if (config.contains("enabled_contexts")) {
        std::istringstream contextStream(config["enabled_contexts"]);
        std::string token;
        while (std::getline(contextStream, token, ',')) {
            enabledContexts.insert(logContextFromString(token));
        }
    }
}

void LoggerConfig::saveConfig(const std::string& filepath) {
    std::ofstream file(filepath);
    if (!file) {
        std::cerr << "Error: Could not create default config file.\n";
        return;
    }

    file << "# Logger Configuration File\n";
    file << "log_directory=logs/\n";
    file << "log_filename_format=log_%Y-%m-%d_%H-%M-%S.txt\n";
    file << "log_rotation_days=7\n";
    file << "enable_console=true\n";
    file << "enable_file=true\n";
    file << "enable_colors=false\n";
    file << "flush_mode=auto\n";
    file << "log_level=INFO\n";

    // ✅ Default context settings
    file << "enabled_contexts=GENERAL,NETWORK,UI\n";

    file << "info_color=32\n";   // Green
    file << "warn_color=33\n";   // Yellow
    file << "error_color=31\n";  // Red
    file << "debug_color=36\n";  // Cyan
}

std::string LoggerConfig::getColorForLevel(LogLevel level) {
    if (!isColorEnabled()) return "";

    std::string key;
    switch (level) {
    case LogLevel::INFO: key = "info_color"; break;
    case LogLevel::WARN: key = "warn_color"; break;
    case LogLevel::ERROR: key = "error_color"; break;
    case LogLevel::DEBUG: key = "debug_color"; break;
    }

    return config.contains(key) ? "\033[" + config[key] + "m" : logLevelColor(level);
}

bool LoggerConfig::isContextEnabled(LogContext context) {
    return enabledContexts.contains(context);
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
