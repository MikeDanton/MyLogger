# myLogger - Asynchronous Multi-Backend Logger for C++

🚀 **myLogger** is a **high-performance, asynchronous logger** supporting **multiple backends (console, file)** with **log level filtering** and **automatic log rotation**.

---

## 📌 Features

✅ **Asynchronous Logging** - Logs run in a background thread.  
✅ **Multiple Backends** - Supports **Console + File Logging Simultaneously**.  
✅ **Log Level Filtering** - Set **INFO, WARN, or ERROR** log levels.  
✅ **Thread-Safe** - Uses **mutex and condition variables**.  
✅ **Timestamped Log Files** - Each session creates a new log file.  
✅ **Automatic Log Cleanup** - Removes logs older than a configurable threshold.  
✅ **Easy Integration** - Single public include `#include "loggerLib.hpp"`.

---

## 📂 Project Structure
```
/myLogger/
│── include/
│   ├── loggerLib.hpp          <-- Public API (Single Include)
│   ├── logger.hpp             <-- Core Logger
│   ├── logLevel.hpp           <-- Log Level Enums
│   ├── logBackend.hpp         <-- Backend Interface
│   ├── consoleBackend.hpp     <-- Console Logger
│   ├── fileBackend.hpp        <-- File Logger
│
│── src/
│   ├── logger.cpp
│   ├── consoleBackend.cpp
│   ├── fileBackend.cpp
│
│── tests/
│   ├── test_logger.cpp
│
│── CMakeLists.txt
│── main.cpp
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

### 2️⃣ Available Log Levels
```cpp
enum class LogLevel { INFO, WARN, ERROR };
```
✅ **INFO** → Logs everything.  
✅ **WARN** → Logs `WARN` and `ERROR`.  
✅ **ERROR** → Logs only `ERROR`.

---

### 3️⃣ Console Logging
```cpp
logger.addBackend(std::make_unique<ConsoleBackend>());
```

---

### 4️⃣ File Logging (Timestamped Files)
```cpp
auto fileBackend = std::make_unique<FileBackend>();
std::cout << "Logging to file: " << fileBackend->getFilename() << "\n";
logger.addBackend(std::move(fileBackend));
```

---

### 5️⃣ Automatic Log Cleanup
```cpp
fileBackend->cleanOldLogs(7);  // ✅ Deletes logs older than 7 days
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
- ✅ **[ ] Improve Log Formatting (Timestamps per Log Entry)**
- ✅ **[ ] Implement a `LoggerManager` for Simplified Setup**
- ✅ **[ ] Performance Benchmarking Under High Load**

---

🛠 **Author**: @BoboBaggins  
👤 **License**: MIT

🔥 **Contributions Welcome!** 🚀

