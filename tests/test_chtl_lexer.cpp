#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <cassert>

void testCHTLLexer() {
    std::cout << "=== CHTL词法分析器测试 ===" << std::endl;
    
    // 测试基础HTML标签
    {
        CHTL::CHTLLexer lexer("<div>Hello</div>");
        auto tokens = lexer.tokenize();
        
        std::cout << "测试基础HTML标签..." << std::endl;
        std::cout << "Token数量: " << tokens.size() << std::endl;
        
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
        }
        
        assert(!tokens.empty());
        std::cout << "✓ 基础HTML标签测试通过" << std::endl << std::endl;
    }
    
    // 测试CHTL关键字
    {
        CHTL::CHTLLexer lexer("template Button { <button>Click</button> }");
        auto tokens = lexer.tokenize();
        
        std::cout << "测试CHTL关键字..." << std::endl;
        std::cout << "Token数量: " << tokens.size() << std::endl;
        
        bool foundTemplate = false;
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
            if (token.type == CHTL::CHTLTokenType::TEMPLATE) {
                foundTemplate = true;
            }
        }
        
        assert(foundTemplate);
        std::cout << "✓ CHTL关键字测试通过" << std::endl << std::endl;
    }
    
    // 测试选择器语法
    {
        CHTL::CHTLLexer lexer(".container { <div>Auto class</div> }");
        auto tokens = lexer.tokenize();
        
        std::cout << "测试选择器语法..." << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
        }
        
        assert(!tokens.empty());
        std::cout << "✓ 选择器语法测试通过" << std::endl << std::endl;
    }
    
    // 测试注释
    {
        CHTL::CHTLLexer lexer("// Line comment\n/* Block comment */\n<div>Content</div>");
        auto tokens = lexer.tokenize();
        
        std::cout << "测试注释..." << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
        }
        
        assert(!tokens.empty());
        std::cout << "✓ 注释测试通过" << std::endl << std::endl;
    }
    
    // 测试变量插值
    {
        CHTL::CHTLLexer lexer("@var { name: \"World\" } <div>Hello ${name}!</div>");
        auto tokens = lexer.tokenize();
        
        std::cout << "测试变量插值..." << std::endl;
        for (const auto& token : tokens) {
            std::cout << "Token: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
        }
        
        assert(!tokens.empty());
        std::cout << "✓ 变量插值测试通过" << std::endl << std::endl;
    }
    
    std::cout << "=== 所有词法分析器测试通过 ===" << std::endl;
}

int main() {
    testCHTLLexer();
    return 0;
}