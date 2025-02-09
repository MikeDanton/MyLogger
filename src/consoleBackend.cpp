#include "logBackend.hpp"
#include <iostream>
#include <fstream>

// Console Backend Implementation
void ConsoleBackend::write(const std::string& message) {
    std::cout << message;
}

// File Backend Implementation
FileBackend::FileBackend(const std::string& filename) : logFile(filename, std::ios::app) {}

void FileBackend::write(const std::string& message) {
    logFile << message;
}