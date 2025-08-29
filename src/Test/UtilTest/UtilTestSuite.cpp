#include "UtilTestSuite.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>

namespace CHTL::Test {

// TestCaseResult实现
TestCaseResult::TestCaseResult(const String& name, TestResult res, const String& msg,
                              double time, const String& f, int l)
    : testName(name), result(res), message(msg), executionTime(time), file(f), line(l) {
}

// TestSuiteResult实现
TestSuiteResult::TestSuiteResult(const String& name)
    : suiteName(name), totalTests(0), passedTests(0), failedTests(0), 
      skippedTests(0), errorTests(0), totalTime(0.0) {
}

void TestSuiteResult::addTestCase(const TestCaseResult& testCase) {
    testCases.push_back(testCase);
    totalTests++;
    totalTime += testCase.executionTime;
    
    switch (testCase.result) {
        case TestResult::PASSED: passedTests++; break;
        case TestResult::FAILED: failedTests++; break;
        case TestResult::SKIPPED: skippedTests++; break;
        case TestResult::ERROR: errorTests++; break;
    }
}

double TestSuiteResult::getSuccessRate() const {
    return totalTests > 0 ? (double)passedTests / totalTests * 100.0 : 0.0;
}

String TestSuiteResult::getSummary() const {
    std::ostringstream oss;
    oss << "Test Suite: " << suiteName << "\n";
    oss << "Total: " << totalTests << ", ";
    oss << "Passed: " << passedTests << ", ";
    oss << "Failed: " << failedTests << ", ";
    oss << "Skipped: " << skippedTests << ", ";
    oss << "Error: " << errorTests << "\n";
    oss << "Success Rate: " << std::fixed << std::setprecision(2) << getSuccessRate() << "%\n";
    oss << "Total Time: " << std::fixed << std::setprecision(3) << totalTime << "ms";
    return oss.str();
}

// AssertionFailedException实现
AssertionFailedException::AssertionFailedException(const String& message, const String& file, int line)
    : message_(message), file_(file), line_(line) {
}

const char* AssertionFailedException::what() const noexcept {
    if (whatMessage_.empty()) {
        std::ostringstream oss;
        oss << "Assertion failed: " << message_;
        if (!file_.empty() && line_ > 0) {
            oss << " at " << file_ << ":" << line_;
        }
        whatMessage_ = oss.str();
    }
    return whatMessage_.c_str();
}

const String& AssertionFailedException::getMessage() const {
    return message_;
}

const String& AssertionFailedException::getFile() const {
    return file_;
}

int AssertionFailedException::getLine() const {
    return line_;
}

// UnitTest实现
UnitTest::UnitTest(const String& testName) 
    : testName_(testName), skipped_(false) {
}

TestCaseResult UnitTest::execute() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    try {
        setUp();
        
        if (skipped_) {
            return TestCaseResult(testName_, TestResult::SKIPPED, skipReason_);
        }
        
        runTest();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        tearDown();
        
        return TestCaseResult(testName_, TestResult::PASSED, "Test passed", duration);
        
    } catch (const AssertionFailedException& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        tearDown();
        
        return TestCaseResult(testName_, TestResult::FAILED, e.getMessage(), duration, e.getFile(), e.getLine());
        
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        tearDown();
        
        return TestCaseResult(testName_, TestResult::ERROR, String("Exception: ") + e.what(), duration);
        
    } catch (...) {
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        tearDown();
        
        return TestCaseResult(testName_, TestResult::ERROR, "Unknown exception", duration);
    }
}

const String& UnitTest::getTestName() const {
    return testName_;
}

void UnitTest::skip(const String& reason) {
    skipped_ = true;
    skipReason_ = reason;
}

void UnitTest::fail(const String& message) {
    throw AssertionFailedException(message, "", 0);
}

// TestSuite实现
TestSuite::TestSuite(const String& suiteName)
    : suiteName_(suiteName), verbose_(false), stopOnFirstFailure_(false), timeoutSeconds_(0) {
}

void TestSuite::addTest(std::unique_ptr<UnitTest> test) {
    tests_.push_back(std::move(test));
}

TestSuiteResult TestSuite::runAll() {
    TestSuiteResult result(suiteName_);
    
    for (auto& test : tests_) {
        if (!shouldRunTest(test->getTestName())) {
            continue;
        }
        
        if (verbose_) {
            std::cout << "Running test: " << test->getTestName() << "..." << std::endl;
        }
        
        TestCaseResult testResult = test->execute();
        result.addTestCase(testResult);
        
        if (verbose_) {
            String status = (testResult.result == TestResult::PASSED) ? "PASSED" :
                           (testResult.result == TestResult::FAILED) ? "FAILED" :
                           (testResult.result == TestResult::SKIPPED) ? "SKIPPED" : "ERROR";
            
            std::cout << "  " << status;
            if (!testResult.message.empty()) {
                std::cout << ": " << testResult.message;
            }
            std::cout << " (" << std::fixed << std::setprecision(3) << testResult.executionTime << "ms)" << std::endl;
        }
        
        if (stopOnFirstFailure_ && testResult.result == TestResult::FAILED) {
            break;
        }
    }
    
    return result;
}

TestSuiteResult TestSuite::runTest(const String& testName) {
    TestSuiteResult result(suiteName_);
    
    for (auto& test : tests_) {
        if (test->getTestName() == testName) {
            TestCaseResult testResult = test->execute();
            result.addTestCase(testResult);
            break;
        }
    }
    
    return result;
}

