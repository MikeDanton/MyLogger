#include "fileBackend.hpp"
#include "loggerConfig.hpp"
#include <ctime>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>

FileBackend::FileBackend() : FileBackend(generateLogFilePath()) {}

FileBackend::FileBackend(const std::string& filename) : filename(filename) {
    ensureLogDirectoryExists();
    logFile.open(filename, std::ios::app);
    if (!logFile) {
        std::cerr << "[ERROR] Failed to open log file: " << filename << "\n";
    }
}

void FileBackend::write(const std::string& message) {
    if (logFile.is_open()) {
        logFile << message;
    }
}

void FileBackend::ensureLogDirectoryExists() {
    std::string logDir = LoggerConfig::getLogDirectory();
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
        std::cout << "[DEBUG] Created log directory: " << logDir << "\n";
    }
}

std::string FileBackend::generateLogFilePath() {
    std::string logDir = LoggerConfig::getLogDirectory();
    std::string filenamePattern = LoggerConfig::getLogFilenameFormat();

    ensureLogDirectoryExists();

    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now_time);
#else
    localtime_r(&now_time, &localTime);
#endif

    std::ostringstream oss;
    for (size_t i = 0; i < filenamePattern.size(); ++i) {
        if (filenamePattern[i] == '%' && i + 1 < filenamePattern.size()) {
            switch (filenamePattern[i + 1]) {
                case 'Y': oss << (localTime.tm_year + 1900); break;
                case 'm': oss << std::setw(2) << std::setfill('0') << (localTime.tm_mon + 1); break;
                case 'd': oss << std::setw(2) << std::setfill('0') << localTime.tm_mday; break;
                case 'H': oss << std::setw(2) << std::setfill('0') << localTime.tm_hour; break;
                case 'M': oss << std::setw(2) << std::setfill('0') << localTime.tm_min; break;
                case 'S': oss << std::setw(2) << std::setfill('0') << localTime.tm_sec; break;
                default: oss << filenamePattern[i] << filenamePattern[i + 1]; break;
            }
            ++i;
        } else {
            oss << filenamePattern[i];
        }
    }

    return logDir + oss.str();
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
                std::cout << "[DEBUG] Removed old log file: " << entry.path() << "\n";
            }
        }
    }
}

std::string FileBackend::getFilename() const {
    return filename;
}
