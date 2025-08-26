#include "CHTL/CHTL.h"
#include <iostream>

int main() {
    std::cout << "CHTL Compiler Simple Test v" << CHTL::getVersion() << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Test basic lexing first
    std::string simple_chtl = R"(div {
    id: test;
    text {
        Hello World
    }
})";

    std::cout << "Testing simple HTML element:" << std::endl;
    std::cout << "Input CHTL:\n" << simple_chtl << std::endl;
    
    auto result = CHTL::compile(simple_chtl);
    
    if (result.success) {
        std::cout << "\n✓ Compilation successful!" << std::endl;
        std::cout << "\nGenerated HTML:" << std::endl;
        std::cout << result.html << std::endl;
        
        if (!result.css.empty()) {
            std::cout << "\nGenerated CSS:" << std::endl;
            std::cout << result.css << std::endl;
        }
        
        if (!result.js.empty()) {
            std::cout << "\nGenerated JS:" << std::endl;
            std::cout << result.js << std::endl;
        }
    } else {
        std::cout << "\n✗ Compilation failed!" << std::endl;
        std::cout << "Errors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    return 0;
}