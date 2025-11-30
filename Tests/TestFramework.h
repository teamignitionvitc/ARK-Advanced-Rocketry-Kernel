/*
                                        ::
                                        ::
                                        ::
                                        ::
                                        ::
    ..    ..........    :.      ::      ::     .........  ..    ..........    ...      .        
    ::    ::            : .:.   ::     .::.       ::      ::    ::       :    :: :.    :        
    ::    ::   ..:::    :   .:. ::    ::::::      ::      ::    ::       :    ::   ::  :        
    ::    ::......::    :      :::    ::::::      ::      ::    ::.......:    ::     :::        
                                      ::::::                                                    
                                      :.::.:                                                    
                         .::::          ::          ::::.                                      
                       .::::::::.       ::       .:::::::::                                    
                       ::::::::::::....::::.....:::::::::::                                    
                        .:::::::::::::::::::::::::::::::::.        

                    © Copyright of Ignition <Software Department>
                        All rights reserved
*/
/**************************************************************************************************
* File: TestFramework.h
* Author: MuhammadRamzy
* Created On: 30/11/25
*
* \brief Lightweight unit testing framework for ARK.
* \details
*   - Provides macros for assertions and test registration.
*   - Manages test execution and reporting.
*
**************************************************************************************************/

#ifndef ARK_TEST_FRAMEWORK_H
#define ARK_TEST_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <cmath>

namespace ARK {
namespace Test {

class TestRegistry {
public:
    static TestRegistry& GetInstance() {
        static TestRegistry instance;
        return instance;
    }

    void RegisterTest(const std::string& name, std::function<void()> testFunc) {
        tests.push_back({name, testFunc});
    }

    int RunAllTests() {
        int passed = 0;
        int failed = 0;

        std::cout << "========================================" << std::endl;
        std::cout << "Running " << tests.size() << " tests..." << std::endl;
        std::cout << "========================================" << std::endl;

        for (const auto& test : tests) {
            std::cout << "[RUN] " << test.name << "... ";
            try {
                test.func();
                std::cout << "PASS" << std::endl;
                passed++;
            } catch (const std::exception& e) {
                std::cout << "FAIL: " << e.what() << std::endl;
                failed++;
            } catch (...) {
                std::cout << "FAIL: Unknown error" << std::endl;
                failed++;
            }
        }

        std::cout << "========================================" << std::endl;
        std::cout << "Tests Passed: " << passed << std::endl;
        std::cout << "Tests Failed: " << failed << std::endl;
        std::cout << "========================================" << std::endl;

        return failed;
    }

private:
    struct TestCase {
        std::string name;
        std::function<void()> func;
    };
    std::vector<TestCase> tests;
};

class TestException : public std::exception {
public:
    TestException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

#define TEST_CASE(name) \
    void name(); \
    struct name##_Register { \
        name##_Register() { \
            ARK::Test::TestRegistry::GetInstance().RegisterTest(#name, name); \
        } \
    } name##_register_instance; \
    void name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) throw ARK::Test::TestException("Assertion failed: " #condition);

#define ASSERT_FALSE(condition) \
    if (condition) throw ARK::Test::TestException("Assertion failed: " #condition);

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) throw ARK::Test::TestException("Assertion failed: " #expected " == " #actual);

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) \
    if (std::abs((expected) - (actual)) > (epsilon)) throw ARK::Test::TestException("Assertion failed: " #expected " ~= " #actual);

} // namespace Test
} // namespace ARK

#endif // ARK_TEST_FRAMEWORK_H
