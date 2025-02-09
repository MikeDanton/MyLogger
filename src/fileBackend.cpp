#include "fileBackend.hpp"

// File Backend Implementation
FileBackend::FileBackend(const std::string& filename) : logFile(filename, std::ios::app) {}

void FileBackend::write(const std::string& message) {
    logFile << message;
}
