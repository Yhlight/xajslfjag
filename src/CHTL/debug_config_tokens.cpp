#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

int main() {
    std::string configCode = R"(
[Configuration]
{
    INDEX_INITIAL_COUNT = 1;
    DEBUG_MODE = true;
}
)";
    
    std::cout << "分析配置代码的Token序列:" << std::endl;
    std::cout << configCode << std::endl;
    std::cout << "=========================" << std::endl;
    
    ErrorReporter errorReporter;
    Lexer lexer(configCode);
    auto tokens = lexer.tokenize();
    
    if (errorReporter.hasErrors()) {
        std::cout << "词法分析错误:" << std::endl;
        errorReporter.printAll();
        return 1;
    }
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << i << ": " << static_cast<int>(token.type) << " '" << token.value << "'" << std::endl;
    }
    
    return 0;
}