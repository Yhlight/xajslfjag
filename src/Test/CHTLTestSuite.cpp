#include "CHTLTestSuite.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"
#include "../CHTL/CHTLIOStream/CHTLFileSystem.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Test {

// TestCase Implementation

TestResult TestCase::run() {
    TestResult result;
    result.testName = name_;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        execute();
        result.passed = passed_;
        result.message = message_;
    } catch (const std::exception& e) {
        result.passed = false;
        result.message = "Exception: " + std::string(e.what());
    } catch (...) {
        result.passed = false;
        result.message = "Unknown exception";
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double, std::milli>(end - start).count();
    
    result.actualOutput = actualOutput_;
    result.expectedOutput = expectedOutput_;
    
    return result;
}

void TestCase::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        passed_ = false;
        message_ = message.empty() ? "Assertion failed: expected true" : message;
    }
}

void TestCase::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        passed_ = false;
        message_ = message.empty() ? "Assertion failed: expected false" : message;
    }
}

void TestCase::assertEqual(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        passed_ = false;
        std::stringstream ss;
        ss << "Assertion failed: values not equal\n";
        ss << "Expected: " << expected << "\n";
        ss << "Actual: " << actual;
        message_ = ss.str();
        actualOutput_ = actual;
        expectedOutput_ = expected;
    }
}

void TestCase::assertNotEqual(const std::string& actual, const std::string& expected) {
    if (actual == expected) {
        passed_ = false;
        message_ = "Assertion failed: values are equal (expected different)";
    }
}

void TestCase::assertContains(const std::string& str, const std::string& substring) {
    if (str.find(substring) == std::string::npos) {
        passed_ = false;
        std::stringstream ss;
        ss << "Assertion failed: string does not contain substring\n";
        ss << "String: " << str << "\n";
        ss << "Expected substring: " << substring;
        message_ = ss.str();
    }
}

void TestCase::assertNotContains(const std::string& str, const std::string& substring) {
    if (str.find(substring) != std::string::npos) {
        passed_ = false;
        std::stringstream ss;
        ss << "Assertion failed: string contains substring (expected not to)\n";
        ss << "String: " << str << "\n";
        ss << "Unexpected substring: " << substring;
        message_ = ss.str();
    }
}

void TestCase::assertThrows(std::function<void()> func, const std::string& message) {
    bool threw = false;
    try {
        func();
    } catch (...) {
        threw = true;
    }
    
    if (!threw) {
        passed_ = false;
        message_ = message.empty() ? "Expected exception but none was thrown" : message;
    }
}

void TestCase::assertNoThrow(std::function<void()> func, const std::string& message) {
    try {
        func();
    } catch (const std::exception& e) {
        passed_ = false;
        message_ = message.empty() ? 
            "Unexpected exception: " + std::string(e.what()) : 
            message + " (Exception: " + e.what() + ")";
    } catch (...) {
        passed_ = false;
        message_ = message.empty() ? "Unexpected exception" : message;
    }
}

// TestSuite Implementation

void TestSuite::addTest(std::shared_ptr<TestCase> test) {
    tests_.push_back(test);
}

std::vector<TestResult> TestSuite::run() {
    std::vector<TestResult> results;
    
    for (auto& test : tests_) {
        results.push_back(test->run());
    }
    
    return results;
}

// TestRunner Implementation

void TestRunner::addSuite(std::shared_ptr<TestSuite> suite) {
    suites_.push_back(suite);
}

void TestRunner::runAll() {
    totalTests_ = 0;
    passedTests_ = 0;
    failedTests_ = 0;
    totalExecutionTime_ = 0.0;
    
    std::cout << "\n========================================\n";
    std::cout << "Running CHTL Test Suite\n";
    std::cout << "========================================\n\n";
    
    for (auto& suite : suites_) {
        std::cout << "Running suite: " << suite->getName() << "\n";
        std::cout << "----------------------------------------\n";
        
        auto results = suite->run();
        
        for (const auto& result : results) {
            totalTests_++;
            totalExecutionTime_ += result.executionTime;
            
            if (result.passed) {
                passedTests_++;
            } else {
                failedTests_++;
            }
            
            printResult(result);
            
            if (!result.passed && stopOnFailure_) {
                std::cout << "\nStopping due to test failure.\n";
                printSummary();
                return;
            }
        }
        
        std::cout << "\n";
    }
    
    printSummary();
}

void TestRunner::runSuite(const std::string& suiteName) {
    auto it = std::find_if(suites_.begin(), suites_.end(),
        [&](const std::shared_ptr<TestSuite>& suite) {
            return suite->getName() == suiteName;
        });
    
    if (it != suites_.end()) {
        totalTests_ = 0;
        passedTests_ = 0;
        failedTests_ = 0;
        totalExecutionTime_ = 0.0;
        
        std::cout << "\nRunning suite: " << (*it)->getName() << "\n";
        std::cout << "----------------------------------------\n";
        
        auto results = (*it)->run();
        
        for (const auto& result : results) {
            totalTests_++;
            totalExecutionTime_ += result.executionTime;
            
            if (result.passed) {
                passedTests_++;
            } else {
                failedTests_++;
            }
            
            printResult(result);
        }
        
        printSummary();
    } else {
        std::cout << "Suite not found: " << suiteName << "\n";
    }
}

