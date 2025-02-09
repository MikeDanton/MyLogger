#include "fileBackend.hpp"
#include <ctime>
#include <sstream>
#include <filesystem>
#include <chrono>

std::string FileBackend::generateTimestampFilename() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream oss;
    oss << "log_"
        << (localTime.tm_year + 1900) << "-"
        << (localTime.tm_mon + 1) << "-"
        << localTime.tm_mday << "_"
        << localTime.tm_hour << "-"
        << localTime.tm_min << "-"
        << localTime.tm_sec << ".txt";
    return oss.str();
}

// ✅ Default constructor: Generates and stores a timestamped filename
FileBackend::FileBackend() : FileBackend(generateTimestampFilename()) {}

FileBackend::FileBackend(const std::string& filename)
    : filename(filename), logFile(filename, std::ios::app) {}

void FileBackend::write(const std::string& message) {
    logFile << message;
}

// ✅ Getter for log filename
std::string FileBackend::getFilename() const {
    return filename;
}

void FileBackend::cleanOldLogs(int days) {
    namespace fs = std::filesystem;
    auto now = std::chrono::system_clock::now();

    for (const auto& entry : fs::directory_iterator(".")) {  // Scan current directory
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            auto ftime = fs::last_write_time(entry);

            auto file_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - fs::file_time_type::clock::now() + now
            );

            auto file_age = std::chrono::duration_cast<std::chrono::hours>(now - file_time).count() / 24;

            if (file_age >= days) {
                fs::remove(entry);
            }
        }
    }
}
