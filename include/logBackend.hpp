#ifndef LOG_BACKEND_HPP
#define LOG_BACKEND_HPP

#include <fstream>
#include <string>

// Base interface for logging backends
class LogBackend {
public:
    virtual ~LogBackend() = default;
    virtual void write(const std::string& message) = 0;
};

// Console Backend Declaration
class ConsoleBackend : public LogBackend {
public:
    void write(const std::string& message) override;
};

// File Backend Declaration
class FileBackend : public LogBackend {
public:
    explicit FileBackend(const std::string& filename);
    void write(const std::string& message) override;

private:
    std::ofstream logFile;
};

#endif // LOG_BACKEND_HPP
