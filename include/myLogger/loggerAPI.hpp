#pragma once

#include <string_view>

namespace myLogger {

    struct LoggerAPI {
        void (*log)(void* instance, std::string_view level, std::string_view context, std::string_view message);
    };

} // namespace myLogger
