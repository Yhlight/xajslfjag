#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include <iostream>

int main() {
    std::cout << "=== 简单Parser测试 ===" << std::endl;
    
    std::string source = "use html5;";
    std::cout << "测试源码: " << source << std::endl;
    
    try {
        // 词法分析
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "Token数量: " << tokens.size() << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "Token[" << i << "]: " << token.getTypeName() << " = \"" << token.value << "\"" << std::endl;
        }
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 词法分析错误" << std::endl;
            return 1;
        }
        
        std::cout << "✅ 词法分析成功" << std::endl;
        
        // 语法分析
        std::cout << "开始语法分析..." << std::endl;
        CHTL::CHTLParser parser(tokens);
        
        std::cout << "调用parseDocument..." << std::endl;
        auto document = parser.parseDocument();
        
        std::cout << "parseDocument完成" << std::endl;
        
        if (parser.hasErrors()) {
            std::cout << "❌ 语法分析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        } else {
            std::cout << "✅ 语法分析成功" << std::endl;
        }
        
        if (document) {
            std::cout << "文档子项数量: " << document->children.size() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "❌ 未知异常" << std::endl;
        return 1;
    }
    
    std::cout << "测试完成" << std::endl;
    return 0;
}