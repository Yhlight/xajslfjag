#include "../CHTLTestSuite.h"
#include "../../Error/ErrorReport.h"
#include <sstream>

using namespace CHTL;
using namespace CHTL::Test;

CHTL_TEST_SUITE(ErrorReport) {
    CHTL_ADD_TEST(ErrorReport, BasicReporting);
    CHTL_ADD_TEST(ErrorReport, ErrorLevels);
    CHTL_ADD_TEST(ErrorReport, ErrorBuilder);
    CHTL_ADD_TEST(ErrorReport, ErrorCollector);
    CHTL_ADD_TEST(ErrorReport, ErrorContext);
    CHTL_ADD_TEST(ErrorReport, JSONReporter);
}

CHTL_TEST(ErrorReport, BasicReporting) {
    auto& report = ErrorReport::getInstance();
    report.removeAllReporters();
    
    auto collector = std::make_shared<ErrorCollector>();
    report.addReporter(collector);
    
    // 测试基本报告
    report.error("Test error");
    report.warning("Test warning");
    report.info("Test info");
    
    assertEqual(collector->getErrorCount(), 1);
    assertEqual(collector->getWarningCount(), 1);
    assertEqual(collector->getErrors().size(), 3);
    
    // 清理
    report.removeAllReporters();
    report.resetCounters();
}

CHTL_TEST(ErrorReport, ErrorLevels) {
    auto& report = ErrorReport::getInstance();
    report.removeAllReporters();
    
    auto collector = std::make_shared<ErrorCollector>();
    report.addReporter(collector);
    
    // 测试不同级别
    report.debug("Debug message");
    report.info("Info message");
    report.warning("Warning message");
    report.error("Error message");
    
    auto errors = collector->getErrorsByLevel(ErrorLevel::ERROR);
    assertEqual(errors.size(), 1);
    
    auto warnings = collector->getErrorsByLevel(ErrorLevel::WARNING);
    assertEqual(warnings.size(), 1);
    
    // 清理
    report.removeAllReporters();
    report.resetCounters();
}

CHTL_TEST(ErrorReport, ErrorBuilder) {
    auto& report = ErrorReport::getInstance();
    report.removeAllReporters();
    
    auto collector = std::make_shared<ErrorCollector>();
    report.addReporter(collector);
    
    // 使用ErrorBuilder
    ErrorBuilder(ErrorLevel::ERROR, ErrorType::SYNTAX_ERROR)
        .withMessage("Invalid syntax")
        .withDetail("Unexpected token ';'")
        .atLocation("test.chtl", 10, 5)
        .withSuggestion("Remove the semicolon")
        .report();
    
    auto errors = collector->getErrors();
    assertEqual(errors.size(), 1);
    
    auto& error = errors[0];
    assertEqual(error.message, "Invalid syntax");
    assertEqual(error.detail, "Unexpected token ';'");
    assertEqual(error.location.filename, "test.chtl");
    assertEqual(error.location.line, 10);
    assertEqual(error.location.column, 5);
    assertEqual(error.suggestions.size(), 1);
    assertEqual(error.suggestions[0], "Remove the semicolon");
    
    // 清理
    report.removeAllReporters();
    report.resetCounters();
}

CHTL_TEST(ErrorReport, ErrorCollector) {
    ErrorCollector collector;
    
    // 创建不同类型的错误
    ErrorInfo error1;
    error1.level = ErrorLevel::ERROR;
    error1.type = ErrorType::SYNTAX_ERROR;
    error1.message = "Syntax error";
    collector.report(error1);
    
    ErrorInfo error2;
    error2.level = ErrorLevel::WARNING;
    error2.type = ErrorType::SEMANTIC_ERROR;
    error2.message = "Semantic warning";
    collector.report(error2);
    
    ErrorInfo error3;
    error3.level = ErrorLevel::FATAL;
    error3.type = ErrorType::INTERNAL_ERROR;
    error3.message = "Fatal error";
    collector.report(error3);
    
    // 测试统计
    assertEqual(collector.getErrorCount(), 2);  // ERROR + FATAL
    assertEqual(collector.getWarningCount(), 1);
    assertTrue(collector.hasErrors());
    assertTrue(collector.hasFatalErrors());
    
    // 测试按类型获取
    auto syntaxErrors = collector.getErrorsByType(ErrorType::SYNTAX_ERROR);
    assertEqual(syntaxErrors.size(), 1);
    
    // 清理
    collector.clear();
    assertEqual(collector.getErrors().size(), 0);
    assertFalse(collector.hasErrors());
}

CHTL_TEST(ErrorReport, ErrorContext) {
    auto& report = ErrorReport::getInstance();
    report.removeAllReporters();
    
    auto collector = std::make_shared<ErrorCollector>();
    report.addReporter(collector);
    
    // 设置上下文
    report.pushContext("Parsing", "ElementNode");
    report.error("Error in context");
    
    auto errors = collector->getErrors();
    assertEqual(errors.size(), 1);
    assertEqual(errors[0].context.phase, "Parsing");
    assertEqual(errors[0].context.component, "ElementNode");
    
    // 测试上下文守卫
    {
        ErrorContextGuard guard("Lexing", "StringLiteral");
        report.error("Error in guarded context");
    }
    
    assertEqual(collector->getErrors().size(), 2);
    assertEqual(collector->getErrors()[1].context.phase, "Lexing");
    
    report.popContext();
    
    // 清理
    report.removeAllReporters();
    report.resetCounters();
}

CHTL_TEST(ErrorReport, JSONReporter) {
    JSONErrorReporter reporter;
    
    ErrorInfo error;
    error.level = ErrorLevel::ERROR;
    error.type = ErrorType::SYNTAX_ERROR;
    error.code = "E001";
    error.message = "Test error";
    error.location.filename = "test.chtl";
    error.location.line = 5;
    error.location.column = 10;
    error.suggestions.push_back("Fix the syntax");
    
    reporter.report(error);
    reporter.flush();
    
    std::string json = reporter.getJSON();
    
    // 基本验证JSON格式
    assertTrue(json.find("\"errors\"") != std::string::npos);
    assertTrue(json.find("\"level\": \"error\"") != std::string::npos);
    assertTrue(json.find("\"type\": \"syntax\"") != std::string::npos);
    assertTrue(json.find("\"code\": \"E001\"") != std::string::npos);
    assertTrue(json.find("\"message\": \"Test error\"") != std::string::npos);
    assertTrue(json.find("\"file\": \"test.chtl\"") != std::string::npos);
    assertTrue(json.find("\"line\": 5") != std::string::npos);
    assertTrue(json.find("\"column\": 10") != std::string::npos);
    assertTrue(json.find("\"suggestions\"") != std::string::npos);
}