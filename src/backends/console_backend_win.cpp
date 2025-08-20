#include "myLogger/backends/console_backend.hpp"
#include "myLogger/logger_config.hpp"
#include <windows.h>
#include <io.h>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <string>
#include <cassert>

using std::string;

namespace {
    static inline string Trim(std::string_view s) {
        size_t b = 0, e = s.size();
        while (b < e && (unsigned char)s[b] <= ' ') ++b;
        while (e > b && (unsigned char)s[e - 1] <= ' ') --e;
        return string{s.substr(b, e - b)};
    }
    static inline bool ParseHexRGB(const string& hex, int& r, int& g, int& b) {
        if (hex.size() != 7 && hex.size() != 9) return false;
        if (hex[0] != '#') return false;
        auto hx = [](char c)->int {
            if (c>='0'&&c<='9') return c-'0';
            c = (char)std::tolower((unsigned char)c);
            if (c>='a'&&c<='f') return 10 + (c-'a');
            return -1;
        };
        int v[8] = {0};
        for (size_t i = 1; i < hex.size(); ++i) {
            int h = hx(hex[i]); if (h < 0) return false; v[i-1] = h;
        }
        r = v[0]*16 + v[1];
        g = v[2]*16 + v[3];
        b = v[4]*16 + v[5];
        return true;
    }
    static inline string Ansi24(const string& hex) {
        int r,g,b; if (!ParseHexRGB(hex, r, g, b)) return {};
        char buf[64];
        std::snprintf(buf, sizeof(buf), "\x1b[38;2;%d;%d;%dm", r, g, b);
        return string(buf);
    }
    static inline string BasicAnsiFromLevel(const string& lvl) {
        if (lvl == "INFO") return "\x1b[32m";
        if (lvl == "WARN") return "\x1b[33m";
        if (lvl == "ERROR" || lvl == "CRITICAL") return "\x1b[31m";
        if (lvl == "DEBUG" || lvl == "VERBOSE") return "\x1b[36m";
        return {};
    }
}

void ConsoleBackend::setup(const LoggerSettings& settings) {
    vtEnabled = false;
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD mode = 0;
        if (GetConsoleMode(hOut, &mode)) {
            if ((mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
                SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
            }
            vtEnabled = true;
        }
    }
    isTty = _isatty(_fileno(stdout)) != 0;
}

void ConsoleBackend::shutdown() {
}

void ConsoleBackend::flush() {
    std::cout << std::flush;
}

void ConsoleBackend::write(const LogMessage& logMsg, const LoggerSettings& settings) {
    const auto& cfg = settings.config;

    string line;
    if (cfg.format.enableTimestamps && !logMsg.timestamp.empty()) {
        line += logMsg.timestamp; line += " ";
    }
    if (!cfg.display.hideLevelTag) {
        line += "["; line += logMsg.level; line += "] ";
    }
    if (!cfg.display.hideContextTag) {
        line += logMsg.context; line += ": ";
    }
    line += logMsg.message;

    bool colorize = cfg.display.enableColors && vtEnabled;

    if (colorize) {
        string hex;
        if (cfg.colors.colorMode == "level") {
            auto it = cfg.levels.levelIndexMap.find(logMsg.level);
            if (it != cfg.levels.levelIndexMap.end() && it->second < MAX_LEVELS)
                hex = cfg.colors.logColorArray[it->second];
            if (hex.empty() || hex == "#FFFFFFFF") {
                auto pm = cfg.colors.parsedLogColors.find("level_" + logMsg.level);
                if (pm != cfg.colors.parsedLogColors.end()) hex = pm->second;
            }
        } else if (cfg.colors.colorMode == "context") {
            auto it = cfg.contexts.contextIndexMap.find(logMsg.context);
            if (it != cfg.contexts.contextIndexMap.end() && it->second < MAX_CONTEXTS)
                hex = cfg.colors.contextColorArray[it->second];
            if (hex.empty() || hex == "#FFFFFFFF") {
                auto pm = cfg.colors.parsedLogColors.find("context_" + logMsg.context);
                if (pm != cfg.colors.parsedLogColors.end()) hex = pm->second;
            }
        }

        string pre = Ansi24(Trim(hex));
        if (pre.empty()) pre = BasicAnsiFromLevel(logMsg.level);

        if (!pre.empty()) {
            std::cout << pre << line << "\x1b[0m\n";
        } else {
            std::cout << line << '\n';
        }
    } else {
        std::cout << line << '\n';
    }

    const string mode = Trim(cfg.general.flushMode);
    if (!(mode == "auto" || mode == "AUTO" || mode == "Auto")) std::cout << std::flush;
    assert(std::cout.good());
}
