#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

#include "loggerLib.hpp"
#include <vector>
#include <string>

class TaskManager {
public:
    explicit TaskManager(Logger& logger);

    void addTask(const std::string& task);
    void listTasks();
    void removeTask(int index);
    void runSimulation();  // Automated testing with different log levels

private:
    Logger& logger;
    std::vector<std::string> tasks;
};

#endif // TASK_MANAGER_HPP