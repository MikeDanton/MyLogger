cmake_minimum_required(VERSION 3.16)

find_package(GTest REQUIRED)
enable_testing()

set(TEST_SOURCES test_logger.cpp)

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    add_executable(${test_name} ${test_file})
    target_link_libraries(${test_name} PRIVATE myLoggerLib GTest::GTest GTest::Main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()
