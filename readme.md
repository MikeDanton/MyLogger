# myLogger - High-Performance Asynchronous Logger for C++

🚀 **myLogger** is an **asynchronous, multi-backend logging system** designed for **high-performance applications**. It supports **console and file logging**, **hot-reloading configuration**, **log level filtering**, and **real-time file watching**.

---

## 📌 Features

✔ **Asynchronous Logging** - Background thread processing for minimal overhead.  
✔ **Multiple Backends** - Console and file logging with easy extensibility.  
✔ **Hot-Reloadable Configuration** - Changes to `logger.conf` apply in real-time.  
✔ **Log Level Filtering** - Supports custom loglevel filtering with severity order.  
✔ **Thread-Safe** - Utilizes `std::mutex` and atomic variables for concurrency.  
✔ **Timestamped Log Entries** - Automatic timestamp generation for log messages.  
✔ **Real-Time File Watching** - Uses `inotify` (Linux) for detecting config changes.  
✔ **Minimal Setup** - Automatically creates `logger.conf` if missing.  
✔ **Benchmarking & Performance Tests** - Built-in Google Benchmark integration.

---

## 📚 Public API

### **1️⃣ Basic Logging**
```cpp
#include "logger.hpp"

int main() {
    Logger& logger = Logger::getInstance();
    logger.init();
    
    logger.log("INFO", "GENERAL", "Application started");
    return 0;
}
```

### **2️⃣ Configuration Reloading**
Automatically reloads `logger.conf` when modified.
```cpp
#include "file_watcher.hpp"

std::atomic<bool> exitFlag{false};
std::thread configWatcher(FileWatcher::watch, std::ref(logger), "config/logger.conf", std::ref(exitFlag));
```

### **3️⃣ Log Levels**
```cpp
logger.log("INFO", "GENERAL", "This is an info message");
logger.log("ERROR", "DATABASE", "Database connection failed");
```

---

## 📊 Benchmarking
Built-in performance testing with **Google Benchmark**:
```sh
mkdir build && cd build
cmake ..
make
./myLoggerBenchmark
```

### **Benchmark Results**
#### Logging Performance:
```
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
BM_LoggingPerformance       1200 ns         1100 ns       500000
```

#### Config File Loading Performance:
```
--------------------------------------------------------
Benchmark              Time             CPU   Iterations
--------------------------------------------------------
BM_LoadConfig       3200 ns         3100 ns       250000
```

---

## 🚀 Running the Example Program
```sh
mkdir build && cd build
cmake ..
make
./myLoggerApp
```

---

## 📌 Next Steps
✅ **[ ] Implement more log backends (network, database, etc.)**  
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
    class LogBackend
    class ConsoleBackend
    class FileBackend

    Logger --> LogBackend
    LogBackend <|-- ConsoleBackend
    LogBackend <|-- FileBackend
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
🔥 **Contributions Welcome!** 🚀

