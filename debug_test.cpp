#include "CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>

int main() {
    std::cout << "Testing CHTL Lexer..." << std::endl;
    
    std::string simple = "div { id: test; }";
    std::cout << "Input: " << simple << std::endl;
    
    try {
        CHTL::CHTLLexer lexer(simple);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens found: " << tokens.size() << std::endl;
        
        for (const auto& token : tokens) {
            std::cout << "Token: " << static_cast<int>(token.type) 
                      << " Value: '" << token.value << "'" << std::endl;
        }
        
        if (lexer.hasErrors()) {
            std::cout << "Lexer errors:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}