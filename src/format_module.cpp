#include "format_module.hpp"
#include <cstdio>  // ✅ Needed for snprintf()

std::string FormatModule::getCurrentTimestamp(const LoggerSettings::Format& format) {
    char timestamp[32];  // ✅ Fixed-size buffer
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    if (format.timestampFormat == "ISO") {
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", timeinfo);
    } else if (format.timestampFormat == "short") {
        std::strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    } else if (format.timestampFormat == "epoch") {
        std::snprintf(timestamp, sizeof(timestamp), "%ld", now);
    }

    return std::string(timestamp);
}

// ✅ Refactored `formatLogMessage()` to handle fixed-size `char[]` buffers
std::string FormatModule::formatLogMessage(const LogMessage& log, const LoggerSettings::Format& format) {
    char formattedMessage[512];  // ✅ Preallocate memory (avoid heap allocations)

    if (format.logFormat == "json") {
        std::snprintf(formattedMessage, sizeof(formattedMessage),
                      R"({"timestamp":"%s", "level":"%s", "context":"%s", "message":"%s"})",
                      log.timestamp, log.level, log.context, log.message);
    } else {
        std::snprintf(formattedMessage, sizeof(formattedMessage),
                      "[%s] [%s] %s: %s",
                      log.timestamp, log.level, log.context, log.message);
    }

    return std::string(formattedMessage);
}
