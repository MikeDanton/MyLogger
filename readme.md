# myLogger - Asynchronous Multi-Backend Logger for C++

🚀 **myLogger** is a **high-performance, asynchronous logger** supporting **multiple backends (console, file)** with **log level filtering**, **automatic log rotation**, and **timestamped log entries**.

---

## 📌 Features

✅ **Asynchronous Logging** - Logs run in a background thread.  
✅ **Multiple Backends** - Supports **Console + File Logging Simultaneously**.  
✅ **Log Level Filtering** - Set **INFO, WARN, ERROR, or DEBUG** log levels.  
✅ **Thread-Safe** - Uses **mutex and condition variables**.  
✅ **Timestamped Log Entries** - Every log message includes a precise timestamp.  
✅ **Timestamped Log Files** - Each session creates a new log file.  
✅ **Automatic Log Cleanup** - Removes logs older than a configurable threshold.  
✅ **LoggerManager for Simplified Setup** - Easy global logger access and configuration.  
✅ **Colored Console Output** - Logs appear in different colors for better readability.  
✅ **Easy Integration** - Single public include `#include "loggerLib.hpp"`.

---

## 📂 Project Structure
```
/myLogger/
│── include/
│   ├── loggerLib.hpp          <-- Public API (Single Include)
│   ├── logger.hpp             <-- Core Logger
│   ├── loggerManager.hpp      <-- LoggerManager for Simplified Setup
│   ├── logLevel.hpp           <-- Log Level Enums
│   ├── logBackend.hpp         <-- Backend Interface
│   ├── consoleBackend.hpp     <-- Console Logger
│   ├── fileBackend.hpp        <-- File Logger
│
│── src/
│   ├── logger.cpp
│   ├── loggerManager.cpp      <-- LoggerManager Implementation
│   ├── consoleBackend.cpp
│   ├── fileBackend.cpp
│
│── demo/                      <-- Example applications
│   ├── main.cpp               <-- Uses Logger & TaskManager
│   ├── taskManager.hpp        <-- Example task manager using logging
│   ├── taskManager.cpp
│
│── tests/
│   ├── test_logger.cpp
│
│── CMakeLists.txt
│── README.md
```

---

## 📚 Public Interface
### 1⃣ Logger Class
```cpp
#include "loggerLib.hpp"
Logger logger;
```
#### 📝 Methods
```cpp
void addBackend(std::unique_ptr<LogBackend> backend);
void setLogLevel(LogLevel level);
void log(LogLevel level, const std::string& message);
void flush();
```

#### 🛠 Example Usage
```cpp
Logger logger;
logger.addBackend(std::make_unique<ConsoleBackend>());
logger.addBackend(std::make_unique<FileBackend>());  // ✅ Uses timestamped filename

logger.setLogLevel(LogLevel::WARN);
logger.log(LogLevel::INFO, "This won't be logged due to filtering");
logger.log(LogLevel::ERROR, "Critical failure!");
```

---

### 2⃣ LoggerManager for Easy Setup
```cpp
#include "loggerManager.hpp"
LoggerManager::configure(true, true, LogLevel::INFO);
Logger& logger = LoggerManager::getInstance();
logger.log(LogLevel::INFO, "Application started");
```

---

### 3⃣ Available Log Levels
```cpp
enum class LogLevel { INFO, WARN, ERROR, DEBUG };
```
✅ **INFO** → Logs everything.  
✅ **WARN** → Logs `WARN` and `ERROR`.  
✅ **ERROR** → Logs only `ERROR`.  
✅ **DEBUG** → Most detailed logs for debugging purposes.

---

### 4⃣ Console Logging
```cpp
logger.addBackend(std::make_unique<ConsoleBackend>());
```

---

### 5⃣ File Logging (Timestamped Files)
```cpp
auto fileBackend = std::make_unique<FileBackend>();
std::cout << "Logging to file: " << fileBackend->getFilename() << "\n";
logger.addBackend(std::move(fileBackend));
```

---

### 6⃣ Automatic Log Cleanup
```cpp
fileBackend->cleanOldLogs(7);  // ✅ Deletes logs older than 7 days
```

---

### 7⃣ Timestamped Log Entries
Each log entry now includes a **timestamp** for precise tracking:
```cpp
[2025-02-09 14:32:10] [INFO] Starting automated simulation...
[2025-02-09 14:32:11] [INFO] Added task: Buy groceries
[2025-02-09 14:32:12] [INFO] Added task: Complete project
[2025-02-09 14:32:13] [INFO] Listing tasks:
[2025-02-09 14:32:13] [INFO] 1. Buy groceries
[2025-02-09 14:32:13] [INFO] 2. Complete project
[2025-02-09 14:32:15] [ERROR] Invalid task number: 10
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
- ✅ **[ ] Performance Benchmarking Under High Load**
- ✅ **[ ] Improve Log Rotation (Compress Old Logs)**
- ✅ **[ ] Expand LoggerManager for More Configurations**
- ✅ **[ ] Implement Colored Console Logs for Better Readability**

---

🛠 **Author**: @BoboBaggins  
👤 **License**: MIT

🔥 **Contributions Welcome!** 🚀

