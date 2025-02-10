#ifndef LOGGER_SETTINGS_HPP
#define LOGGER_SETTINGS_HPP

#include <logLevel.hpp>
#include <unordered_map>
#include <string>
#include <vector>

class LoggerSettings {
public:
    static LoggerSettings& getInstance();

    void setContextLogLevel(const std::string& context, const std::string& level);
    std::string getContextLogLevel(const std::string& context) const;
    bool isContextEnabled(const std::string& context) const;

    void setGlobalSetting(const std::string& key, const std::string& value);
    std::string getGlobalSetting(const std::string& key) const;

    bool isConsoleEnabled() const;
    bool isFileEnabled() const;
    bool isColorEnabled() const;

    std::string getLogLevel() const;

    void setColorSetting(const std::string& key, const std::string& value);
    std::string getColorSetting(const std::string& key) const;
    std::string getColorForLog(LogLevel level, const std::string& context);

    void setContextColor(const std::string& context, const std::string& value);
    std::string getContextColor(const std::string& context) const;

    std::vector<std::string> getConfiguredContexts() const;
    std::string logContextFromString(const std::string& context) const;

private:
    LoggerSettings() = default;
    std::unordered_map<std::string, std::string> contextLogLevels;
    std::unordered_map<std::string, std::string> globalSettings;
    std::unordered_map<std::string, std::string> colorSettings;
    std::unordered_map<std::string, std::string> contextColors;
};

#endif // LOGGER_SETTINGS_HPP
