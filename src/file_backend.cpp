#include "file_backend.h"
#include <fstream>

std::ofstream logFile("log.txt", std::ios::app);

void file_write(const LogMessage* log) {
    logFile << "[" << log->level << "] " << log->context << ": " << log->message << std::endl;
}

void file_flush() {
    logFile.flush();
}

// ✅ Define FileBackend (matches the declaration in `file_backend.h`)
LogBackend FileBackend = { file_write, file_flush };
