cmake_minimum_required(VERSION 3.16)
project(myLoggerAppDemo)

set(CMAKE_CXX_STANDARD 20)

# ✅ Option to enable/disable logging for this demo
option(DEMO_LOGGING_ENABLED "Enable logging in demo (true/false)" ON)

# ✅ Define logging flag for this demo
if (NOT DEMO_LOGGING_ENABLED)
    add_definitions(-DLOGGING_ENABLED=0)
else()
    add_definitions(-DLOGGING_ENABLED=1)
endif()

# ✅ Create the demo executable (Removed taskManager.cpp)
add_executable(myLoggerApp main.cpp)

# ✅ Link against `myLoggerLib`
target_link_libraries(myLoggerApp PRIVATE myLoggerLib)
