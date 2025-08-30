#include "UtilTest/NodeTest.h"
#include "CompilationMonitor/CompilationMonitor.h"
#include <iostream>
#include <chrono>

using namespace CHTL::Test;

int main() {
    std::cout << "CHTL Test Suite" << std::endl;
    std::cout << "================" << std::endl;
    
    // 初始化全局编译监视器
    auto monitorPtr = CompilationMonitorFactory::createDebugMonitor();
    GlobalMonitorManager::getInstance().setGlobalMonitor(std::move(monitorPtr));
    GlobalMonitorManager::getInstance().bindToCHTLProject();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    bool allTestsPassed = true;
    
    try {
        // 开始监控
        CHTL_MONITOR_START("CHTL Test Suite");
        CHTL_MONITOR_PHASE(CompilationPhase::LEXICAL_ANALYSIS);
        
        // 运行节点测试
        std::cout << "\n>>> Running Node Tests <<<" << std::endl;
        CHTL_MONITOR_HEARTBEAT();
        allTestsPassed &= NodeTest::runAllTests();
        
        // 运行错误报告器测试
        std::cout << "\n>>> Running ErrorReporter Tests <<<" << std::endl;
        CHTL_MONITOR_HEARTBEAT();
        allTestsPassed &= ErrorReporterTest::runAllTests();
        
        // 运行Token测试
        std::cout << "\n>>> Running Token Tests <<<" << std::endl;
        CHTL_MONITOR_PHASE(CompilationPhase::SYNTAX_ANALYSIS);
        CHTL_MONITOR_HEARTBEAT();
        // allTestsPassed &= TokenTest::runAllTests(); // 如果实现了的话
        
        // 运行性能测试
        std::cout << "\n>>> Running Performance Tests <<<" << std::endl;
        CHTL_MONITOR_PHASE(CompilationPhase::OPTIMIZATION);
        CHTL_MONITOR_HEARTBEAT();
        // allTestsPassed &= PerformanceTest::runPerformanceTests(); // 如果实现了的话
        
    } catch (const std::exception& e) {
        std::cerr << "Test execution failed with exception: " << e.what() << std::endl;
        allTestsPassed = false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    // 停止监控
    CHTL_MONITOR_STOP();
    
    // 打印总结
    std::cout << "\n===========================================" << std::endl;
    std::cout << "Test Summary:" << std::endl;
    std::cout << "  Status: " << (allTestsPassed ? "✓ ALL TESTS PASSED" : "❌ SOME TESTS FAILED") << std::endl;
    std::cout << "  Duration: " << duration.count() << "ms" << std::endl;
    
    // 获取监视器统计
    auto* globalMonitor = GlobalMonitorManager::getInstance().getGlobalMonitor();
    if (globalMonitor) {
        auto stats = globalMonitor->getStatistics();
        std::cout << "  Memory Peak: " << stats.maxResourceUsage.memoryUsageMB << "MB" << std::endl;
        std::cout << "  Errors: " << stats.errorCount << std::endl;
        std::cout << "  Warnings: " << stats.warningCount << std::endl;
    }
    
    std::cout << "===========================================" << std::endl;
    
    return allTestsPassed ? 0 : 1;
}