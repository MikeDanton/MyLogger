#include "file_watcher.hpp"
#include <sys/inotify.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>
#include <logger.hpp>

void FileWatcher::watch(Logger& logger, const std::string& configFile, std::atomic<bool>& exitFlag) {
    int fd = inotify_init();
    if (fd < 0) {
        std::cerr << "[Logger] Failed to initialize inotify.\n";
        return;
    }

    std::filesystem::path configPath = configFile;
    int wd = inotify_add_watch(fd, configPath.parent_path().c_str(), IN_MODIFY);
    if (wd < 0) {
        std::cerr << "[Logger] Failed to watch " << configPath << "\n";
        close(fd);
        return;
    }

    std::cerr << "[Logger] Watching " << configPath << " for changes...\n";

    char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

    while (!exitFlag.load()) {
        ssize_t length = read(fd, buffer, sizeof(buffer));
        if (length < 0 && errno != EINTR) {
            std::cerr << "[Logger] Read error on inotify.\n";
            break;
        }

        struct inotify_event* event = (struct inotify_event*) buffer;
        if (event->mask & IN_MODIFY && configPath.filename() == event->name) {
            logger.updateSettings(configFile);  // ✅ Apply new settings immediately
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);
}
