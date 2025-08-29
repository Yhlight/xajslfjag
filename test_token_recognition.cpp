#include "src/CHTL/CHTLLexer/Lexer.h"
#include <iostream>

int main() {
    using namespace CHTL;
    
    // 测试基础关键字识别
    String testCode = R"(
        // 基础语法测试
        text { "Hello World" }
        
        [Template] @Style MyStyle { color: red; }
        [Custom] @Element MyElement { }
        [Origin] @Html { <div></div> }
        [Import] @Chtl from module.chtl
        [Import] @CJmod from extension.cjmod
        [Configuration] @Config MyConfig { }
        
        // CHTL JS语法测试
        script {
            module { load: ./test.cjjs }
            vir handler = listen { click: function() {} }
            animate { duration: 300 }
            delegate { '.btn': { click: handler } }
        }
    )";
    
    try {
        Lexer lexer(testCode);
        
        std::cout << "Token Recognition Test:\n";
        std::cout << "======================\n";
        
        while (lexer.hasMoreTokens()) {
            Token token = lexer.nextToken();
            if (token.type == TokenType::EOF_TOKEN) break;
            
            // 跳过空白字符和换行
            if (token.type == TokenType::WHITESPACE || token.type == TokenType::NEWLINE) {
                continue;
            }
            
            std::cout << "Type: " << Token::tokenTypeToString(token.type) 
                      << ", Value: \"" << token.value << "\"\n";
        }
        
        if (lexer.hasErrors()) {
            std::cout << "\nErrors found:\n";
            for (const auto& error : lexer.getErrors()) {
                std::cout << "Error: " << error << "\n";
            }
        } else {
            std::cout << "\nNo errors found - Token recognition successful!\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}