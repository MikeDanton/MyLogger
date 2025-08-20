#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "myLogger/logger_core.hpp"
#include "myLogger/logger_config.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>

class FileBackend {
private:
    std::string logFilePath;

    std::string resolveFilename(const std::string& format);

public:
    FileBackend() = default;

    void setup(const LoggerSettings& settings);
    void write(const LogMessage& log, const LoggerSettings& settings);
    void flush();
    void shutdown();
};

#endif // FILE_BACKEND_HPP
