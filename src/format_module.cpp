#include "format_module.hpp"

std::string FormatModule::getCurrentTimestamp(const LoggerSettings::Format& format) {
    std::ostringstream timestamp;
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);

    if (format.timestampFormat == "ISO") {
        timestamp << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S");
    } else if (format.timestampFormat == "short") {
        timestamp << std::put_time(timeinfo, "%H:%M:%S");
    } else if (format.timestampFormat == "epoch") {
        timestamp << now;
    }
    return timestamp.str();
}

// ✅ Refactored to accept `LogMessage` and `LoggerSettings::Format`
std::string FormatModule::formatLogMessage(const LogMessage& log, const LoggerSettings::Format& format) {
    if (format.logFormat == "json") {
        return "{\"timestamp\":\"" + log.timestamp +
               "\", \"level\":\"" + log.level +
               "\", \"context\":\"" + log.context +
               "\", \"message\":\"" + log.message + "\"}";
    }
    return "[" + log.timestamp + "] [" + log.level + "] " + log.context + ": " + log.message;
}
