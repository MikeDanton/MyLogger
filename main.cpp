#include <loggerLib.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// Simulated Task Manager with Various Log Levels
class TaskManager {
public:
    explicit TaskManager(Logger& logger) : logger(logger) {}

    void addTask(const std::string& task) {
        tasks.push_back(task);
        logger.log(LogLevel::INFO, "Added task: " + task);
    }

    void listTasks() {
        if (tasks.empty()) {
            logger.log(LogLevel::WARN, "No tasks available.");
        } else {
            logger.log(LogLevel::INFO, "Listing tasks:");
            for (size_t i = 0; i < tasks.size(); ++i) {
                logger.log(LogLevel::INFO, std::to_string(i + 1) + ". " + tasks[i]);
            }
        }
    }

    void removeTask(int index) {
        if (index < 1 || index > static_cast<int>(tasks.size())) {
            logger.log(LogLevel::ERROR, "Invalid task number: " + std::to_string(index));
            return;
        }
        logger.log(LogLevel::INFO, "Removed task: " + tasks[index - 1]);
        tasks.erase(tasks.begin() + (index - 1));
    }

    // **Automated Simulation with Different Log Levels**
    void runSimulation() {
        logger.log(LogLevel::INFO, "Starting automated simulation...");

        addTask("Buy groceries");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        addTask("Complete project");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        listTasks();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        removeTask(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        listTasks();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        removeTask(10);  // ✅ Should log an ERROR
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        removeTask(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        listTasks();  // ✅ Should log a WARN since the list is empty
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        logger.log(LogLevel::INFO, "Simulation completed.");
    }

private:
    Logger& logger;
    std::vector<std::string> tasks;
};

// **Main Program with User Logging Selection**
int main() {
    Logger logger;

    std::cout << "Select Logging Mode:\n";
    std::cout << "1. Console Only\n2. Console + File\nEnter choice: ";

    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice == 2) {
        auto fileBackend = std::make_unique<FileBackend>();  // ✅ Uses timestamped file
        std::cout << "Logging to both console and file: " << fileBackend->getFilename() << "\n";
        logger.addBackend(std::move(fileBackend));
    }

    std::cout << "Set Log Level:\n";
    std::cout << "1. INFO (All Logs)\n2. WARN and ERROR Only\n3. ERROR Only\nEnter choice: ";

    int logLevelChoice;
    std::cin >> logLevelChoice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (logLevelChoice == 2) {
        logger.setLogLevel(LogLevel::WARN);
    } else if (logLevelChoice == 3) {
        logger.setLogLevel(LogLevel::ERROR);
    }

    TaskManager taskManager(logger);
    taskManager.runSimulation();

    std::cout << "Simulation completed. Check logs for details.\n";
    return 0;
}
