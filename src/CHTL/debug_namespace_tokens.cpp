#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void debugNamespaceTokens() {
    std::cout << "\n=== 调试命名空间词法分析 ===" << std::endl;
    
    std::string namespaceCode = R"([Namespace] ui
{
    [Custom] @Element Button
    {
        button
        {
            style
            {
                padding: 10px;
            }
        }
    }
})";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(namespaceCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "生成的词元:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "  [" << i << "] " << token.toString() << std::endl;
        }
        
        if (errorReporter.hasErrors()) {
            std::cout << "\n❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
        } else {
            std::cout << "\n✓ 词法分析成功!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 调试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 命名空间词法调试" << std::endl;
    std::cout << "===================" << std::endl;
    
    debugNamespaceTokens();
    
    std::cout << "\n调试完成！" << std::endl;
    return 0;
}