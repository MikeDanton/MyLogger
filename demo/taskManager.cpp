#include "taskManager.hpp"
#include <chrono>
#include <thread>

// Constructor
TaskManager::TaskManager(Logger<LOGGING_ENABLED>& logger) : logger(logger) {}

// Add Task
void TaskManager::addTask(const std::string& task) {
    tasks.push_back(task);
    logger.log("INFO", "TaskManager", "Added task: " + task);
}

// List Tasks
void TaskManager::listTasks() {
    if (tasks.empty()) {
        logger.log("WARN", "TaskManager", "No tasks available.");
    } else {
        logger.log("INFO", "TaskManager", "Listing tasks:");
        for (size_t i = 0; i < tasks.size(); ++i) {
            logger.log("INFO", "TaskManager", std::to_string(i + 1) + ". " + tasks[i]);
        }
    }
}

// Remove Task
void TaskManager::removeTask(int index) {
    if (index < 1 || index > static_cast<int>(tasks.size())) {
        logger.log("ERROR", "TaskManager", "Invalid task number: " + std::to_string(index));
        return;
    }
    logger.log("INFO", "TaskManager", "Removed task: " + tasks[index - 1]);
    tasks.erase(tasks.begin() + (index - 1));
}

// Run a simulation of different logging levels
void TaskManager::runSimulation() {
    logger.log("INFO", "TaskManager", "Starting automated simulation...");

    addTask("Buy groceries");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    addTask("Complete project");
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    listTasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    removeTask(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    listTasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    removeTask(10);  // ✅ Should log an ERROR
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    removeTask(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    listTasks();  // ✅ Should log a WARN since the list is empty
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    logger.log("INFO", "TaskManager", "Simulation completed.");
}
