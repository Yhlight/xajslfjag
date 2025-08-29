#include "CHTLTestSuite.h"
#include <iostream>
#include <cstring>

// 声明外部测试套件（由链接器提供）
extern void registerAllTests();

int main(int argc, char* argv[]) {
    using namespace CHTL::Test;
    
    auto& runner = TestRunner::getInstance();
    
    // 解析命令行参数
    bool verbose = false;
    bool stopOnFailure = false;
    std::string suiteName;
    std::string testName;
    
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stop-on-failure") == 0) {
            stopOnFailure = true;
        }
        else if (strcmp(argv[i], "--suite") == 0 && i + 1 < argc) {
            suiteName = argv[++i];
        }
        else if (strcmp(argv[i], "--test") == 0 && i + 1 < argc) {
            testName = argv[++i];
        }
        else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            std::cout << "CHTL Test Runner\n";
            std::cout << "Usage: " << argv[0] << " [options]\n";
            std::cout << "Options:\n";
            std::cout << "  -v, --verbose          Show detailed test output\n";
            std::cout << "  -s, --stop-on-failure  Stop execution on first failure\n";
            std::cout << "  --suite <name>         Run only specified test suite\n";
            std::cout << "  --test <name>          Run only specified test\n";
            std::cout << "  -h, --help             Show this help message\n";
            return 0;
        }
    }
    
    // 配置测试运行器
    runner.setVerbose(verbose);
    runner.setStopOnFailure(stopOnFailure);
    
    // 运行测试
    if (!suiteName.empty()) {
        if (!testName.empty()) {
            runner.runTest(suiteName, testName);
        } else {
            runner.runSuite(suiteName);
        }
    } else {
        runner.runAll();
    }
    
    // 返回状态码
    return runner.getFailedTests() > 0 ? 1 : 0;
}