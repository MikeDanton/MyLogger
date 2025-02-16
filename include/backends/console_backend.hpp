#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logger_core.hpp"
#include "format_module.hpp"
#include "color_module.hpp"
#include "logger_config.hpp"

class ConsoleBackend {
public:
    void setup([[maybe_unused]] const LoggerSettings& settings) {}
    void write(const LogMessage& log, const LoggerSettings& settings);
    void flush();
};

#endif // CONSOLE_BACKEND_HPP
