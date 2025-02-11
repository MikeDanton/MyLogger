#ifndef LOG_MESSAGE_HPP
#define LOG_MESSAGE_HPP

#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

struct LogMessage {
    std::string level;
    std::string context;
    std::string message;
    std::string timestamp;

    LogMessage(const std::string& lvl, const std::string& ctx, const std::string& msg)
        : level(lvl), context(ctx), message(msg), timestamp(getCurrentTimestamp()) {}

private:
    // ✅ Generate timestamp in format: YYYY-MM-DD HH:MM:SS
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &now_time);
#else
        localtime_r(&now_time, &localTime);
#endif
        std::ostringstream oss;
        oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

#endif // LOG_MESSAGE_HPP
