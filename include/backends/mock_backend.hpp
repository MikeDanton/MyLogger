#ifndef MOCK_BACKEND_HPP
#define MOCK_BACKEND_HPP

#include "logger_core.hpp"
#include <vector>
#include <string>
#include <algorithm>

class MockBackend {
public:
    std::vector<std::string> logEntries;

    // ✅ Empty setup function to satisfy Logger requirements
    void setup([[maybe_unused]] const LoggerSettings& settings) {}

    void write(const LogMessage& log, [[maybe_unused]] const LoggerSettings& settings) {
        logEntries.push_back(log.timestamp + " [" + log.level + "] " + log.context + ": " + log.message);
    }

    void flush() {
        logEntries.clear();
    }

    // ✅ Helper function to check if a message exists in logs
    bool contains(const std::string& message) const {
        return std::any_of(logEntries.begin(), logEntries.end(),
                           [&](const std::string& entry) { return entry.find(message) != std::string::npos; });
    }

    // ✅ Clear logs for fresh tests
    void reset() {
        logEntries.clear();
    }
};

#endif // MOCK_BACKEND_HPP
