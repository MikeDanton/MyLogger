cmake_minimum_required(VERSION 3.16)
project(myLoggerDemos)

# ✅ Add each demo directory
add_subdirectory(basic_demo)
add_subdirectory(advanced_demo)

message(STATUS "Building multiple demo applications.")
