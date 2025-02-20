#ifndef FILE_WATCHER_HPP
#define FILE_WATCHER_HPP

#include <string>
#include <atomic>
#include <filesystem>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/select.h>
#include <cstring>

template <typename LoggerType>
class FileWatcher {
public:
    static void watch(LoggerType& logger, const std::string& configFile, std::atomic<bool>& exitFlag) {
        int fd = inotify_init();
        if (fd < 0) {
            std::cerr << "[FileWatcher] Error: Failed to initialize inotify: " << strerror(errno) << "\n";
            return;
        }

        std::filesystem::path configPath = configFile;
        std::string dirToWatch = configPath.parent_path();

        int wd = inotify_add_watch(fd, dirToWatch.c_str(), IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE);
        if (wd < 0) {
            std::cerr << "[FileWatcher] Error: Failed to watch directory: " << dirToWatch << " - " << strerror(errno) << "\n";
            close(fd);
            return;
        }

        char buffer[sizeof(struct inotify_event) + NAME_MAX + 1];

        while (!exitFlag.load()) {
            fd_set rfds;
            struct timeval timeout;

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            timeout.tv_sec = 2;
            timeout.tv_usec = 0;

            int retval = select(fd + 1, &rfds, nullptr, nullptr, &timeout);
            if (retval == -1) {
                std::cerr << "[FileWatcher] Error: select() failed: " << strerror(errno) << "\n";
                break;
            } else if (retval == 0) {
                continue;
            }

            ssize_t length = read(fd, buffer, sizeof(buffer));
            if (length < 0 && errno != EINTR) {
                std::cerr << "[FileWatcher] Error: Read error: " << strerror(errno) << "\n";
                break;
            }

            for (char* ptr = buffer; ptr < buffer + length; ptr += sizeof(struct inotify_event) + reinterpret_cast<struct inotify_event*>(ptr)->len) {
                struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);

                if (event->len > 0 && configPath.filename() == event->name) {
                    logger.updateSettings(configFile);  // ✅ Notify Logger to reload settings
                }
            }
        }

        inotify_rm_watch(fd, wd);
        close(fd);
    }
};

#endif // FILE_WATCHER_HPP
