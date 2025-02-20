#include "myLogger/my_logger.hpp"
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
    // ✅ Create LoggerSettings
    auto settings = std::make_shared<LoggerSettings>();

    // ✅ Create ConsoleBackend & FileBackend instances
    ConsoleBackend consoleBackend;
    FileBackend fileBackend;

    // ✅ Create Logger with explicit settings & backends
    auto logger = Logger<ConsoleBackend, FileBackend>::createLogger(settings, consoleBackend, fileBackend);

    // ✅ Dereference unique_ptr to pass reference to components
    auto& loggerRef = *logger;

    // ✅ Initialize system components with logger reference
    SystemMonitor system(loggerRef);
    NetworkManager network(loggerRef);
    ApplicationCore app(loggerRef);

    system.checkSystem();
    network.monitorNetwork();
    app.runApp();

    // ✅ Cleanly shut down
    logger->shutdown();
    return 0;
}
