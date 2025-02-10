#include "consoleBackend.hpp"
#include "loggerConfig.hpp"

void ConsoleBackend::write(const LogMessage& logMessage) {
    if (!LoggerConfig::isConsoleEnabled()) return;

    std::string color = LoggerConfig::getColorForLog(logMessage.level, logMessage.context);
    std::string resetColor = "\033[0m";  // Reset terminal color

    std::cout << color << "[" << to_string(logMessage.level) << "] "
              << "[" << to_string(logMessage.context) << "] "
              << logMessage.message << resetColor << std::endl;
}
