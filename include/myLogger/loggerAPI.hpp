#pragma once

namespace myLogger {

    struct LoggerAPI {
        void (*log)(void* instance, const char* level, const char* context, const char* message);
    };

} // namespace myLogger
