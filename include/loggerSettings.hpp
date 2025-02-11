#ifndef LOGGER_SETTINGS_HPP
#define LOGGER_SETTINGS_HPP

#ifndef LOGGING_ENABLED
    #define LOGGING_ENABLED true
#endif

#include <unordered_map>
#include <string>
#include <vector>

class LoggerSettings {
public:
    static LoggerSettings& getInstance();

    void setLogLevel(const std::string& level, const std::string& value);
    void setContextLogLevel(const std::string& context, const std::string& level);
    std::string getContextLogLevel(const std::string& context) const;

    bool shouldLog(const std::string& level, const std::string& context) const;

    void setLogColor(const std::string& key, const std::string& value);
    std::string getLogColor(const std::string& level, const std::string& context) const;

    void setGlobalSetting(const std::string& key, const std::string& value);
    std::string getGlobalSetting(const std::string& key) const;

    bool isConsoleEnabled() const;
    bool isFileEnabled() const;
    bool isColorEnabled() const;

    std::vector<std::string> getConfiguredContexts() const;

private:
    LoggerSettings() = default;

    std::unordered_map<std::string, std::string> logLevels;
    std::unordered_map<std::string, std::string> contextLevels;
    std::unordered_map<std::string, std::string> logColors;
    std::unordered_map<std::string, std::string> globalSettings;
};

#endif // LOGGER_SETTINGS_HPP
