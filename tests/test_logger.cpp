#include "mock_backend.hpp"
#include "logger.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

//------------------------------------------------------------------------------
// ✅ Test Environment for Global Logger Setup
//------------------------------------------------------------------------------
class LoggerTestEnvironment : public ::testing::Environment {
public:
    static inline MockBackend mockBackend;
    static inline std::unique_ptr<Logger<MockBackend>> logger;

    void SetUp() override {
        std::cout << "[TestEnv] Setting up global logger\n";
        mockBackend.reset();

        // ✅ Create Logger with MockBackend
        logger = std::make_unique<Logger<MockBackend>>();
        logger->updateSettings("config/logger.conf");
    }

    void TearDown() override {
        std::cout << "[TestEnv] Calling shutdown...\n";
        logger->shutdown();
        std::cout << "[TestEnv] Shutdown completed.\n";
    }
};

//------------------------------------------------------------------------------
// ✅ Basic Logging Test
//------------------------------------------------------------------------------
TEST(LoggerTest, LogsCorrectly) {
    auto& logger = *LoggerTestEnvironment::logger;
    auto& mockBackend = LoggerTestEnvironment::mockBackend;

    logger.log("INFO", "TEST_CONTEXT", "This is a test log.");

    // ✅ Wait for the log to appear in mockBackend
    for (int i = 0; i < 10; ++i) {
        if (mockBackend.logEntries.size() > 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    EXPECT_EQ(mockBackend.logEntries.size(), 1);
    EXPECT_TRUE(mockBackend.contains("This is a test log."));
}

//------------------------------------------------------------------------------
// ✅ Log Level Filtering Test
//------------------------------------------------------------------------------
TEST(LoggerTest, RespectsLoggingLevels) {
    auto& logger = *LoggerTestEnvironment::logger;
    auto& mockBackend = LoggerTestEnvironment::mockBackend;

    logger.log("VERBOSE", "TEST_CONTEXT", "This should not log");
    logger.log("ERROR", "TEST_CONTEXT", "This should log");

    // ✅ Allow async logging to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_TRUE(mockBackend.contains("This should log"));
    EXPECT_FALSE(mockBackend.contains("This should not log"));
}

//------------------------------------------------------------------------------
// ✅ Logger Shutdown Test
//------------------------------------------------------------------------------
TEST(LoggerTest, ShutdownFlushesLogs) {
    auto& logger = *LoggerTestEnvironment::logger;
    auto& mockBackend = LoggerTestEnvironment::mockBackend;

    logger.log("INFO", "TEST_CONTEXT", "Message before shutdown");

    // ✅ Wait for logs to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_FALSE(mockBackend.logEntries.empty());

    logger.shutdown();  // ✅ Ensures logs are flushed

    EXPECT_TRUE(mockBackend.logEntries.empty());
}

//------------------------------------------------------------------------------
// ✅ Run GoogleTest with Global Logger Environment
//------------------------------------------------------------------------------
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // ✅ Register global test environment
    ::testing::AddGlobalTestEnvironment(new LoggerTestEnvironment());

    return RUN_ALL_TESTS();
}
