#include "loggerManager.hpp"
#include "taskManager.hpp"
#include <iostream>
#include <chrono>
#include <loggerSettings.hpp>

void interactiveSession(TaskManager& taskManager) {
    while (true) {
        std::cout << "\n=== Task Manager ===\n";
        std::cout << "1. Add Task\n2. List Tasks\n3. Remove Task\n4. Simulate Actions\n5. Exit\n";
        std::cout << "Enter choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            std::cout << "Enter task: ";
            std::string task;
            std::getline(std::cin, task);
            taskManager.addTask(task);
        } else if (choice == 2) {
            taskManager.listTasks();
        } else if (choice == 3) {
            std::cout << "Enter task number to remove: ";
            int index;
            std::cin >> index;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            taskManager.removeTask(index);
        } else if (choice == 4) {
            taskManager.runSimulation();
        } else if (choice == 5) {
            std::cout << "Exiting Task Manager.\n";
            break;
        } else {
            std::cout << "Invalid choice! Try again.\n";
        }
    }
}

void logTestMessages(Logger& logger) {
    logger.log("INFO", "Starting log test...");

    // ✅ Fetch dynamically configured contexts
    std::vector<std::string> contextNames = LoggerSettings::getInstance().getConfiguredContexts();

    for (const auto& contextName : contextNames) {
        std::string levelStr = LoggerSettings::getInstance().getContextLogLevel(contextName);

        logger.log(levelStr, contextName, "Test message"); // ✅ Now correctly logging contexts
    }

    logger.log("INFO", "Log test complete.");
}

int main() {
    Logger& logger = LoggerManager::getInstance();

    logger.log("INFO", "Task Manager starting...");

    logTestMessages(logger);

    TaskManager taskManager(logger);
    interactiveSession(taskManager);

    logger.log("INFO", "Task Manager session ended.");

    // ✅ Ensure logs are written before exiting
    logger.flush();

    return 0;
}
