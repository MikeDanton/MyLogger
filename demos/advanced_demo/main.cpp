#include "myLogger/my_logger.hpp"
#include <memory>
#include <thread>

// Simulated System Components
class SystemMonitor {
    LoggerAPI* api;
    void* loggerInstance;
public:
    explicit SystemMonitor(LoggerAPI* api, void* instance) : api(api), loggerInstance(instance) {}

    void checkSystem() {
        api->log(loggerInstance, "INFO", "SYSTEM", "System running smoothly.");
        api->log(loggerInstance, "WARN", "SYSTEM", "Memory usage is getting high.");
    }
};

class NetworkManager {
    LoggerAPI* api;
    void* loggerInstance;
public:
    explicit NetworkManager(LoggerAPI* api, void* instance) : api(api), loggerInstance(instance) {}

    void monitorNetwork() {
        api->log(loggerInstance, "DEBUG", "NETWORK", "Checking network connectivity.");
        api->log(loggerInstance, "ERROR", "NETWORK", "Failed to connect to server.");
    }
};

class ApplicationCore {
    LoggerAPI* api;
    void* loggerInstance;
public:
    explicit ApplicationCore(LoggerAPI* api, void* instance) : api(api), loggerInstance(instance) {}

    void runApp() {
        api->log(loggerInstance, "INFO", "APP", "Application is starting.");
        api->log(loggerInstance, "CRITICAL", "APP", "Unexpected shutdown detected!");
    }
};

int main() {

    auto logger = LoggerFactory::createLogger<ConsoleBackend>();

    LoggerAPI* api = logger->getAPI();
    void* loggerInstance = logger->getInstance();

    SystemMonitor system(api, loggerInstance);
    NetworkManager network(api, loggerInstance);
    ApplicationCore app(api, loggerInstance);

    system.checkSystem();
    network.monitorNetwork();
    app.runApp();


    api->shutdown(loggerInstance);

    return 0;
}
