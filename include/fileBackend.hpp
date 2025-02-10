#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logBackend.hpp"
#include "logMessage.hpp"
#include <fstream>
#include <string>
#include <filesystem>

class FileBackend : public LogBackend {
public:
    FileBackend();
    explicit FileBackend(const std::string& filename);

    void write(const LogMessage& message) override;
    void cleanOldLogs(int days);
    std::string getFilename() const;

private:
    std::ofstream logFile;
    std::string filename;

    void ensureLogDirectoryExists();
    std::string generateLogFilePath();
};

#endif // FILE_BACKEND_HPP
