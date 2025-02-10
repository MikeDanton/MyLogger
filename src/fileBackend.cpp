#include "fileBackend.hpp"
#include "loggerConfig.hpp"
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>

// ✅ Default constructor calls `generateLogFilePath()`
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

    // ✅ Get current timestamp
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now_time);
#else
    localtime_r(&now_time, &localTime);
#endif

    std::ostringstream oss;
    oss << "[" << (localTime.tm_year + 1900) << "-"
        << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1) << "-"
        << std::setw(2) << std::setfill('0') << localTime.tm_mday << " "
        << std::setw(2) << std::setfill('0') << localTime.tm_hour << ":"
        << std::setw(2) << std::setfill('0') << localTime.tm_min << ":"
        << std::setw(2) << std::setfill('0') << localTime.tm_sec << "] ";

    // ✅ Write log entry
    logFile << oss.str() << "[" << to_string(logMessage.level) << "] "
            << "[" << to_string(logMessage.context) << "] "
            << logMessage.message << "\n";
}

void FileBackend::ensureLogDirectoryExists() {
    std::string logDir = LoggerConfig::getLogDirectory();
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }
}

std::string FileBackend::generateLogFilePath() {
    std::string logDir = LoggerConfig::getLogDirectory();
    std::string filenamePattern = LoggerConfig::getLogFilenameFormat(); // "log_%Y-%m-%d_%H-%M-%S.txt"

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

    // Use `std::put_time` for formatting the filename pattern
    std::ostringstream oss;
    oss << logDir << std::put_time(&localTime, filenamePattern.c_str());

    return oss.str();
}

void FileBackend::cleanOldLogs(int days) {
    namespace fs = std::filesystem;
    auto now = std::chrono::system_clock::now();
    std::string logDir = LoggerConfig::getLogDirectory();

    for (const auto& entry : fs::directory_iterator(logDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
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

std::string FileBackend::getFilename() const {
    return filename;
}