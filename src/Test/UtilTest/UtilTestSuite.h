#pragma once

#include "../../Util/Common.h"
#include <functional>
#include <vector>
#include <chrono>

namespace CHTL::Test {

// 测试结果枚举
enum class TestResult {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR
};

// 单个测试用例结果
struct TestCaseResult {
    String testName;
    TestResult result;
    String message;
    double executionTime; // 毫秒
    String file;
    int line;
    
    TestCaseResult(const String& name, TestResult res, const String& msg = "",
                  double time = 0.0, const String& f = "", int l = 0);
};

// 测试套件结果
struct TestSuiteResult {
    String suiteName;
    std::vector<TestCaseResult> testCases;
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    size_t skippedTests;
    size_t errorTests;
    double totalTime;
    
    TestSuiteResult(const String& name);
    void addTestCase(const TestCaseResult& testCase);
    double getSuccessRate() const;
    String getSummary() const;
};

// 测试用例函数类型
using TestFunction = std::function<void()>;

// 测试断言宏
#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #condition " to be true", __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #condition " to be false", __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #expected " == " #actual, __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_NE(expected, actual) \
    do { \
        if ((expected) == (actual)) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #expected " != " #actual, __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_STREQ(expected, actual) \
    do { \
        if (String(expected) != String(actual)) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: \"" + String(expected) + "\" == \"" + String(actual) + "\"", \
                __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_THROW(statement, exception_type) \
    do { \
        bool threw = false; \
        try { \
            statement; \
        } catch (const exception_type&) { \
            threw = true; \
        } catch (...) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #statement " to throw " #exception_type, __FILE__, __LINE__); \
        } \
        if (!threw) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #statement " to throw " #exception_type, __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_NO_THROW(statement) \
    do { \
        try { \
            statement; \
        } catch (...) { \
            throw CHTL::Test::AssertionFailedException( \
                "Expected: " #statement " not to throw", __FILE__, __LINE__); \
        } \
    } while(0)

// 断言失败异常
class AssertionFailedException : public std::exception {
public:
    AssertionFailedException(const String& message, const String& file, int line);
    
    const char* what() const noexcept override;
    const String& getMessage() const;
    const String& getFile() const;
    int getLine() const;
    
private:
    String message_;
    String file_;
    int line_;
    mutable String whatMessage_;
};

// 单元测试基类
class UnitTest {
public:
    explicit UnitTest(const String& testName);
    virtual ~UnitTest() = default;
    
    // 测试生命周期
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void runTest() = 0;
    
    // 执行测试
    TestCaseResult execute();
    
    const String& getTestName() const;
    
protected:
    // 辅助方法
    void skip(const String& reason = "");
    void fail(const String& message = "Test failed");
    
private:
    String testName_;
    bool skipped_;
    String skipReason_;
};

// 测试套件类
class TestSuite {
public:
    explicit TestSuite(const String& suiteName);
    virtual ~TestSuite() = default;
    
    // 添加测试
    void addTest(std::unique_ptr<UnitTest> test);
    template<typename TestClass>
    void addTest(const String& testName);
    
    // 执行测试
    TestSuiteResult runAll();
    TestSuiteResult runTest(const String& testName);
    TestSuiteResult runTests(const StringVector& testNames);
    
    // 测试过滤
    void setTestFilter(const String& pattern);
    void excludeTest(const String& testName);
    void includeTest(const String& testName);
    
    // 配置选项
    void setVerbose(bool verbose);
    void setStopOnFirstFailure(bool stop);
    void setTimeout(int timeoutSeconds);
    
    const String& getSuiteName() const;
    size_t getTestCount() const;
    StringVector getTestNames() const;
    
private:
    String suiteName_;
    std::vector<std::unique_ptr<UnitTest>> tests_;
    String testFilter_;
    StringVector excludedTests_;
    StringVector includedTests_;
    bool verbose_;
    bool stopOnFirstFailure_;
    int timeoutSeconds_;
    
    bool shouldRunTest(const String& testName) const;
    bool matchesFilter(const String& testName) const;
};

// 测试运行器
class TestRunner {
public:
    static TestRunner& getInstance();
    
    // 添加测试套件
    void addTestSuite(std::unique_ptr<TestSuite> suite);
    template<typename SuiteClass>
    void addTestSuite(const String& suiteName);
    
    // 运行测试
    std::vector<TestSuiteResult> runAllSuites();
    TestSuiteResult runSuite(const String& suiteName);
    std::vector<TestSuiteResult> runSuites(const StringVector& suiteNames);
    
    // 配置
    void setOutputFormat(const String& format); // "console", "xml", "json"
    void setOutputFile(const String& filePath);
    void setVerbose(bool verbose);
    void setParallel(bool parallel);
    void setMaxThreads(int maxThreads);
    
    // 结果处理
    void generateReport();
    String getOverallSummary() const;
    bool hasFailures() const;
    
    // 发现测试
    void discoverTests(const String& directory);
    void registerTestSuite(const String& name, std::function<std::unique_ptr<TestSuite>()> factory);
    
private:
    TestRunner() = default;
    ~TestRunner() = default;
    
    std::vector<std::unique_ptr<TestSuite>> testSuites_;
    std::vector<TestSuiteResult> results_;
    String outputFormat_;
    String outputFile_;
    bool verbose_;
    bool parallel_;
    int maxThreads_;
    
    void writeConsoleReport() const;
    void writeXMLReport() const;
    void writeJSONReport() const;
};

// 便捷宏
#define CHTL_TEST_SUITE(suiteName) \
    class suiteName##_TestSuite : public CHTL::Test::TestSuite { \
    public: \
        suiteName##_TestSuite() : TestSuite(#suiteName) { \
            initializeTests(); \
        } \
    private: \
        void initializeTests(); \
    }; \
    void suiteName##_TestSuite::initializeTests()

#define CHTL_TEST_CASE(testName) \
    class testName##_Test : public CHTL::Test::UnitTest { \
    public: \
        testName##_Test() : UnitTest(#testName) {} \
        void runTest() override; \
    }; \
    addTest(std::make_unique<testName##_Test>()); \
    } \
    void testName##_Test::runTest()

#define CHTL_RUN_TEST_SUITE(suiteName) \
    do { \
        auto suite = std::make_unique<suiteName##_TestSuite>(); \
        CHTL::Test::TestRunner::getInstance().addTestSuite(std::move(suite)); \
    } while(0)

} // namespace CHTL::Test