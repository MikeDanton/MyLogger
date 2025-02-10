#include "logger.hpp"
#include "logBackend.hpp"
#include <gtest/gtest.h>
#include <sstream>
#include <thread>

// Mock backend for testing
class TestBackend : public LogBackend {
public:
    void write(const LogMessage& message) override {  // ✅ Correct method signature
        std::cout << "TestBackend: " << message.content << std::endl;
    }
};

TEST(LoggerTest, LogMessages) {
    Logger logger;
    auto backend = std::make_unique<TestBackend>();
    TestBackend* rawBackend = backend.get();
    logger.setBackend(std::move(backend));

    logger.log(LogLevel::INFO, "Test Info Message");
    logger.log(LogLevel::ERROR, "Test Error Message");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));  // ✅ Allow logger time to process

    std::string output = rawBackend->logStream.str();
    EXPECT_TRUE(output.find("[INFO] Test Info Message") != std::string::npos);
    EXPECT_TRUE(output.find("[ERROR] Test Error Message") != std::string::npos);
}

// Run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
