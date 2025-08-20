#pragma once

#include "logger_instance.hpp"
#include "backends/console_backend.hpp"
#include "backends/file_backend.hpp"
#include <memory>

namespace myLogger {

    class LoggerFactory {
    public:
        template <typename... Backends>
        static std::unique_ptr<LoggerInstance<Backends...>> createLogger() {
            auto settings = std::make_shared<LoggerSettings>();
            LoggerConfig::loadOrGenerateConfig("config/logger.conf", *settings);

            // ✅ Move-created backends into LoggerInstance
            return std::make_unique<LoggerInstance<Backends...>>(settings, Backends{}...);
        }
    };

} // namespace myLogger
