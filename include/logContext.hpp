#ifndef LOG_CONTEXT_HPP
#define LOG_CONTEXT_HPP

#include <string>

enum class LogContext {
    GENERAL,
    NETWORK,
    DATABASE,
    UI,
    AUDIO,
    RENDERING
};

inline std::string to_string(LogContext context) {
    switch (context) {
    case LogContext::GENERAL: return "general";
    case LogContext::NETWORK: return "network";
    case LogContext::DATABASE: return "database";
    case LogContext::UI: return "ui";
    case LogContext::AUDIO: return "audio";
    case LogContext::RENDERING: return "rendering";
    default: return "unknown";
    }
}

inline LogContext logContextFromString(const std::string& contextStr) {
    if (contextStr == "GENERAL") return LogContext::GENERAL;
    if (contextStr == "NETWORK") return LogContext::NETWORK;
    if (contextStr == "DATABASE") return LogContext::DATABASE;
    if (contextStr == "UI") return LogContext::UI;
    if (contextStr == "AUDIO") return LogContext::AUDIO;
    if (contextStr == "RENDERING") return LogContext::RENDERING;
    return LogContext::GENERAL;  // Default if unknown
}

#endif // LOG_CONTEXT_HPP
