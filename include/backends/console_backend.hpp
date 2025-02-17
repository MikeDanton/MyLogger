#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logger_core.hpp"
#include "format_module.hpp"
#include "logger_modules.hpp"
#include "logger_config.hpp"

class ConsoleBackend {
    ColorModule& colorModule;
public:
    explicit ConsoleBackend(LoggerModules& modules);

    void setup(const LoggerSettings& settings);
    void write(const LogMessage& log, const LoggerSettings& settings);
    void flush();
    void shutdown();
};

#endif // CONSOLE_BACKEND_HPP
