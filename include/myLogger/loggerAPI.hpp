#pragma once

struct LoggerAPI {
    void (*log)(void* instance, const char* level, const char* context, const char* message);
    void (*updateSettings)(void* instance, const char* configFile);
    void (*shutdown)(void* instance);
};