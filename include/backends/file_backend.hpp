#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logger_core.hpp"
#include "logger_config.hpp"
#include "logger_modules.hpp"  // ✅ Include to access modules
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

class FileBackend {
private:
    std::ofstream logFile;
    std::string logFilePath;
    LoggerModules& modules;  // ✅ Store reference to modules

    // ✅ Helper function to resolve filename format
    std::string resolveFilename(const std::string& format) {
        std::ostringstream filename;
        std::time_t now = std::time(nullptr);
        std::tm* timeinfo = std::localtime(&now);
        filename << std::put_time(timeinfo, format.c_str());
        return filename.str();
    }

public:
    // ✅ Accepts LoggerModules
    explicit FileBackend(LoggerModules& mods) : modules(mods) {
        std::cerr << "[FileBackend] Initialized with LoggerModules.\n";
    }

    void setup(const LoggerSettings& settings) {
        if (settings.config.backends.enableFile) {
            // ✅ Ensure the directory exists
            std::filesystem::path logDir = settings.config.general.logDirectory;
            if (!std::filesystem::exists(logDir)) {
                std::filesystem::create_directories(logDir);
            }

            // ✅ Resolve filename format into an actual timestamped filename
            std::string resolvedFilename = resolveFilename(settings.config.general.logFilenameFormat);
            logFilePath = settings.config.general.logDirectory + "/" + resolvedFilename;

            logFile.open(logFilePath, std::ios::app);
            if (!logFile) {
                throw std::runtime_error("[FileBackend] Failed to open log file: " + logFilePath);
            }
        }
    }

    void write(const LogMessage& log, [[maybe_unused]] const LoggerSettings& settings) {
        std::ofstream logFile(logFilePath, std::ios::app); // Open on demand for thread safety
        if (logFile.is_open()) {
            logFile << log.timestamp << " [" << log.level << "] " << log.context << ": " << log.message << "\n";
            logFile.close();
        }
    }

    void flush() {
        if (logFile.is_open()) {
            std::cout << "[FileBackend] Flushing log file: " << logFilePath << "\n";
            logFile.flush();
        }
    }

    void shutdown() {
        flush();
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

#endif // FILE_BACKEND_HPP
