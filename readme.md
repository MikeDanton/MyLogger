# myLogger - Asynchronous Multi-Backend Logger for C++

🚀 **myLogger** is a **high-performance, asynchronous logger** supporting **multiple backends (console, file)** with **log level filtering**.

---

## 📌 Features

✅ **Asynchronous Logging** - Logs run in a background thread.  
✅ **Multiple Backends** - Supports **Console + File Logging Simultaneously**.  
✅ **Log Level Filtering** - Set **INFO, WARN, or ERROR** log levels.  
✅ **Thread-Safe** - Uses **mutex and condition variables**.  
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
logger.addBackend(std::make_unique<FileBackend>("app_log.txt"));

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

### 4️⃣ File Logging
```cpp
logger.addBackend(std::make_unique<FileBackend>("log_2025-02-09.txt"));
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
- ✅ **[ ] Log Rotation (Auto-Delete Old Logs)**
- ✅ **[ ] Benchmark Performance with High Load**
- ✅ **[ ] Add a `LoggerManager` for Simplified Setup**

---

🛠 **Author**: @BoboBaggins  
👤 **License**: MIT

🔥 **Contributions Welcome!** 🚀

