#include "CHTLLexer/Lexer.h"
#include "CHTLState/State.h"
#include "CHTLContext/Context.h"
#include "ErrorReport.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testUpdatedLexer() {
    std::cout << "=== Testing Updated Lexer with External Components ===" << std::endl;
    
    // 创建外部组件
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter errorReporter;
    
    // 测试源代码
    std::string source = R"(
div
{
    id: "box";
    class: container;
    
    style
    {
        .active
        {
            color: red;
        }
    }
    
    text
    {
        "Hello World"
    }
}
)";
    
    // 创建Lexer，传入外部组件
    Lexer lexer(source, &stateMachine, &context, &errorReporter);
    
    // 测试基本功能
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    // 检查是否有错误
    if (errorReporter.hasErrors()) {
        std::cout << "Errors found:" << std::endl;
        errorReporter.printAll();
    } else {
        std::cout << "✓ No errors found" << std::endl;
    }
    
    // 验证一些重要的token
    bool foundDiv = false;
    bool foundClass = false;
    bool foundSelector = false;
    bool foundText = false;
    
    for (const auto& token : tokens) {
        switch (token.type) {
            case TokenType::HTML_ELEMENT:
                if (token.value == "div") foundDiv = true;
                break;
            case TokenType::IDENTIFIER:
                if (token.value == "class") foundClass = true;
                break;
            case TokenType::CLASS_SELECTOR:
                if (token.value == ".active") foundSelector = true;
                break;
            case TokenType::TEXT:
                foundText = true;
                break;
            default:
                break;
        }
    }
    
    assert(foundDiv);
    assert(foundClass);
    assert(foundSelector);
    assert(foundText);
    
    std::cout << "✓ All expected tokens found" << std::endl;
}

void testErrorReporting() {
    std::cout << "\n=== Testing Error Reporting System ===" << std::endl;
    
    ErrorReporter errorReporter;
    
    // 测试各种错误类型
    ErrorPosition pos("test.chtl", 1, 5, 4, 3);
    
    errorReporter.info("This is an info message", pos);
    errorReporter.warning("This is a warning", pos);
    errorReporter.error(ErrorType::SYNTAX_ERROR, "This is a syntax error", pos);
    errorReporter.fatal(ErrorType::INTERNAL_ERROR, "This is a fatal error", pos);
    
    // 检查统计
    ErrorStatistics stats = errorReporter.getStatistics();
    assert(stats.infoCount == 1);
    assert(stats.warningCount == 1);
    assert(stats.errorCount == 1);
    assert(stats.fatalCount == 1);
    assert(stats.totalCount() == 4);
    
    std::cout << "✓ Error statistics correct" << std::endl;
    
    // 测试输出
    std::cout << "Error output:" << std::endl;
    errorReporter.printAll();
    errorReporter.printSummary();
}

void testLexerWithErrorReporting() {
    std::cout << "\n=== Testing Lexer with Error Reporting ===" << std::endl;
    
    // 创建包含错误的源代码
    std::string source = R"(
div
{
    id: "unterminated string;
    class: valid;
}
)";
    
    ErrorReporter errorReporter;
    Lexer lexer(source, nullptr, nullptr, &errorReporter);
    
    // 分析token（应该会产生错误）
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    // 检查错误
    if (errorReporter.hasErrors()) {
        std::cout << "✓ Errors detected as expected:" << std::endl;
        errorReporter.printAll();
    } else {
        std::cout << "⚠ No errors found (unexpected)" << std::endl;
    }
}

int main() {
    std::cout << "CHTL Updated Lexer and Error System Test" << std::endl;
    std::cout << "=========================================" << std::endl;
    
    try {
        testUpdatedLexer();
        testErrorReporting();
        testLexerWithErrorReporting();
        
        std::cout << "\n=== All tests completed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}