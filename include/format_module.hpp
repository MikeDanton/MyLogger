#ifndef FORMAT_MODULE_HPP
#define FORMAT_MODULE_HPP

#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "logger_config.hpp"  // ✅ Include for `LoggerSettings::Format`
#include "logger_core.hpp"    // ✅ Include for `LogMessage`

class FormatModule {
public:
    static std::string getCurrentTimestamp(const LoggerSettings::Format& format);
    static std::string formatLogMessage(const LogMessage& log, const LoggerSettings::Format& format);
};

#endif // FORMAT_MODULE_HPP
