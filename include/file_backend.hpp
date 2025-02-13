#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logger.hpp"
#include <fstream>

struct FileBackend {
    FileBackend() = default;
    FileBackend(const FileBackend&) = delete;
    FileBackend& operator=(const FileBackend&) = delete;

    void setup(const LoggerSettings& settings);
    void write(const LogMessage* log, [[maybe_unused]] const LoggerSettings& settings);
    void flush();

private:
    std::ofstream logFile;
};

#endif // FILE_BACKEND_HPP
