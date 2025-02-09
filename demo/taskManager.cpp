#include "taskManager.hpp"
#include <chrono>
#include <thread>

// Constructor
TaskManager::TaskManager(Logger& logger) : logger(logger) {}

// Add Task
void TaskManager::addTask(const std::string& task) {
    tasks.push_back(task);
    logger.log(LogLevel::INFO, "Added task: " + task);
}

// List Tasks
void TaskManager::listTasks() {
    if (tasks.empty()) {
        logger.log(LogLevel::WARN, "No tasks available.");
    } else {
        logger.log(LogLevel::INFO, "Listing tasks:");
        for (size_t i = 0; i < tasks.size(); ++i) {
            logger.log(LogLevel::INFO, std::to_string(i + 1) + ". " + tasks[i]);
        }
    }
}

// Remove Task
void TaskManager::removeTask(int index) {
    if (index < 1 || index > static_cast<int>(tasks.size())) {
        logger.log(LogLevel::ERROR, "Invalid task number: " + std::to_string(index));
        return;
    }
    logger.log(LogLevel::INFO, "Removed task: " + tasks[index - 1]);
    tasks.erase(tasks.begin() + (index - 1));
}

// Run a simulation of different logging levels
void TaskManager::runSimulation() {
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
