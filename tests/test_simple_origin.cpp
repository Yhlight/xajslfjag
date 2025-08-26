#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>

int main() {
    std::cout << "=== 简单原始嵌入测试 ===" << std::endl;
    
    std::string source = "[Origin] @Html test { content }";
    std::cout << "源码: " << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    
    std::cout << "开始词法分析..." << std::endl;
    auto tokens = lexer.tokenize();
    std::cout << "词法分析完成" << std::endl;
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN) {
            std::cout << "Token " << i << ": " << token.getTypeName() << std::endl;
            std::cout << "  值: \"" << token.value.substr(0, 50);
            if (token.value.length() > 50) std::cout << "...";
            std::cout << "\"" << std::endl;
        }
    }
    
    std::cout << "=== 测试完成 ===" << std::endl;
    return 0;
}