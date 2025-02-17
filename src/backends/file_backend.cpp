#include "file_backend.hpp"

//------------------------------------------------------------------------------
// Generate a Log Filename Based on the Config Format
//------------------------------------------------------------------------------
std::string FileBackend::resolveFilename(const std::string& format) {
    std::ostringstream filename;
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::localtime(&now);
    filename << std::put_time(timeinfo, format.c_str());
    return filename.str();
}

//------------------------------------------------------------------------------
// Setup File Logging
//------------------------------------------------------------------------------
void FileBackend::setup(const LoggerSettings& settings) {
    if (settings.config.backends.enableFile) {
        std::filesystem::path logDir = settings.config.general.logDirectory;
        if (!std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }

        logFilePath = settings.config.general.logDirectory + "/" + resolveFilename(settings.config.general.logFilenameFormat);

        std::ofstream testFile(logFilePath, std::ios::app);
        if (!testFile) {
            throw std::runtime_error("[FileBackend] Failed to open log file: " + logFilePath);
        }
    }
}

//------------------------------------------------------------------------------
// Write Log Message to File
//------------------------------------------------------------------------------
void FileBackend::write(const LogMessage& log, [[maybe_unused]] const LoggerSettings& settings) {
    std::ofstream logFile(logFilePath, std::ios::app);
    if (logFile.is_open()) {
        logFile << log.timestamp << " [" << log.level << "] " << log.context << ": " << log.message << "\n";
        logFile.close();
    }
}

//------------------------------------------------------------------------------
// Flush (No-op for file backend)
//------------------------------------------------------------------------------
void FileBackend::flush() {}

//------------------------------------------------------------------------------
// Shutdown (No-op for file backend)
//------------------------------------------------------------------------------
void FileBackend::shutdown() {}
