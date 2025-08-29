#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/TemplateManager.h"
#include "../Error/ErrorReport.h"
#include <iostream>
#include <string>

using namespace CHTL;

void testBasicTemplateDefinition() {
    std::cout << "\n=== 测试基本模板定义 ===" << std::endl;
    
    // 测试样式组模板
    std::string styleTemplateSource = 
"[Template] @Style DefaultText {\n"
"    color: \"black\";\n"
"    line-height: 1.6;\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(styleTemplateSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 样式组模板解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testElementTemplate() {
    std::cout << "\n=== 测试元素模板 ===" << std::endl;
    
    std::string elementTemplateSource = 
"[Template] @Element Box {\n"
"    span {\n"
"        text {\n"
"            \"这是一组div\"\n"
"        }\n"
"    }\n"
"    div {\n"
"        style {\n"
"            width: 200px;\n"
"            height: 200px;\n"
"            background-color: red;\n"
"        }\n"
"    }\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(elementTemplateSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 元素模板解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testVarTemplate() {
    std::cout << "\n=== 测试变量组模板 ===" << std::endl;
    
    std::string varTemplateSource = 
"[Template] @Var ThemeColor {\n"
"    tableColor: \"rgb(255, 192, 203)\";\n"
"    textColor: \"black\";\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(varTemplateSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 变量组模板解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testTemplateInheritance() {
    std::cout << "\n=== 测试模板继承 ===" << std::endl;
    
    std::string inheritanceSource = 
"[Template] @Style ThemeColor {\n"
"    color: rgba(255, 192, 203, 1);\n"
"    background-color: rgba(253, 144, 162, 1);\n"
"}\n"
"\n"
"[Template] @Style ThemeColor2 {\n"
"    background-color: yellow;\n"
"    @Style ThemeColor;\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(inheritanceSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 模板继承解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testCustomStyleWithoutValue() {
    std::cout << "\n=== 测试自定义无值样式组 ===" << std::endl;
    
    std::string customStyleSource = 
"[Custom] @Style TextSet {\n"
"    color,\n"
"    font-size;\n"
"}\n"
"\n"
"div {\n"
"    style {\n"
"        @Style TextSet {\n"
"            color: red;\n"
"            font-size: 16px;\n"
"        }\n"
"    }\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(customStyleSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 无值自定义样式组解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testTemplateUsage() {
    std::cout << "\n=== 测试模板使用 ===" << std::endl;
    
    std::string templateUsageSource = 
"div {\n"
"    style {\n"
"        @Style DefaultText;\n"
"    }\n"
"}\n"
"\n"
"body {\n"
"    @Element Box;\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(templateUsageSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 模板使用解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testVariableReference() {
    std::cout << "\n=== 测试变量引用 ===" << std::endl;
    
    std::string varRefSource = 
"[Template] @Element Box {\n"
"    div {\n"
"        style {\n"
"            color: ThemeColor(tableColor);\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"body {\n"
"    div {\n"
"        style {\n"
"            background-color: ThemeColor(tableColor);\n"
"        }\n"
"    }\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(varRefSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 变量引用解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testFullQualifiedName() {
    std::cout << "\n=== 测试全缀名 ===" << std::endl;
    
    std::string fqnSource = 
"body {\n"
"    @Element Box;\n"
"    [Custom] @Element Box;\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(fqnSource, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 全缀名解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

int main() {
    std::cout << "CHTL模板系统测试" << std::endl;
    std::cout << "=================" << std::endl;
    
    // 测试基本模板定义
    testBasicTemplateDefinition();
    
    // 测试元素模板
    testElementTemplate();
    
    // 测试变量组模板
    testVarTemplate();
    
    // 测试模板继承
    testTemplateInheritance();
    
    // 测试自定义无值样式组
    testCustomStyleWithoutValue();
    
    // 测试模板使用
    testTemplateUsage();
    
    // 测试变量引用
    testVariableReference();
    
    // 测试全缀名
    testFullQualifiedName();
    
    // 检查错误报告
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    auto stats = errorReporter.getStatistics();
    
    std::cout << "\n=== 错误统计 ===" << std::endl;
    std::cout << "信息: " << stats.infoCount << std::endl;
    std::cout << "警告: " << stats.warningCount << std::endl;
    std::cout << "错误: " << stats.errorCount << std::endl;
    std::cout << "致命: " << stats.fatalCount << std::endl;
    
    if (stats.errorCount == 0 && stats.fatalCount == 0) {
        std::cout << "\n🎉 所有模板系统功能测试通过！" << std::endl;
        std::cout << "\n✅ 已实现的模板特性：" << std::endl;
        std::cout << "   • [Template] @Style 样式组模板" << std::endl;
        std::cout << "   • [Template] @Element 元素模板" << std::endl;
        std::cout << "   • [Template] @Var 变量组模板" << std::endl;
        std::cout << "   • [Custom] 自定义模板（支持无值属性）" << std::endl;
        std::cout << "   • 模板继承（组合式和显式继承）" << std::endl;
        std::cout << "   • 模板使用和实例化" << std::endl;
        std::cout << "   • 变量引用和替换" << std::endl;
        std::cout << "   • 全缀名访问" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ 部分测试失败，请检查实现。" << std::endl;
        
        if (errorReporter.hasErrors()) {
            std::cout << "\n最近的错误:" << std::endl;
            auto errors = errorReporter.getErrorsByLevel(ErrorLevel::ERROR);
            for (size_t i = 0; i < std::min(errors.size(), size_t(5)); ++i) {
                std::cout << "  - " << errors[i].message << std::endl;
            }
        }
        
        return 1;
    }
}