// advanced_logger_demo.cpp

#include "my_logger.hpp"
#include <memory>
#include <thread>

// Simulated System Components
class SystemMonitor {
    Logger<ConsoleBackend, FileBackend>& logger;
public:
    explicit SystemMonitor(Logger<ConsoleBackend, FileBackend>& log) : logger(log) {}
    void checkSystem() {
        logger.log("INFO", "SYSTEM", "System running smoothly.");
        logger.log("WARN", "SYSTEM", "Memory usage is getting high.");
    }
};

class NetworkManager {
    Logger<ConsoleBackend, FileBackend>& logger;
public:
    explicit NetworkManager(Logger<ConsoleBackend, FileBackend>& log) : logger(log) {}
    void monitorNetwork() {
        logger.log("DEBUG", "NETWORK", "Checking network connectivity.");
        logger.log("ERROR", "NETWORK", "Failed to connect to server.");
    }
};

class ApplicationCore {
    Logger<ConsoleBackend, FileBackend>& logger;
public:
    explicit ApplicationCore(Logger<ConsoleBackend, FileBackend>& log) : logger(log) {}
    void runApp() {
        logger.log("INFO", "APP", "Application is starting.");
        logger.log("CRITICAL", "APP", "Unexpected shutdown detected!");
    }
};

int main() {
    // Logger Configuration
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend consoleBackend;
    FileBackend fileBackend;
    Logger<ConsoleBackend, FileBackend> logger(settings, consoleBackend, fileBackend);
    
    // Inject Logger into Components
    SystemMonitor system(logger);
    NetworkManager network(logger);
    ApplicationCore app(logger);
    
    // Run Demo
    system.checkSystem();
    network.monitorNetwork();
    app.runApp();
    
    // Graceful Shutdown
    logger.shutdown();
    return 0;
}
