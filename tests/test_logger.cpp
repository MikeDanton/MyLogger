#include "mock_backend.hpp"
#include "logger.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

class LoggerTestEnvironment : public ::testing::Environment {
public:
    static inline MockBackend mockBackend;
    static inline std::unique_ptr<Logger<MockBackend>> logger;

    void SetUp() override {
        std::cout << "[TestEnv] Setting up global logger" << std::endl;
        mockBackend.reset();
        logger = std::make_unique<Logger<MockBackend>>(std::make_shared<LoggerSettings>(), mockBackend);
    }

    void TearDown() override {
        std::cout << "[TestEnv] Calling shutdown..." << std::endl;
        logger->shutdown();
        std::cout << "[TestEnv] Shutdown completed." << std::endl;
    }
};

TEST(LoggerTest, LogsCorrectly) {
    auto& logger = *LoggerTestEnvironment::logger;
    auto& mockBackend = LoggerTestEnvironment::mockBackend;

    logger.log("INFO", "TEST_CONTEXT", "This is a test log.");

    // ✅ Allow time for asynchronous log processing
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    EXPECT_EQ(mockBackend.logEntries.size(), 1);
    EXPECT_TRUE(mockBackend.contains("This is a test log."));
}

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

// ✅ Move AddGlobalTestEnvironment inside main()
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);

    // ✅ Register the environment properly
    ::testing::AddGlobalTestEnvironment(new LoggerTestEnvironment());

    return RUN_ALL_TESTS();
}
