cmake_minimum_required(VERSION 3.16)
project(AdvancedLoggerDemo)

set(CMAKE_CXX_STANDARD 20)

add_executable(advancedLoggerDemo main.cpp
        advanced_logger_demo.cpp
        advanced_logger_demo.hpp)

target_link_libraries(advancedLoggerDemo PRIVATE myLoggerLib)
