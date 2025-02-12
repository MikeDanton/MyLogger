#include "logger_controller.hpp"
#include <iostream>

int main() {
    LoggerController controller;
    controller.start();

    std::cin.get(); // Wait for user input to stop
    controller.stop();

    return 0;
}
