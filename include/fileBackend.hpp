#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logBackend.hpp"
#include <fstream>
#include <string>
#include <filesystem>

class FileBackend : public LogBackend {
public:
    FileBackend();  // ✅ Uses auto-generated filename based on config
    explicit FileBackend(const std::string& filename);

    void write(const std::string& message) override;
    void cleanOldLogs(int days);
    std::string getFilename() const;  // ✅ Returns the log filename

private:
    std::ofstream logFile;
    std::string filename;

    void ensureLogDirectoryExists();  // ✅ Ensures log directory is created
    std::string generateLogFilePath();  // ✅ Uses logger.conf format
};

#endif // FILE_BACKEND_HPP
