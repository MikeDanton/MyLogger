#include "file_watcher.hpp"
#include <logger.hpp>
#include <console_backend.hpp>
#include <file_backend.hpp>
#include <string>
#include <atomic>
#include <filesystem>
#include <iostream>
#include <sys/inotify.h>
#include <unistd.h>

template <typename LoggerType>
void FileWatcher<LoggerType>::watch(LoggerType& logger, const std::string& configFile, std::atomic<bool>& exitFlag) {
    int fd = inotify_init();
    if (fd < 0) {
        return;
    }

    std::filesystem::path configPath = configFile;
    int wd = inotify_add_watch(fd, configPath.parent_path().c_str(), IN_MODIFY);
    if (wd < 0) {
        close(fd);
        return;
    }

    char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

    while (!exitFlag.load()) {
        ssize_t length = read(fd, buffer, sizeof(buffer));
        if (length < 0 && errno != EINTR) {
            break;
        }

        struct inotify_event* event = reinterpret_cast<struct inotify_event*>(buffer);
        if (event->mask & IN_MODIFY) {
            if (configPath.filename() == event->name) {
                logger.updateSettings(configFile);
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}

template class FileWatcher<Logger<LoggerBackends<ConsoleBackend, FileBackend>>>;
