#include "CompilationMonitor/CompilationMonitor.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace CHTL::Test;

void simulateCompilationWork(CompilationMonitor& monitor) {
    std::cout << "Simulating compilation work..." << std::endl;
    
    // 模拟词法分析阶段
    monitor.setCurrentPhase(CompilationPhase::LEXICAL_ANALYSIS);
    for (int i = 0; i < 1000; ++i) {
        monitor.updateProgress(i, 1000);
        monitor.addTokenCount(10);
        monitor.heartbeat();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    // 模拟语法分析阶段
    monitor.setCurrentPhase(CompilationPhase::SYNTAX_ANALYSIS);
    for (int i = 0; i < 500; ++i) {
        monitor.updateProgress(i, 500);
        monitor.addNodeCount(5);
        monitor.heartbeat();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // 模拟语义分析阶段
    monitor.setCurrentPhase(CompilationPhase::SEMANTIC_ANALYSIS);
    for (int i = 0; i < 200; ++i) {
        monitor.updateProgress(i, 200);
        if (i % 50 == 0) {
            monitor.addWarningCount(1);
        }
        if (i % 100 == 0) {
            monitor.addErrorCount(1);
        }
        monitor.heartbeat();
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    // 模拟AST构建阶段
    monitor.setCurrentPhase(CompilationPhase::AST_BUILDING);
    for (int i = 0; i < 300; ++i) {
        monitor.updateProgress(i, 300);
        monitor.addNodeCount(3);
        monitor.heartbeat();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    
    std::cout << "\nCompilation work completed!" << std::endl;
}

void testBasicMonitoring() {
    std::cout << "=== Testing Basic Monitoring ===" << std::endl;
    
    auto monitor = CompilationMonitorFactory::createDebugMonitor();
    
    // 设置回调
    monitor->setPhaseChangeCallback([](CompilationPhase oldPhase, CompilationPhase newPhase) {
        std::cout << "\n[Callback] Phase changed from " 
                  << static_cast<int>(oldPhase) << " to " << static_cast<int>(newPhase) << std::endl;
    });
    
    monitor->setProgressCallback([](double progress) {
        if (static_cast<int>(progress * 100) % 10 == 0) {
            std::cout << "\n[Callback] Progress: " << (progress * 100) << "%" << std::endl;
        }
    });
    
    // 开始监控
    monitor->startMonitoring("Basic Test Project");
    
    // 模拟编译工作
    simulateCompilationWork(*monitor);
    
    // 停止监控
    monitor->stopMonitoring();
    
    // 打印报告
    std::cout << "\n" << monitor->generateReport() << std::endl;
}

void testTimeoutMonitoring() {
    std::cout << "\n=== Testing Timeout Monitoring ===" << std::endl;
    
    // 创建快速超时的监视器
    CompilationMonitor::MonitorConfig config;
    config.maxCompilationTime = std::chrono::milliseconds(3000); // 3秒超时
    config.enableAutoTermination = false; // 不自动终止，只报告
    
    auto monitor = CompilationMonitorFactory::createCustomMonitor(config);
    
    monitor->setTimeoutCallback([]() {
        std::cout << "\n[TIMEOUT] Compilation timeout detected!" << std::endl;
    });
    
    monitor->startMonitoring("Timeout Test Project");
    
    // 模拟长时间编译
    monitor->setCurrentPhase(CompilationPhase::LEXICAL_ANALYSIS);
    for (int i = 0; i < 50; ++i) {
        monitor->heartbeat();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "." << std::flush;
    }
    
    monitor->stopMonitoring();
    std::cout << "\nTimeout test completed." << std::endl;
}

void testDeadlockDetection() {
    std::cout << "\n=== Testing Deadlock Detection ===" << std::endl;
    
    auto monitor = CompilationMonitorFactory::createDebugMonitor();
    
    monitor->setDeadlockCallback([]() {
        std::cout << "\n[DEADLOCK] Potential deadlock detected!" << std::endl;
    });
    
    monitor->startMonitoring("Deadlock Test Project");
    
    // 模拟死锁（不发送心跳）
    monitor->setCurrentPhase(CompilationPhase::SYNTAX_ANALYSIS);
    std::cout << "Simulating deadlock (no heartbeat)..." << std::endl;
    
    // 等待死锁检测触发
    std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    
    monitor->stopMonitoring();
    std::cout << "Deadlock detection test completed." << std::endl;
}

void testResourceMonitoring() {
    std::cout << "\n=== Testing Resource Monitoring ===" << std::endl;
    
    // 创建低资源限制的监视器
    CompilationMonitor::MonitorConfig config;
    config.maxMemoryUsageMB = 100; // 很低的内存限制
    config.maxCpuUsagePercent = 50.0; // 50% CPU限制
    config.enableAutoTermination = false;
    
    auto monitor = CompilationMonitorFactory::createCustomMonitor(config);
    
    monitor->setResourceLimitCallback([](const ResourceUsage& usage) {
        std::cout << "\n[RESOURCE] Resource limit exceeded! Memory: " 
                  << usage.memoryUsageMB << "MB, CPU: " << usage.cpuUsagePercent << "%" << std::endl;
    });
    
    monitor->startMonitoring("Resource Test Project");
    
    // 模拟一些工作
    for (int i = 0; i < 100; ++i) {
        monitor->heartbeat();
        monitor->updateProgress(i, 100);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    monitor->stopMonitoring();
    
    auto stats = monitor->getStatistics();
    std::cout << "Resource monitoring completed. Max memory: " 
              << stats.maxResourceUsage.memoryUsageMB << "MB" << std::endl;
}

void testGlobalMonitorManager() {
    std::cout << "\n=== Testing Global Monitor Manager ===" << std::endl;
    
    // 绑定到CHTL项目
    GlobalMonitorManager::getInstance().bindToCHTLProject();
    
    // 使用宏进行监控
    CHTL_MONITOR_START("Global Manager Test");
    CHTL_MONITOR_PHASE(CompilationPhase::LEXICAL_ANALYSIS);
    
    for (int i = 0; i < 50; ++i) {
        CHTL_MONITOR_HEARTBEAT();
        CHTL_MONITOR_PROGRESS(i, 50);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    CHTL_MONITOR_PHASE(CompilationPhase::SYNTAX_ANALYSIS);
    
    for (int i = 0; i < 30; ++i) {
        CHTL_MONITOR_HEARTBEAT();
        CHTL_MONITOR_PROGRESS(i, 30);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    CHTL_MONITOR_STOP();
    
    auto* globalMonitor = GlobalMonitorManager::getInstance().getGlobalMonitor();
    if (globalMonitor) {
        std::cout << "\n" << globalMonitor->generateReport() << std::endl;
    }
}

int main() {
    std::cout << "CHTL Compilation Monitor Test Suite" << std::endl;
    std::cout << "====================================" << std::endl;
    
    try {
        testBasicMonitoring();
        testTimeoutMonitoring();
        testResourceMonitoring();
        testGlobalMonitorManager();
        
        // 注释掉死锁检测测试，因为它需要很长时间
        // testDeadlockDetection();
        
        std::cout << "\n=== All Monitor Tests Completed Successfully ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Monitor tests failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}