# myLogger - Asynchronous Multi-Backend Logger for C++

🚀 **myLogger** is a **high-performance, asynchronous logger** supporting **multiple backends (console, file)** with **log level filtering**, **automatic log rotation**, and **timestamped log entries**.

---

## 📌 Features

👉 **Asynchronous Logging** - Logs run in a background thread.  
👉 **Multiple Backends** - Supports **Console + File Logging Simultaneously**.  
👉 **Log Level Filtering** - Set **INFO, WARN, ERROR, or DEBUG** log levels.  
👉 **Thread-Safe** - Uses **mutex and condition variables**.  
👉 **Timestamped Log Entries** - Every log message includes a precise timestamp.  
👉 **Timestamped Log Files** - Each session creates a new log file.  
👉 **Automatic Log Cleanup** - Removes logs older than a configurable threshold.  
👉 **LoggerManager for Simplified Setup** - Easy global logger access and configuration.  
👉 **Colored Console Output** - Logs appear in different colors for better readability.  
👉 **Benchmarking Support** - Performance tests for logging efficiency and config loading speed.  
👉 **Easy Integration** - Single public include `#include "loggerLib.hpp"`.

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
│── tests/                     <-- Unit tests for correctness
│   ├── test_logger.cpp
│
│── benchmarks/                <-- Performance benchmarks
│   ├── benchmark_logger.cpp   <-- Logging throughput, latency tests
│   ├── benchmark_config.cpp   <-- Config file loading performance
│
│── CMakeLists.txt
│── README.md
```

---

## 📚 Public Interface
### 1️⃣ Logger Class
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

### 2️⃣ LoggerManager for Easy Setup
```cpp
#include "loggerManager.hpp"
LoggerManager::configure(true, true, LogLevel::INFO);
Logger& logger = LoggerManager::getInstance();
logger.log(LogLevel::INFO, "Application started");
```

---

### 3️⃣ Available Log Levels
```cpp
enum class LogLevel { INFO, WARN, ERROR, DEBUG };
```
✅ **INFO** → Logs everything.  
✅ **WARN** → Logs `WARN` and `ERROR`.  
✅ **ERROR** → Logs only `ERROR`.  
✅ **DEBUG** → Most detailed logs for debugging purposes.

---

## 📊 Benchmarking
Performance testing using **Google Benchmark**:
```sh
mkdir build && cd build
cmake ..
make
./myLoggerBenchmark
```

### Benchmark Results
#### Logging Performance:
```
Benchmark                      Time             CPU   Iterations
----------------------------------------------------------------
BM_LoggingPerformance       2252 ns         2175 ns       337001
```
✅ **Optimized for high-speed logging with minimal overhead.**

#### Config File Loading Performance:
```
--------------------------------------------------------
Benchmark              Time             CPU   Iterations
--------------------------------------------------------
BM_LoadConfig       3823 ns         3805 ns       182955
```
✅ **Efficient config file parsing and automatic loading.**

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
- ✅ **[ ] Improve Log Rotation (Compress Old Logs)**
- ✅ **[ ] Expand LoggerManager for More Configurations**
- ✅ **[ ] Further optimize logging throughput and reduce latency**

---

🤦 **Author**: @BoboBaggins  
👤 **License**: MIT

🔥 **Contributions Welcome!** 🚀

