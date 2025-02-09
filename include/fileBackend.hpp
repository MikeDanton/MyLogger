#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logBackend.hpp"
#include <fstream>
#include <string>

class FileBackend : public LogBackend {
public:
    FileBackend();  // Use auto-generated filename
    explicit FileBackend(const std::string& filename);

    void write(const std::string& message) override;
    void cleanOldLogs(int days);

    std::string getFilename() const;  // ✅ Returns the log filename

private:
    std::ofstream logFile;
    std::string filename;
    std::string generateTimestampFilename();
};

#endif // FILE_BACKEND_HPP
