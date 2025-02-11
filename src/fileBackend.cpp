#include "fileBackend.hpp"
#include "loggerSettings.hpp"
#include <filesystem>
#include <iostream>

// ✅ Default constructor
FileBackend::FileBackend() : FileBackend(generateLogFilePath()) {}

FileBackend::FileBackend(const std::string& filename) : filename(filename) {
    ensureLogDirectoryExists();
    logFile.open(filename, std::ios::app);
    if (!logFile) {
        std::cerr << "[ERROR] Failed to open log file: " << filename << "\n";
    }
}

void FileBackend::write(const LogMessage& logMessage) {
    if (!logFile.is_open()) return;

    std::ostringstream oss;

    if (LoggerSettings::getInstance().isTimestampEnabled()) {
        oss << "[" << logMessage.timestamp << "] ";  // ✅ Use pre-stored timestamp
    }

    oss << "[" << logMessage.level << "] "
        << "[" << logMessage.context << "] "
        << logMessage.message << "\n";

    logFile << oss.str();
}

void FileBackend::flush() {  // ✅ Fixed redundant scope resolution
    if (logFile.is_open()) {
        logFile.flush();
    }
}

void FileBackend::ensureLogDirectoryExists() {
    std::string logDir = LoggerSettings::getInstance().getGlobalSetting("log_directory");
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }
}

std::string FileBackend::generateLogFilePath() {
    std::string logDir = LoggerSettings::getInstance().getGlobalSetting("log_directory");
    std::string filenamePattern = LoggerSettings::getInstance().getGlobalSetting("log_filename_format");

    // Ensure log directory exists
    std::filesystem::create_directories(logDir);

    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now_time);
#else
    localtime_r(&now_time, &localTime);
#endif

    std::ostringstream oss;
    oss << logDir << std::put_time(&localTime, filenamePattern.c_str());

    return oss.str();
}

void FileBackend::cleanOldLogs(int days) {
    namespace fs = std::filesystem;
    auto now = std::chrono::system_clock::now();
    std::string logDir = LoggerSettings::getInstance().getGlobalSetting("log_directory");

    for (const auto& entry : fs::directory_iterator(logDir)) {
        if (entry.is_regular_file()) {
            auto ftime = fs::last_write_time(entry);
            auto file_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + now);
            auto file_age = std::chrono::duration_cast<std::chrono::hours>(now - file_time).count() / 24;

            if (file_age >= days) {
                fs::remove(entry);
            }
        }
    }
}