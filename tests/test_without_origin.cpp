#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>

int main() {
    std::cout << "=== 基础CHTL测试（不含Origin） ===" << std::endl;
    
    // 1. 测试基础CHTL语法
    std::string source1 = "div { text { Hello World } }";
    std::cout << "\n测试1: " << source1 << std::endl;
    
    CHTL::CHTLLexer lexer1(source1);
    auto tokens1 = lexer1.tokenize();
    
    std::cout << "Token数量: " << tokens1.size() << std::endl;
    for (const auto& token : tokens1) {
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN) {
            std::cout << "  " << token.getTypeName() << ": \"" << token.value << "\"" << std::endl;
        }
    }
    
    // 2. 测试模板语法
    std::string source2 = "[Template] @Style MyStyle { color: blue; }";
    std::cout << "\n测试2: " << source2 << std::endl;
    
    CHTL::CHTLLexer lexer2(source2);
    auto tokens2 = lexer2.tokenize();
    
    std::cout << "Token数量: " << tokens2.size() << std::endl;
    for (const auto& token : tokens2) {
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN) {
            std::cout << "  " << token.getTypeName() << ": \"" << token.value << "\"" << std::endl;
        }
    }
    
    std::cout << "\n=== 基础测试完成 ===" << std::endl;
    return 0;
}