#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <atomic>

class Logger;

class FileWatcher {
public:
    static void watch(Logger& logger, const std::string& configFile, std::atomic<bool>& exitFlag);
};

#endif // FILE_WATCHER_HPP
