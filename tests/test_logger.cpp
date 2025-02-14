#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "logger.hpp"
#include "logger_core.hpp"
#include "backends/console_backend.hpp"
#include "backends/file_backend.hpp"

// ✅ Mock Backend for Testing (Captures Log Messages Instead of Printing)
class MockBackend {
public:
    std::vector<std::string> logs;

    void setup(const LoggerSettings&) {}

    void write(const LogMessage& log, const LoggerSettings&) {
        std::ostringstream oss;
        oss << log.timestamp << " [" << log.level << "] " << log.context << ": " << log.message;
        logs.push_back(oss.str());
    }

    void flush() {}
};

class LoggerTest : public ::testing::Test {
protected:
    std::shared_ptr<LoggerSettings> settings;
    MockBackend mockBackend;
    LoggerBackends<MockBackend> backends;
    Logger<LoggerBackends<MockBackend>> logger;

    LoggerTest()
        : settings(std::make_shared<LoggerSettings>()),
          backends(mockBackend), // ✅ Wrap mockBackend inside LoggerBackends
          logger(settings, backends) {} // ✅ Now matches expected constructor arguments

    void SetUp() override {
        std::ofstream logFile("test_log.txt", std::ios::trunc);
        settings->config.backends.enableConsole = false;
        settings->config.backends.enableFile = false;
    }
};

// ✅ Test: Log Message Captures Correctly
TEST_F(LoggerTest, LogMessageCapturedCorrectly) {
    logger.log("INFO", "TEST", "This is a test log.");
    ASSERT_FALSE(mockBackend.logs.empty());
    EXPECT_NE(mockBackend.logs[0].find("INFO"), std::string::npos);
    EXPECT_NE(mockBackend.logs[0].find("TEST"), std::string::npos);
    EXPECT_NE(mockBackend.logs[0].find("This is a test log."), std::string::npos);
}

// ✅ Test: Log Filtering by Level (DEBUG Should Not Appear)
TEST_F(LoggerTest, LogFilteringByLevel) {
    settings->config.levels.enabledArray[2] = false; // Disable DEBUG
    logger.log("DEBUG", "TEST", "This should not be logged.");
    EXPECT_TRUE(mockBackend.logs.empty()); // Should not log DEBUG messages

    logger.log("INFO", "TEST", "This should be logged.");
    EXPECT_FALSE(mockBackend.logs.empty()); // INFO should log
}

// ✅ Test: File Logging Writes Correctly
TEST_F(LoggerTest, FileLoggingWorks) {
    settings->config.backends.enableFile = true;
    FileBackend fileBackend;
    fileBackend.setup(*settings);

    logger.log("INFO", "FILE_TEST", "Testing file logging.");

    std::ifstream logFile("log.txt");
    std::string logContent((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    logFile.close();

    EXPECT_NE(logContent.find("INFO"), std::string::npos);
    EXPECT_NE(logContent.find("FILE_TEST"), std::string::npos);
    EXPECT_NE(logContent.find("Testing file logging."), std::string::npos);
}

// ✅ Test: Console Logging Captures Output
TEST_F(LoggerTest, ConsoleLoggingCapturesOutput) {
    settings->config.backends.enableConsole = true;
    ConsoleBackend consoleBackend;

    // Redirect std::cout
    std::stringstream capturedOutput;
    std::streambuf* oldCout = std::cout.rdbuf(capturedOutput.rdbuf());

    logger.log("INFO", "CONSOLE_TEST", "Testing console output.");

    // Restore std::cout
    std::cout.rdbuf(oldCout);

    std::string output = capturedOutput.str();
    EXPECT_NE(output.find("INFO"), std::string::npos);
    EXPECT_NE(output.find("CONSOLE_TEST"), std::string::npos);
    EXPECT_NE(output.find("Testing console output."), std::string::npos);
}

// ✅ Test: Log File Rotation (Simulating Old Logs Deletion)
TEST_F(LoggerTest, LogFileRotation) {
    settings->config.general.logRotationDays = 1; // Set very low for testing

    // Simulate old log file creation
    std::ofstream oldLogFile("logs/log_2000-01-01_00-00-00.txt");
    oldLogFile.close();

    logger.log("INFO", "ROTATION_TEST", "Checking rotation.");

    // Check if old log file is deleted
    EXPECT_FALSE(std::filesystem::exists("logs/log_2000-01-01_00-00-00.txt"));
}

