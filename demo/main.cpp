#include "loggerManager.hpp"
#include "taskManager.hpp"  // Refactored TaskManager into its own file
#include <iostream>
#include <thread>
#include <chrono>

// Function to simulate real user input and logging behavior
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
            taskManager.runSimulation();  // Runs automated logging simulation
        } else if (choice == 5) {
            std::cout << "Exiting Task Manager.\n";
            break;
        } else {
            std::cout << "Invalid choice! Try again.\n";
        }
    }
}

int main() {
    // ✅ Load logging configuration from a config file
    LoggerManager::configureFromConfig("logger.conf");

    Logger& logger = LoggerManager::getInstance();
    logger.log(LogLevel::INFO, "Starting Task Manager...");
    logger.log(LogLevel::DEBUG, "Logger successfully configured from config file.");

    TaskManager taskManager(logger);

    // ✅ Interactive session starts immediately, no logging setup prompts needed
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
            taskManager.runSimulation();  // Runs automated logging simulation
        } else if (choice == 5) {
            std::cout << "Exiting Task Manager.\n";
            logger.log(LogLevel::INFO, "Task Manager session ended.");
            break;
        } else {
            std::cout << "Invalid choice! Try again.\n";
        }
    }

    return 0;
}