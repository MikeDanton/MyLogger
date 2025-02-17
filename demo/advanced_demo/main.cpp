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
    // 1) Create Logger
    auto logger = Logger<ConsoleBackend, FileBackend>::createLogger();

    // 2) Dereference the unique_ptr to get Logger&
    auto& loggerRef = *logger;

    // 3) Use loggerRef in your components
    SystemMonitor system(loggerRef);
    NetworkManager network(loggerRef);
    ApplicationCore app(loggerRef);

    system.checkSystem();
    network.monitorNetwork();
    app.runApp();

    // 4) Cleanly shut down
    logger->shutdown();
    return 0;
}
