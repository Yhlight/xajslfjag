#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>

int main() {
    std::cout << "=== Origin关键字测试 ===" << std::endl;
    
    std::string source = "[Origin] @Html RawContent { <div class=\"test\">Raw HTML</div> }";
    std::cout << "源码: " << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    for (const auto& token : tokens) {
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN) {
            std::cout << "  " << token.getTypeName() << ": \"";
            
            // 安全打印，限制长度
            std::string value = token.value;
            if (value.length() > 100) {
                value = value.substr(0, 100) + "...";
            }
            
            // 替换特殊字符以避免输出问题
            for (char& c : value) {
                if (c == '\n') c = '\\';
                if (c == '\r') c = '\\';
                if (c == '\t') c = '\\';
            }
            
            std::cout << value << "\"" << std::endl;
        }
    }
    
    if (lexer.hasErrors()) {
        std::cout << "❌ 词法分析错误:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 词法分析通过" << std::endl;
    }
    
    return 0;
}