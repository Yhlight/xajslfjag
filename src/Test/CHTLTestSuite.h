#ifndef CHTL_TEST_SUITE_H
#define CHTL_TEST_SUITE_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <chrono>
#include <iostream>

namespace CHTL {
namespace Test {

// 测试结果
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime; // 毫秒
    std::string actualOutput;
    std::string expectedOutput;
};

// 测试用例基类
class TestCase {
public:
    TestCase(const std::string& name) : name_(name) {}
    virtual ~TestCase() = default;
    
    // 运行测试
    TestResult run();
    
    // 获取测试名称
    const std::string& getName() const { return name_; }
    
protected:
    // 子类需要实现的测试逻辑
    virtual void execute() = 0;
    
    // 断言方法
    void assertTrue(bool condition, const std::string& message = "");
    void assertFalse(bool condition, const std::string& message = "");
    void assertEqual(const std::string& actual, const std::string& expected);
    void assertNotEqual(const std::string& actual, const std::string& expected);
    void assertContains(const std::string& str, const std::string& substring);
    void assertNotContains(const std::string& str, const std::string& substring);
    void assertThrows(std::function<void()> func, const std::string& message = "");
    void assertNoThrow(std::function<void()> func, const std::string& message = "");
    
    // 设置实际输出和期望输出（用于详细报告）
    void setActualOutput(const std::string& output) { actualOutput_ = output; }
    void setExpectedOutput(const std::string& output) { expectedOutput_ = output; }

private:
    std::string name_;
    bool passed_ = true;
    std::string message_;
    std::string actualOutput_;
    std::string expectedOutput_;
    
    friend class TestRunner;
};

// 测试套件
class TestSuite {
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    // 添加测试用例
    void addTest(std::shared_ptr<TestCase> test);
    
    // 运行所有测试
    std::vector<TestResult> run();
    
    // 获取套件名称
    const std::string& getName() const { return name_; }
    
    // 获取测试数量
    size_t getTestCount() const { return tests_.size(); }

private:
    std::string name_;
    std::vector<std::shared_ptr<TestCase>> tests_;
};

// 测试运行器
class TestRunner {
public:
    static TestRunner& getInstance() {
        static TestRunner instance;
        return instance;
    }
    
    // 添加测试套件
    void addSuite(std::shared_ptr<TestSuite> suite);
    
    // 运行所有测试
    void runAll();
    
    // 运行特定套件
    void runSuite(const std::string& suiteName);
    
    // 运行特定测试
    void runTest(const std::string& suiteName, const std::string& testName);
    
    // 设置输出详细程度
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // 设置是否在失败时停止
    void setStopOnFailure(bool stop) { stopOnFailure_ = stop; }
    
    // 获取统计信息
    size_t getTotalTests() const { return totalTests_; }
    size_t getPassedTests() const { return passedTests_; }
    size_t getFailedTests() const { return failedTests_; }
    double getTotalExecutionTime() const { return totalExecutionTime_; }

private:
    TestRunner() = default;
    
    std::vector<std::shared_ptr<TestSuite>> suites_;
    bool verbose_ = false;
    bool stopOnFailure_ = false;
    
    // 统计信息
    size_t totalTests_ = 0;
    size_t passedTests_ = 0;
    size_t failedTests_ = 0;
    double totalExecutionTime_ = 0.0;
    
    // 输出测试结果
    void printResult(const TestResult& result);
    void printSummary();
};

// 测试宏
#define CHTL_TEST(SuiteName, TestName) \
    class Test_##SuiteName##_##TestName : public CHTL::Test::TestCase { \
    public: \
        Test_##SuiteName##_##TestName() : TestCase(#TestName) {} \
    protected: \
        void execute() override; \
    }; \
    void Test_##SuiteName##_##TestName::execute()

#define CHTL_TEST_SUITE(SuiteName) \
    static struct SuiteRegistrar_##SuiteName { \
        SuiteRegistrar_##SuiteName() { \
            auto suite = std::make_shared<CHTL::Test::TestSuite>(#SuiteName); \
            registerTests(suite); \
            CHTL::Test::TestRunner::getInstance().addSuite(suite); \
        } \
        void registerTests(std::shared_ptr<CHTL::Test::TestSuite> suite); \
    } suiteRegistrar_##SuiteName; \
    void SuiteRegistrar_##SuiteName::registerTests(std::shared_ptr<CHTL::Test::TestSuite> suite)

#define CHTL_ADD_TEST(SuiteName, TestName) \
    suite->addTest(std::make_shared<Test_##SuiteName##_##TestName>())

// 测试辅助函数
namespace TestUtil {
    // 编译并比较输出
    bool compileAndCompare(const std::string& chtlCode, 
                          const std::string& expectedHTML,
                          const std::string& expectedCSS = "",
                          const std::string& expectedJS = "");
    
    // 验证语法
    bool validateSyntax(const std::string& code, const std::string& type = "chtl");
    
    // 读取测试文件
    std::string readTestFile(const std::string& filename);
    
    // 写入测试结果
    void writeTestResult(const std::string& filename, const std::string& content);
    
    // 比较文件内容
    bool compareFiles(const std::string& file1, const std::string& file2);
    
    // 格式化代码（用于比较）
    std::string normalizeCode(const std::string& code);
}

} // namespace Test
} // namespace CHTL

#endif // CHTL_TEST_SUITE_H