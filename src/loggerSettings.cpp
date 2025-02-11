#include "loggerSettings.hpp"

LoggerSettings& LoggerSettings::getInstance() {
    static LoggerSettings instance;
    return instance;
}

// ✅ Store log levels (INFO, WARN, ERROR, DEBUG) as ON/OFF
void LoggerSettings::setLogLevel(const std::string& level, const std::string& value) {
    logLevels[level] = value;
}

// ✅ Store context-based log levels
void LoggerSettings::setContextLogLevel(const std::string& context, const std::string& level) {
    contextLevels[context] = level;
}

std::string LoggerSettings::getContextLogLevel(const std::string& context) const {
    auto it = contextLevels.find(context);
    return (it != contextLevels.end()) ? it->second : getContextLogLevel("UNKNOWN");
}

// ✅ Unified method for checking if a log should be written
bool LoggerSettings::shouldLog(const std::string& level, const std::string& context) const {
    auto levelIt = logLevels.find(level);
    bool levelEnabled = (levelIt != logLevels.end() && levelIt->second == "ON");

    auto contextIt = contextLevels.find(context);
    bool contextExists = (contextIt != contextLevels.end());

    return levelEnabled && contextExists;
}

// ✅ Unified color lookup for levels and contexts
void LoggerSettings::setLogColor(const std::string& key, const std::string& value) {
    logColors[key] = value;
}

std::string LoggerSettings::getLogColor(const std::string& level, const std::string& context) const {
    if (!isColorEnabled()) return "";  // ✅ No color if disabled

    std::string colorMode = getGlobalSetting("color_mode");
    std::string colorKey;

    if (colorMode == "context") {
        colorKey = "context_" + context;
    } else {
        colorKey = "level_" + level;  // ✅ Ensure level colors match stored keys
    }

    auto it = logColors.find(colorKey);
    std::string colorCode = (it != logColors.end()) ? it->second : "37";  // ✅ Default to white

    return "\033[" + colorCode + "m";
}

// ✅ Global settings retrieval
void LoggerSettings::setGlobalSetting(const std::string& key, const std::string& value) {
    globalSettings[key] = value;
}

std::string LoggerSettings::getGlobalSetting(const std::string& key) const {
    auto it = globalSettings.find(key);
    return (it != globalSettings.end()) ? it->second : "";
}

// ✅ Feature toggles
bool LoggerSettings::isConsoleEnabled() const {
    return getGlobalSetting("enable_console") == "true";
}

bool LoggerSettings::isFileEnabled() const {
    return getGlobalSetting("enable_file") == "true";
}

bool LoggerSettings::isColorEnabled() const {
    return getGlobalSetting("enable_colors") == "true";
}

std::vector<std::string> LoggerSettings::getConfiguredContexts() const {
    std::vector<std::string> contexts;
    for (const auto& [context, _] : contextLevels) {
        contexts.push_back(context);
    }
    return contexts;
}

bool LoggerSettings::isTimestampEnabled() const {
    return getGlobalSetting("log_timestamps") == "true";
}
