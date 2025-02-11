#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>

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

    std::unordered_map<std::string, bool> logLevelEnabled;  // Dynamically mapped levels
    std::unordered_map<std::string, int> logLevelSeverities; // Numeric severity for each level, e.g. "DEBUG" -> 2, "ERROR" -> 5, etc.
    std::unordered_map<std::string, int> logColors;         // Colors per level & context
    std::unordered_map<std::string, std::string> contextLevels; // Context-based log filtering

    std::string colorMode;
};

class Logger {
public:
    static Logger& getInstance();
    void init(const std::string& configFile = "config/logger.conf");
    void addBackend(LogBackend* backend);
    void log(const char* level, const char* context, const char* message);
    void flush();

    const LoggerSettings& getSettings() const { return settings; }

private:
    Logger();
    ~Logger();
    void processQueue();
    int getSeverity(const std::string& level) const;

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
