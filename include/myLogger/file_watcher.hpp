// file_watcher.hpp
#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <atomic>
#include <filesystem>
#include <chrono>
#include <thread>
#include <cerrno>
#include <iostream>

#if !defined(_WIN32)
  #include <sys/inotify.h>
  #include <unistd.h>
  #include <sys/select.h>
  #include <limits.h>
  #include <cstring>
#endif

template <typename LoggerType>
class FileWatcher {
public:
    static void watch(LoggerType& logger, const std::string& configFile, std::atomic<bool>& exitFlag) {
#if defined(_WIN32)
        namespace fs = std::filesystem;
        fs::file_time_type last = fs::exists(configFile) ? fs::last_write_time(configFile) : fs::file_time_type::min();
        while (!exitFlag.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            fs::file_time_type now = fs::exists(configFile) ? fs::last_write_time(configFile) : fs::file_time_type::min();
            if (now != last) {
                last = now;
                logger.updateSettings(configFile);
            }
        }
#else
        int fd = inotify_init1(IN_NONBLOCK);
        if (fd < 0) {
            std::cerr << "[FileWatcher] Error: inotify_init1 failed: " << strerror(errno) << "\n";
            return;
        }

        std::filesystem::path configPath = configFile;
        std::string dirToWatch = configPath.parent_path().empty() ? "." : configPath.parent_path().string();

        int wd = inotify_add_watch(fd, dirToWatch.c_str(), IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_MOVED_TO);
        if (wd < 0) {
            std::cerr << "[FileWatcher] Error: Failed to watch directory: " << dirToWatch << " - " << strerror(errno) << "\n";
            close(fd);
            return;
        }

        const size_t bufSize = sizeof(struct inotify_event) + NAME_MAX + 1;
        std::vector<char> buffer(bufSize);

        while (!exitFlag.load()) {
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            timeval tv{2, 0};
            int rv = select(fd + 1, &rfds, nullptr, nullptr, &tv);
            if (rv <= 0) continue;

            const ssize_t length = read(fd, buffer.data(), buffer.size());
            if (length <= 0) continue;

            for (ssize_t offset = 0; offset < length;) {
                auto* event = reinterpret_cast<inotify_event*>(buffer.data() + offset);
                if (event->len > 0) {
                    if (configPath.filename() == event->name) {
                        logger.updateSettings(configFile);
                    }
                }
                offset += sizeof(inotify_event) + event->len;
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
#endif
    }
};

#endif // FILE_WATCHER_HPP
