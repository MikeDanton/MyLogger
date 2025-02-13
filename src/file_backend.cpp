#include "file_backend.hpp"
#include <fstream>

void FileBackend::setup(const LoggerSettings& settings) {
    if (settings.enableFile) {
        logFile.open("log.txt", std::ios::app);
        if (!logFile) {
            throw std::runtime_error("[FileBackend] Failed to open log file.");
        }
    }
}

void FileBackend::write(const LogMessage* log, [[maybe_unused]] const LoggerSettings& settings) {
    if (logFile.is_open()) {
        logFile << "[" << log->level << "] " << log->context << ": " << log->message << std::endl;
    }
}

void FileBackend::flush() {
    if (logFile.is_open()) {
        logFile.flush();
    }
}
