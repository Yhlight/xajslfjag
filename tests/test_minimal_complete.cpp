#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include "CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>

int main() {
    std::cout << "=== 最小完整编译器测试 ===" << std::endl;
    
    std::string source = "div { text { Hello } }";
    std::cout << "源码: " << source << std::endl;
    
    try {
        // 词法分析
        std::cout << "1. 词法分析..." << std::endl;
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        std::cout << "Token数量: " << tokens.size() << std::endl;
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 词法分析错误" << std::endl;
            return 1;
        }
        
        // 语法分析
        std::cout << "2. 语法分析..." << std::endl;
        CHTL::CHTLParser parser(tokens);
        auto document = parser.parseDocument();
        std::cout << "AST节点数量: " << document->children.size() << std::endl;
        
        if (parser.hasErrors()) {
            std::cout << "❌ 语法分析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return 1;
        }
        
        // 代码生成
        std::cout << "3. 代码生成..." << std::endl;
        CHTL::CHTLGenerator generator;
        auto result = generator.generate(document);
        
        if (!result.success) {
            std::cout << "❌ 代码生成错误:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  " << error << std::endl;
            }
            return 1;
        }
        
        std::cout << "✅ 编译成功!" << std::endl;
        std::cout << "生成的HTML:\n" << result.html << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}