#include "loggerSettings.hpp"
#include "logLevel.hpp"
#include <iostream>
#include <sstream>

LoggerSettings& LoggerSettings::getInstance() {
    static LoggerSettings instance;
    return instance;
}

void LoggerSettings::setContextLogLevel(const std::string& context, const std::string& level) {
    if (logLevelFromString(level) != LogLevel::INFO || level == "INFO") {
        contextLogLevels[context] = level;
    } else {
        std::cerr << "[WARNING] Invalid log level for context '" << context << "'. Using default.\n";
        contextLogLevels[context] = "INFO";
    }
}

std::string LoggerSettings::getColorForLog(LogLevel level, const std::string& context) {
    if (!isColorEnabled()) return "";  // ✅ No color if disabled

    std::string colorMode = getGlobalSetting("color_mode");
    std::string colorCode;

    if (colorMode == "context") {
        colorCode = getContextColor(context);  // ✅ FIXED: No need to prepend "context_"
    } else {
        switch (level) {
        case LogLevel::INFO:  colorCode = getColorSetting("info_color"); break;
        case LogLevel::WARN:  colorCode = getColorSetting("warn_color"); break;
        case LogLevel::ERROR: colorCode = getColorSetting("error_color"); break;
        case LogLevel::DEBUG: colorCode = getColorSetting("debug_color"); break;
        default: colorCode = "37";  // ✅ FIXED: Default to white if missing
        }
    }

    return "\033[" + colorCode + "m";  // ✅ Always prepend "\033[" and append "m"
}

std::string LoggerSettings::getContextLogLevel(const std::string& context) const {
    auto it = contextLogLevels.find(context);
    return (it != contextLogLevels.end()) ? it->second : "INFO";
}

bool LoggerSettings::isContextEnabled(const std::string& context) const {
    return contextLogLevels.find(context) != contextLogLevels.end();
}

void LoggerSettings::setGlobalSetting(const std::string& key, const std::string& value) {
    globalSettings[key] = value;
}

std::string LoggerSettings::getGlobalSetting(const std::string& key) const {
    auto it = globalSettings.find(key);
    return (it != globalSettings.end()) ? it->second : "";
}

// ✅ Fetch global settings
bool LoggerSettings::isConsoleEnabled() const {
    return getGlobalSetting("enable_console") == "true";
}

bool LoggerSettings::isFileEnabled() const {
    return getGlobalSetting("enable_file") == "true";
}

bool LoggerSettings::isColorEnabled() const {
    return getGlobalSetting("enable_colors") == "true";
}

std::string LoggerSettings::getLogLevel() const {
    return getGlobalSetting("log_level");
}

void LoggerSettings::setColorSetting(const std::string& key, const std::string& value) {
    colorSettings[key] = value;
}

std::string LoggerSettings::getColorSetting(const std::string& key) const {
    auto it = colorSettings.find(key);
    return (it != colorSettings.end()) ? it->second : "";  // Return empty if not found
}

void LoggerSettings::setContextColor(const std::string& context, const std::string& value) {
    contextColors[context] = value;
}

std::string LoggerSettings::getContextColor(const std::string& context) const {
    auto it = contextColors.find("context_" + context);
    if (it != contextColors.end()) {
        return "\033[" + it->second + "m";  // ✅ Return full ANSI sequence
    }
    return "\033[37m";  // ✅ Default to white if missing
}

std::vector<std::string> LoggerSettings::getConfiguredContexts() const {
    std::vector<std::string> contexts;
    for (const auto& [context, _] : contextLogLevels) {
        contexts.push_back(context);
    }
    return contexts;
}

std::string LoggerSettings::logContextFromString(const std::string& context) const {
    return isContextEnabled(context) ? context : "GENERAL";  // Default to GENERAL
}