TestSuiteResult TestSuite::runTests(const StringVector& testNames) {
    TestSuiteResult result(suiteName_);
    
    for (const auto& testName : testNames) {
        for (auto& test : tests_) {
            if (test->getTestName() == testName) {
                TestCaseResult testResult = test->execute();
                result.addTestCase(testResult);
                break;
            }
        }
    }
    
    return result;
}

void TestSuite::setTestFilter(const String& pattern) {
    testFilter_ = pattern;
}

void TestSuite::excludeTest(const String& testName) {
    excludedTests_.push_back(testName);
}

void TestSuite::includeTest(const String& testName) {
    includedTests_.push_back(testName);
}

void TestSuite::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void TestSuite::setStopOnFirstFailure(bool stop) {
    stopOnFirstFailure_ = stop;
}

void TestSuite::setTimeout(int timeoutSeconds) {
    timeoutSeconds_ = timeoutSeconds;
}

const String& TestSuite::getSuiteName() const {
    return suiteName_;
}

size_t TestSuite::getTestCount() const {
    return tests_.size();
}

StringVector TestSuite::getTestNames() const {
    StringVector names;
    for (const auto& test : tests_) {
        names.push_back(test->getTestName());
    }
    return names;
}

bool TestSuite::shouldRunTest(const String& testName) const {
    // 检查排除列表
    if (std::find(excludedTests_.begin(), excludedTests_.end(), testName) != excludedTests_.end()) {
        return false;
    }
    
    // 检查包含列表
    if (!includedTests_.empty()) {
        return std::find(includedTests_.begin(), includedTests_.end(), testName) != includedTests_.end();
    }
    
    // 检查过滤器
    if (!testFilter_.empty()) {
        return matchesFilter(testName);
    }
    
    return true;
}

bool TestSuite::matchesFilter(const String& testName) const {
    // 简单的通配符匹配
    if (testFilter_ == "*") {
        return true;
    }
    
    return testName.find(testFilter_) != String::npos;
}

// TestRunner实现
TestRunner& TestRunner::getInstance() {
    static TestRunner instance;
    return instance;
}

void TestRunner::addTestSuite(std::unique_ptr<TestSuite> suite) {
    testSuites_.push_back(std::move(suite));
}

std::vector<TestSuiteResult> TestRunner::runAllSuites() {
    results_.clear();
    
    for (auto& suite : testSuites_) {
        if (verbose_) {
            std::cout << "\n=== Running Test Suite: " << suite->getSuiteName() << " ===" << std::endl;
        }
        
        suite->setVerbose(verbose_);
        TestSuiteResult result = suite->runAll();
        results_.push_back(result);
        
        if (verbose_) {
            std::cout << result.getSummary() << std::endl;
        }
    }
    
    return results_;
}

TestSuiteResult TestRunner::runSuite(const String& suiteName) {
    for (auto& suite : testSuites_) {
        if (suite->getSuiteName() == suiteName) {
            suite->setVerbose(verbose_);
            return suite->runAll();
        }
    }
    
    return TestSuiteResult("NotFound");
}

std::vector<TestSuiteResult> TestRunner::runSuites(const StringVector& suiteNames) {
    std::vector<TestSuiteResult> results;
    
    for (const auto& suiteName : suiteNames) {
        TestSuiteResult result = runSuite(suiteName);
        results.push_back(result);
    }
    
    return results;
}

void TestRunner::setOutputFormat(const String& format) {
    outputFormat_ = format;
}

void TestRunner::setOutputFile(const String& filePath) {
    outputFile_ = filePath;
}

void TestRunner::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void TestRunner::setParallel(bool parallel) {
    parallel_ = parallel;
}

void TestRunner::setMaxThreads(int maxThreads) {
    maxThreads_ = maxThreads;
}

void TestRunner::generateReport() {
    if (outputFormat_ == "xml") {
        writeXMLReport();
    } else if (outputFormat_ == "json") {
        writeJSONReport();
    } else {
        writeConsoleReport();
    }
}

String TestRunner::getOverallSummary() const {
    size_t totalTests = 0, totalPassed = 0, totalFailed = 0, totalSkipped = 0, totalError = 0;
    double totalTime = 0.0;
    
    for (const auto& result : results_) {
        totalTests += result.totalTests;
        totalPassed += result.passedTests;
        totalFailed += result.failedTests;
        totalSkipped += result.skippedTests;
        totalError += result.errorTests;
        totalTime += result.totalTime;
    }
    
    std::ostringstream oss;
    oss << "\n=== Overall Test Summary ===\n";
    oss << "Total Test Suites: " << results_.size() << "\n";
    oss << "Total Tests: " << totalTests << "\n";
    oss << "Passed: " << totalPassed << "\n";
    oss << "Failed: " << totalFailed << "\n";
    oss << "Skipped: " << totalSkipped << "\n";
    oss << "Error: " << totalError << "\n";
    oss << "Success Rate: " << std::fixed << std::setprecision(2) 
        << (totalTests > 0 ? (double)totalPassed / totalTests * 100.0 : 0.0) << "%\n";
    oss << "Total Time: " << std::fixed << std::setprecision(3) << totalTime << "ms";
    
    return oss.str();
}

bool TestRunner::hasFailures() const {
    for (const auto& result : results_) {
        if (result.failedTests > 0 || result.errorTests > 0) {
            return true;
        }
    }
    return false;
}

void TestRunner::writeConsoleReport() const {
    std::cout << getOverallSummary() << std::endl;
}

void TestRunner::writeXMLReport() const {
    // XML报告实现（基础版本）
    std::cout << "XML report generation not yet implemented" << std::endl;
}

void TestRunner::writeJSONReport() const {
    // JSON报告实现（基础版本）
    std::cout << "JSON report generation not yet implemented" << std::endl;
}

} // namespace CHTL::Test