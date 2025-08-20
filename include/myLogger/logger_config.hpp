#ifndef LOGGER_CONFIG_HPP
#define LOGGER_CONFIG_HPP

#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include <toml++/toml.hpp>

#define MAX_LEVELS 16
#define MAX_CONTEXTS 256

struct LoggerSettings
{
    struct General {
        std::string logDirectory = "logs/";
        std::string logFilenameFormat = "log_%Y-%m-%d_%H-%M-%S.txt";
        int logRotationDays = 7;
        std::string flushMode = "instant";
    };

    struct Format {
        bool enableTimestamps = true;
        std::string timestampFormat = "ISO";
        std::string logFormat = "plain";
    };

    // Simple struct for toggling which backends are enabled
    struct Backends {
        bool enableConsole = true;
        bool enableFile = true;
    };

    struct Display {
        bool enableColors = true;
        bool hideLevelTag = false;
        bool hideContextTag = false;
    };

    struct Levels {
        std::array<bool, MAX_LEVELS> enabledArray = {};
        std::array<int, MAX_LEVELS> severitiesArray = {};
        std::unordered_map<std::string, int> levelIndexMap;
        std::vector<std::string> levelNames;
    };

    struct Colors {
        std::string colorMode = "level";
        std::array<std::string, MAX_LEVELS> logColorArray = {};
        std::array<std::string, MAX_CONTEXTS> contextColorArray = {};
        std::unordered_map<std::string, std::string> parsedLogColors;
    };

    struct Contexts {
        std::unordered_map<std::string, int> contextIndexMap;
        std::array<int, MAX_CONTEXTS> contextSeverityArray = {};
        std::vector<std::string> contextNames;
    };

    // All settings grouped into one config
    struct Config {
        General   general;
        Format    format;
        Backends  backends;   // <--- Merged here
        Display   display;
        Levels    levels;
        Colors    colors;
        Contexts  contexts;
    } config;

    // Constructor
    LoggerSettings();
};

class LoggerConfig {
public:
    static void printConfigState(const LoggerSettings& settings);

    // Loads config from `filepath`, or generates a default if missing
    static void loadOrGenerateConfig(const std::string& filepath, LoggerSettings& settings);

private:
    // Actual function that parses the TOML file
    static void loadConfig(const std::string& filepath, LoggerSettings& settings);

    // If config file is missing, generate a default
    static void generateDefaultConfig(const std::string& filepath);

    // Helper functions
    static void loadGeneral(const toml::table& config, LoggerSettings& settings);
    static void loadFormat(const toml::table& config, LoggerSettings& settings);
    static void loadBackends(const toml::table& config, LoggerSettings& settings);
    static void loadDisplay(const toml::table& config, LoggerSettings& settings);
    static void loadLevels(const toml::table& config, LoggerSettings& settings);
    static void loadSeverities(const toml::table& config, LoggerSettings& settings);
    static void loadColors(const toml::table& config, LoggerSettings& settings);
    static void loadContexts(const toml::table& config, LoggerSettings& settings);
};

#endif // LOGGER_CONFIG_HPP
