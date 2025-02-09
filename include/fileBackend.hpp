#ifndef FILE_BACKEND_HPP
#define FILE_BACKEND_HPP

#include "logBackend.hpp"
#include <fstream>
#include <string>

// File Logging Backend
class FileBackend : public LogBackend {
public:
    explicit FileBackend(const std::string& filename);
    void write(const std::string& message) override;

private:
    std::ofstream logFile;
};

#endif // FILE_BACKEND_HPP
