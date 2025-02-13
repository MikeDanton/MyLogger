#include "file_backend.hpp"
#include <fstream>
#include <iomanip>
#include <ctime>

void FileBackend::setup(const LoggerSettings& settings) {
    if (!settings.enableFile) return;

    logFile.open("log.txt", std::ios::app);
    if (!logFile) {
        throw std::runtime_error("[FileBackend] Failed to open log file.");
    }
}

void FileBackend::write(const LogMessage* log, const LoggerSettings& settings) {
    if (!logFile.is_open()) return;

    std::ostringstream logEntry;

    // Timestamp Handling
    if (settings.enableTimestamps) {
        std::time_t now = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&now);

        if (settings.timestampFormat == "ISO") {
            logEntry << std::put_time(timeinfo, "%Y-%m-%dT%H:%M:%S") << " ";
        } else if (settings.timestampFormat == "short") {
            logEntry << std::put_time(timeinfo, "%H:%M:%S") << " ";
        } else if (settings.timestampFormat == "epoch") {
            logEntry << now << " ";
        }
    }

    // Level & Context Handling
    if (!settings.hideLevelTag) {
        logEntry << "[" << log->level << "] ";
    }
    if (!settings.hideContextTag) {
        logEntry << log->context << ": ";
    }

    logEntry << log->message << "\n";

    // Write to File
    logFile << logEntry.str();
}

void FileBackend::flush() {
    if (logFile.is_open()) {
        logFile.flush();
    }
}
