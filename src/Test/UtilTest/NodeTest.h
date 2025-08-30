#pragma once
#include "../../CHTL/CHTLNode/BaseNode.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/TemplateNode.h"
#include "../../CHTL/CHTLNode/OperatorNode.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

namespace CHTL {
namespace Test {

/**
 * 节点测试类
 * 提供各种节点的单元测试功能
 */
class NodeTest {
public:
    /**
     * 运行所有测试
     */
    static bool runAllTests();
    
    /**
     * 测试基础节点功能
     */
    static bool testBaseNode();
    
    /**
     * 测试元素节点功能
     */
    static bool testElementNode();
    
    /**
     * 测试文本节点功能
     */
    static bool testTextNode();
    
    /**
     * 测试模板节点功能
     */
    static bool testTemplateNode();
    
    /**
     * 测试操作节点功能
     */
    static bool testOperatorNode();
    
    /**
     * 测试节点工厂功能
     */
    static bool testNodeFactory();
    
    /**
     * 测试节点克隆功能
     */
    static bool testNodeCloning();
    
    /**
     * 测试节点验证功能
     */
    static bool testNodeValidation();
    
    /**
     * 测试节点序列化功能
     */
    static bool testNodeSerialization();

private:
    /**
     * 辅助测试方法
     */
    static bool assertTrue(bool condition, const std::string& message);
    static bool assertFalse(bool condition, const std::string& message);
    static bool assertEqual(const std::string& expected, const std::string& actual, const std::string& message);
    static bool assertNotNull(void* ptr, const std::string& message);
    
    /**
     * 打印测试结果
     */
    static void printTestResult(const std::string& testName, bool success);
};

/**
 * 错误报告器测试类
 */
class ErrorReporterTest {
public:
    /**
     * 运行所有错误报告器测试
     */
    static bool runAllTests();
    
    /**
     * 测试基础错误报告功能
     */
    static bool testBasicErrorReporting();
    
    /**
     * 测试错误统计功能
     */
    static bool testErrorStatistics();
    
    /**
     * 测试错误过滤功能
     */
    static bool testErrorFiltering();
    
    /**
     * 测试错误状态管理
     */
    static bool testErrorStateManagement();

private:
    static bool assertTrue(bool condition, const std::string& message);
    static void printTestResult(const std::string& testName, bool success);
};

/**
 * Token测试类
 */
class TokenTest {
public:
    /**
     * 运行所有Token测试
     */
    static bool runAllTests();
    
    /**
     * 测试Token基础功能
     */
    static bool testBasicTokenOperations();
    
    /**
     * 测试Token位置信息
     */
    static bool testTokenPosition();
    
    /**
     * 测试Token序列
     */
    static bool testTokenSequence();

private:
    static bool assertTrue(bool condition, const std::string& message);
    static bool assertEqual(const std::string& expected, const std::string& actual, const std::string& message);
    static void printTestResult(const std::string& testName, bool success);
};

/**
 * 性能测试类
 */
class PerformanceTest {
public:
    /**
     * 运行性能测试
     */
    static bool runPerformanceTests();
    
    /**
     * 测试节点创建性能
     */
    static bool testNodeCreationPerformance();
    
    /**
     * 测试节点查找性能
     */
    static bool testNodeSearchPerformance();
    
    /**
     * 测试AST构建性能
     */
    static bool testASTBuildPerformance();

private:
    static void printPerformanceResult(const std::string& testName, double timeMs, size_t operations);
};

} // namespace Test
} // namespace CHTL