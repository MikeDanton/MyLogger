#ifndef MOCK_BACKEND_HPP
#define MOCK_BACKEND_HPP

#include "myLogger/logger_core.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

class MockBackend {
public:
    std::vector<std::string> logEntries;

    void setup([[maybe_unused]] const LoggerSettings& settings) {}

    void write(const LogMessage& log, [[maybe_unused]] const LoggerSettings& settings) {
        std::cout << "[MockBackend] Writing log: " << log.message << std::endl;

        std::string logEntry = std::string(log.timestamp) + " [" + log.level + "] " + log.context + ": " + log.message;
        logEntries.push_back(std::move(logEntry));
    }

    void flush() {
        logEntries.clear();
    }

    bool contains(const std::string& message) const {
        return std::any_of(logEntries.begin(), logEntries.end(),
                           [&](const std::string& entry) { return entry.find(message) != std::string::npos; });
    }

    void reset() {
        logEntries.clear();
    }

    void shutdown() {
        std::cout << "[MockBackend] Shutting down logger." << std::endl;
        flush();
    }
};

#endif // MOCK_BACKEND_HPP
