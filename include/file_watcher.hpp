#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <atomic>
#include <filesystem>

template <typename LoggerType>
class FileWatcher {
public:
    static void watch(LoggerType& logger, const std::string& configFile, std::atomic<bool>& exitFlag);
};

#endif // FILE_WATCHER_HPP
