cmake_minimum_required(VERSION 3.16)
project(advanced_demo)

add_executable(advanced_demo main.cpp)
target_link_libraries(advanced_demo PRIVATE myLoggerLib) # Link with myLoggerLib

# Do NOT install this executable
