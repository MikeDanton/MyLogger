cmake_minimum_required(VERSION 3.16)
project(BasicLoggerDemo)

set(CMAKE_CXX_STANDARD 20)

# ✅ Option to enable/disable logging for this demo
option(DEMO_LOGGING_ENABLED "Enable logging in demo (true/false)" ON)

# ✅ Define logging flag for this demo
if (NOT DEMO_LOGGING_ENABLED)
    add_definitions(-DLOGGING_ENABLED=0)
else()
    add_definitions(-DLOGGING_ENABLED=1)
endif()

# ✅ Create the demo executable
add_executable(basicLoggerDemo main.cpp)

# ✅ Link against `myLoggerLib`
target_link_libraries(basicLoggerDemo PRIVATE myLoggerLib)

# ✅ Debugging Info
message(STATUS "Basic Logger Demo logging enabled: ${DEMO_LOGGING_ENABLED}")
