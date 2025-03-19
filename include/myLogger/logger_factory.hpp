#pragma once

#include "logger_instance.hpp"
#include "backends/console_backend.hpp"
#include "backends/file_backend.hpp"
#include <memory>

class LoggerFactory {
public:
    static std::unique_ptr<LoggerInstance<FileBackend, ConsoleBackend>> createLogger() {

        auto settings = std::make_shared<LoggerSettings>();
        LoggerConfig::loadOrGenerateConfig("config/logger.conf", *settings);

        FileBackend fileBackend;
        ConsoleBackend consoleBackend;

        // Return the LoggerInstance with managed settings
        return std::make_unique<LoggerInstance<FileBackend, ConsoleBackend>>(settings, fileBackend, consoleBackend);
    }
};