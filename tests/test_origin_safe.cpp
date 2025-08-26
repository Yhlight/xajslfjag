#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <string>

void testOriginSafe(const std::string& name, const std::string& source) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码长度: " << source.length() << std::endl;
    
    try {
        CHTL::CHTLLexer lexer(source);
        std::cout << "创建词法分析器成功" << std::endl;
        
        auto tokens = lexer.tokenize();
        std::cout << "词法分析完成，Token数量: " << tokens.size() << std::endl;
        
        size_t validTokens = 0;
        for (size_t i = 0; i < tokens.size() && i < 10; ++i) { // 限制只看前10个Token
            const auto& token = tokens[i];
            if (token.type != CHTL::CHTLTokenType::EOF_TOKEN && 
                token.type != CHTL::CHTLTokenType::WHITESPACE && 
                token.type != CHTL::CHTLTokenType::NEWLINE) {
                
                std::cout << "Token " << validTokens++ << ": " << token.getTypeName();
                
                // 安全打印值，限制长度
                if (token.value.length() > 50) {
                    std::cout << " (长度: " << token.value.length() << ")";
                } else {
                    std::cout << ": \"" << token.value << "\"";
                }
                std::cout << std::endl;
            }
        }
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 有错误" << std::endl;
        } else {
            std::cout << "✅ 无错误" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "❌ 未知异常" << std::endl;
    }
}

int main() {
    std::cout << "=== 安全原始嵌入测试 ===" << std::endl;
    
    // 1. 简单测试
    testOriginSafe("简单Origin", "[Origin] @Html test { content }");
    
    // 2. 带引号的测试
    testOriginSafe("带引号Origin", "[Origin] @Html test { <div class=\"test\">content</div> }");
    
    // 3. 多行测试
    testOriginSafe("多行Origin", 
        "[Origin] @Html test {\n"
        "  <div>content</div>\n"
        "}");
    
    // 4. 嵌套花括号测试
    testOriginSafe("嵌套花括号Origin", 
        "[Origin] @JavaScript test {\n"
        "  function test() { console.log('hello'); }\n"
        "}");
    
    std::cout << "\n=== 安全测试完成 ===" << std::endl;
    return 0;
}