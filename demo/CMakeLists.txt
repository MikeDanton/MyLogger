cmake_minimum_required(VERSION 3.16)
project(myLoggerAppDemo)

set(CMAKE_CXX_STANDARD 20)

# ✅ Define a separate option for logging in this demo project
option(DEMO_LOGGING_ENABLED "Enable logging in demo (true/false)" ON)

# ✅ Override the logging setting only for this demo
if (NOT DEMO_LOGGING_ENABLED)
    add_definitions(-DLOGGING_ENABLED=0)
else()
    add_definitions(-DLOGGING_ENABLED=1)
endif()

# ✅ Create the demo executable
add_executable(myLoggerApp main.cpp taskManager.cpp)

# ✅ Link against `myLoggerLib` directly (instead of `find_package()`)
target_link_libraries(myLoggerApp PRIVATE myLoggerLib)
