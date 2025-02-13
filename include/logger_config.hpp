#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include <toml++/toml.hpp>

#define MAX_LEVELS 16
#define MAX_CONTEXTS 16

// LoggerSettings struct that holds configuration values
struct LoggerSettings {
    bool enableConsole = true;
    bool enableFile = true;
    bool enableColors = false;
    bool enableTimestamps = true;

    std::array<bool, MAX_LEVELS> logLevelEnabledArray = {};
    std::array<int, MAX_LEVELS> logLevelSeveritiesArray = {};
    std::array<int, MAX_LEVELS> logColorArray = {};
    std::array<int, MAX_CONTEXTS> contextColorArray = {};
    std::array<int, MAX_CONTEXTS> contextSeverityArray = {};

    std::unordered_map<std::string, int> levelIndexMap;
    std::unordered_map<std::string, int> contextIndexMap;
    std::unordered_map<std::string, int> parsedLogColors;

    std::vector<std::string> logLevelNames;
    std::vector<std::string> contextNames;

    std::string colorMode = "level";

    LoggerSettings();
};

// LoggerConfig class responsible for loading & managing settings
class LoggerConfig {
public:
    static void loadOrGenerateConfig(const std::string& filepath, LoggerSettings& settings);
    static void loadConfig(const std::string& filepath, LoggerSettings& settings);
    static void generateDefaultConfig(const std::string& filepath);
    static void precomputeColors(LoggerSettings& settings);

private:
    static void loadGeneralSettings(const toml::table& config, LoggerSettings& settings);
    static void loadToggles(const toml::table& config, LoggerSettings& settings);
    static void loadLevels(const toml::table& config, LoggerSettings& settings);
    static void loadSeverities(const toml::table& config, LoggerSettings& settings);
    static void loadColors(const toml::table& config, LoggerSettings& settings);
    static void loadContexts(const toml::table& config, LoggerSettings& settings);
};

#endif // LOGGER_CONFIG_HPP
