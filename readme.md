# myLogger - High-Performance Asynchronous Logger for C++

🚀 **myLogger** is a **modular, dependency-injection-based asynchronous logging system** designed for **high-performance applications**. It supports **multiple logging backends**, **hot-reloading configuration**, **log level filtering**, and **real-time file watching** with `inotify` on Linux.

---

## 📌 Features

✔ **Asynchronous Logging** - Background thread processing for minimal overhead.  
✔ **Dependency Injection (DI) Design** - Easily plug-and-play different backends.  
✔ **Multiple Logging Backends** - Console, file logging, and future extensibility.  
✔ **Hot-Reloadable Configuration** - Updates `logger.conf` in real-time.  
✔ **Log Level Filtering** - Supports configurable log severity levels.  
✔ **Thread-Safe** - Utilizes `std::mutex` and atomic variables for concurrency.  
✔ **Timestamped Log Entries** - Supports multiple timestamp formats.  
✔ **Real-Time File Watching** - Uses `inotify` (Linux) for detecting config changes.  
✔ **Minimal Setup** - Automatically generates `logger.conf` if missing.  
✔ **Batch Processing for Efficiency** - Reduces locking contention.  
✔ **Google Benchmark Integration** - Built-in performance testing.

---

## 📚 Getting Started

### **1️⃣ Setting Up Logger with Multiple Backends**
```cpp
#include "my_logger.hpp"
#include "console_backend.hpp"
#include "file_backend.hpp"

int main() {
    auto settings = std::make_shared<LoggerSettings>();
    ConsoleBackend console;
    FileBackend file;
    
    Logger<ConsoleBackend, FileBackend> logger(settings, console, file);
    
    logger.log("INFO", "GENERAL", "Application started");
    return 0;
}
```

### **2️⃣ File Watching & Configuration Reloading**
To automatically reload `logger.conf` when modified:
```cpp
#include "file_watcher.hpp"

std::atomic<bool> exitFlag{false};
std::thread configWatcher(FileWatcher<Logger<ConsoleBackend, FileBackend>>::watch, std::ref(logger), "config/logger.conf", std::ref(exitFlag));
```

### **3️⃣ Log Levels and Contexts**
```cpp
logger.log("INFO", "GENERAL", "This is an info message");
logger.log("ERROR", "DATABASE", "Database connection failed");
```

## 🚀 Running the Example Program
```sh
mkdir build && cd build
cmake ..
make
./advancedLoggerDemo
```

---

## 📌 Next Steps
✅ **[ ] Implement additional backends (network, database, etc.)**  
✅ **[ ] Improve log rotation & compression**  
✅ **[ ] Optimize memory usage for high-throughput logging**  
✅ **[ ] Extend logging with JSON output for structured logs**

---

## 📚 Class Structure

### **1️⃣ Logger System Overview**
```mermaid
classDiagram
    class Logger {
        +log(level, context, message)
        +flush()
    }
    class LoggerCore
    class LogBackend
    class ConsoleBackend
    class FileBackend
    class FileWatcher

    Logger --> LoggerCore
    Logger --> LogBackend
    LogBackend <|-- ConsoleBackend
    LogBackend <|-- FileBackend
    Logger --> FileWatcher
```

### **2️⃣ File Watching & Hot Reloading**
```mermaid
classDiagram
    class FileWatcher {
        +watch(Logger, configFile, exitFlag)
    }
    Logger --> FileWatcher
```

---

👤 **Author**: @BoboBaggins  
📜 **License**: MIT

