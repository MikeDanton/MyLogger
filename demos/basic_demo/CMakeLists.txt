cmake_minimum_required(VERSION 3.16)
project(basic_demo)

add_executable(basic_demo main.cpp)
target_link_libraries(basic_demo PRIVATE myLoggerLib) # Link with myLoggerLib

# Do NOT install this executable
