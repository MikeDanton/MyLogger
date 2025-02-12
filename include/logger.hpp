#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <string>
#include <unordered_map>
#include <vector>
#include <array>

#define MAX_LEVELS 16
#define MAX_CONTEXTS 16
#define MAX_BACKENDS 4
#define MAX_LOG_ENTRIES 1024

struct LogMessage {
    char level[16];
    char context[64];
    char message[256];
    char timestamp[20]; // "YYYY-MM-DD HH:MM:SS"
};

struct LogBackend {
    void (*write)(const LogMessage*);
    void (*flush)();
};

struct LoggerSettings {
    bool enableConsole;
    bool enableFile;
    bool enableColors;
    bool enableTimestamps;

    std::array<bool, MAX_LEVELS> logLevelEnabledArray;
    std::array<int, MAX_LEVELS> logLevelSeveritiesArray;
    std::array<int, MAX_LEVELS> logColorArray;          // Level colors
    std::array<int, MAX_CONTEXTS> contextColorArray;    // Context colors
    std::array<int, MAX_CONTEXTS> contextSeverityArray;

    std::unordered_map<std::string, int> levelIndexMap;  // Fast level lookup
    std::unordered_map<std::string, int> contextIndexMap; // Fast context lookup
    std::unordered_map<std::string, int> parsedLogColors;

    std::vector<std::string> logLevelNames;
    std::vector<std::string> contextNames;

    std::string colorMode;
};

class Logger {
public:
    static Logger& getInstance();
    void init(const std::string& configFile = "config/logger.conf");
    void addBackend(LogBackend* backend);
    void log(const char* level, const char* context, const char* message);
    void flush();
    int getSeverity(const std::string& level) const;
    int getLevelIndex(const std::string& level) const;
    int getContextIndex(const std::string& context) const;
    void updateSettings(const std::string& configFile);

    const LoggerSettings& getSettings() const { return settings; }

private:
    Logger();
    ~Logger();
    void processQueue();

    LogBackend* backends[MAX_BACKENDS];
    int backendCount;
    LoggerSettings settings;

    LogMessage logQueue[MAX_LOG_ENTRIES];
    std::atomic<int> queueHead;
    std::atomic<int> queueTail;
    std::mutex mutex;
    std::thread logThread;
    std::atomic<bool> exitFlag;
};


#endif // LOGGER_H
