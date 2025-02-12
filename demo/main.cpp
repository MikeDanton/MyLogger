#include "logger.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <filesystem>
#include <sys/inotify.h>
#include <unistd.h>

std::atomic<bool> exitFlag{false};

// ✅ Function to log periodic messages
void periodicLogging(Logger& logger) {
    int counter = 0;
    while (!exitFlag.load()) {
        logger.log("INFO", "GENERAL", ("Heartbeat log entry #" + std::to_string(counter++)).c_str());
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

// ✅ Function to simulate multi-threaded logging
void threadedLogging(Logger& logger) {
    while (!exitFlag.load()) {
        std::thread logThread([&]() {
            for (int i = 0; i < 5; i++) { // Reduced number of entries for live testing
                logger.log("INFO", "MULTI_THREAD", ("Threaded log entry #" + std::to_string(i)).c_str());
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });
        logThread.join();
        std::this_thread::sleep_for(std::chrono::seconds(3)); // Delay before spawning another thread
    }
}

void watchConfig(Logger& logger, const std::string& configFile) {
    int fd = inotify_init();
    if (fd < 0) {
        std::cerr << "[Logger] Failed to initialize inotify.\n";
        return;
    }

    std::filesystem::path configPath = configFile;
    int wd = inotify_add_watch(fd, configPath.parent_path().c_str(), IN_MODIFY);
    if (wd < 0) {
        std::cerr << "[Logger] Failed to watch " << configPath << "\n";
        close(fd);
        return;
    }

    std::cerr << "[Logger] Watching " << configPath << " for changes...\n";

    char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

    while (!exitFlag.load()) {
        ssize_t length = read(fd, buffer, sizeof(buffer));
        if (length < 0 && errno != EINTR) {
            std::cerr << "[Logger] Read error on inotify.\n";
            break;
        }

        struct inotify_event* event = (struct inotify_event*) buffer;
        if (event->mask & IN_MODIFY && configPath.filename() == event->name) {
            std::cerr << "[Logger] Config file modified, reloading...\n";
            logger.updateSettings(configFile);
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

// ✅ Entry point
int main() {
    Logger& logger = Logger::getInstance();
    logger.init();

    std::cout << "🌍 Logger is running. Modify `config/logger.conf` to see changes in real-time!\n";
    std::cout << "Press Ctrl+C to stop.\n";

    // ✅ Start background threads
    std::thread logThread(periodicLogging, std::ref(logger));
    std::thread multiThreadedLogger(threadedLogging, std::ref(logger));
    std::thread configWatcher(watchConfig, std::ref(logger), "config/logger.conf");

    // ✅ Wait for user to stop the program
    std::cin.get(); // Wait for ENTER key to stop (or use Ctrl+C)
    exitFlag.store(true);

    // ✅ Cleanup
    logThread.join();
    multiThreadedLogger.join();
    configWatcher.join();

    std::cout << "Flushing logs before exit...\n";
    logger.flush();  // ✅ Ensures all logs are written before exiting

    std::cout << "Logger test completed. Goodbye!\n";
    return 0;
}