void TestRunner::runTest(const std::string& suiteName, const std::string& testName) {
    auto suiteIt = std::find_if(suites_.begin(), suites_.end(),
        [&](const std::shared_ptr<TestSuite>& suite) {
            return suite->getName() == suiteName;
        });
    
    if (suiteIt == suites_.end()) {
        std::cout << "Suite not found: " << suiteName << "\n";
        return;
    }
    
    // Run specific test (would need to modify TestSuite to support this)
    std::cout << "Running test: " << suiteName << "::" << testName << "\n";
    // Implementation would go here
}

void TestRunner::printResult(const TestResult& result) {
    if (result.passed) {
        std::cout << "[PASS] ";
    } else {
        std::cout << "[FAIL] ";
    }
    
    std::cout << std::setw(40) << std::left << result.testName;
    std::cout << " (" << std::fixed << std::setprecision(2) << result.executionTime << " ms)";
    
    if (!result.passed && verbose_) {
        std::cout << "\n       " << result.message;
        
        if (!result.actualOutput.empty() && !result.expectedOutput.empty()) {
            std::cout << "\n       Expected:\n" << result.expectedOutput;
            std::cout << "\n       Actual:\n" << result.actualOutput;
        }
    }
    
    std::cout << "\n";
}

void TestRunner::printSummary() {
    std::cout << "\n========================================\n";
    std::cout << "Test Summary\n";
    std::cout << "========================================\n";
    std::cout << "Total tests:    " << totalTests_ << "\n";
    std::cout << "Passed:         " << passedTests_ << "\n";
    std::cout << "Failed:         " << failedTests_ << "\n";
    std::cout << "Success rate:   " << std::fixed << std::setprecision(1) 
              << (totalTests_ > 0 ? (100.0 * passedTests_ / totalTests_) : 0.0) << "%\n";
    std::cout << "Total time:     " << std::fixed << std::setprecision(2) 
              << totalExecutionTime_ << " ms\n";
    std::cout << "========================================\n\n";
}

// TestUtil Implementation

namespace TestUtil {

bool compileAndCompare(const std::string& chtlCode, 
                      const std::string& expectedHTML,
                      const std::string& expectedCSS,
                      const std::string& expectedJS) {
    auto dispatcher = CompilerFactory::createDispatcher();
    
    CompileOptions options;
    options.prettify = true;
    dispatcher->setOptions(options);
    
    auto result = dispatcher->compileString(chtlCode);
    
    if (!result.success) {
        return false;
    }
    
    // Normalize and compare
    std::string normalizedActualHTML = normalizeCode(result.htmlOutput);
    std::string normalizedExpectedHTML = normalizeCode(expectedHTML);
    
    if (normalizedActualHTML != normalizedExpectedHTML) {
        return false;
    }
    
    if (!expectedCSS.empty()) {
        std::string normalizedActualCSS = normalizeCode(result.cssOutput);
        std::string normalizedExpectedCSS = normalizeCode(expectedCSS);
        
        if (normalizedActualCSS != normalizedExpectedCSS) {
            return false;
        }
    }
    
    if (!expectedJS.empty()) {
        std::string normalizedActualJS = normalizeCode(result.jsOutput);
        std::string normalizedExpectedJS = normalizeCode(expectedJS);
        
        if (normalizedActualJS != normalizedExpectedJS) {
            return false;
        }
    }
    
    return true;
}

bool validateSyntax(const std::string& code, const std::string& type) {
    auto dispatcher = CompilerFactory::createDispatcher();
    
    CompileOptions options;
    dispatcher->setOptions(options);
    
    auto result = dispatcher->compileString(code);
    
    return result.success && result.errors.empty();
}

std::string readTestFile(const std::string& filename) {
    auto content = File::readToString(filename);
    return content.value_or("");
}

void writeTestResult(const std::string& filename, const std::string& content) {
    File::writeString(filename, content);
}

bool compareFiles(const std::string& file1, const std::string& file2) {
    auto content1 = readTestFile(file1);
    auto content2 = readTestFile(file2);
    
    return normalizeCode(content1) == normalizeCode(content2);
}

std::string normalizeCode(const std::string& code) {
    std::string normalized = code;
    
    // Remove leading/trailing whitespace
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    // Normalize line endings
    std::regex crlf("\r\n");
    normalized = std::regex_replace(normalized, crlf, "\n");
    
    // Remove multiple consecutive blank lines
    std::regex multipleNewlines("\n\n+");
    normalized = std::regex_replace(normalized, multipleNewlines, "\n\n");
    
    // Normalize whitespace within lines
    std::regex multipleSpaces("[ \t]+");
    normalized = std::regex_replace(normalized, multipleSpaces, " ");
    
    return normalized;
}

} // namespace TestUtil

} // namespace Test
} // namespace CHTL