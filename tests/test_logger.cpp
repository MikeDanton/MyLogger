#include "mock_backend.hpp"
#include "logger.hpp"
#include <gtest/gtest.h>

TEST(LoggerTest, LogsCorrectly) {
    MockBackend mockBackend;
    auto logger = createLogger(mockBackend);

    logger.log("INFO", "TEST_CONTEXT", "This is a test log.");

    EXPECT_EQ(mockBackend.logEntries.size(), 1);
    EXPECT_TRUE(mockBackend.contains("This is a test log."));
}

TEST(LoggerTest, RespectsLoggingLevels) {
    MockBackend mockBackend;
    auto logger = createLogger(mockBackend);

    logger.log("DEBUG", "TEST_CONTEXT", "This should not log");
    logger.log("ERROR", "TEST_CONTEXT", "This should log");

    EXPECT_TRUE(mockBackend.contains("This should log"));
    EXPECT_FALSE(mockBackend.contains("This should not log"));
}

TEST(LoggerTest, FlushClearsLogs) {
    MockBackend mockBackend;
    auto logger = createLogger(mockBackend);

    logger.log("INFO", "TEST_CONTEXT", "Message before flush");
    EXPECT_FALSE(mockBackend.logEntries.empty());

    logger.flush();
    EXPECT_TRUE(mockBackend.logEntries.empty());
}

