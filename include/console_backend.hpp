#ifndef CONSOLE_BACKEND_HPP
#define CONSOLE_BACKEND_HPP

#include "logger.hpp"

struct ConsoleBackend {
    void setup([[maybe_unused]] const LoggerSettings& settings) {
        // ✅ No setup needed for now, but exists to satisfy `Logger::init()`
    }

    void write(const LogMessage* log, const LoggerSettings& settings);
    void flush();
};

#endif // CONSOLE_BACKEND_HPP
