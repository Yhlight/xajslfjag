#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void testSimpleCustom() {
    std::cout << "\n=== 测试简单自定义节点解析 ===" << std::endl;
    
    std::string customCode = R"([Custom] @Element Button
{
    button
    {
        style
        {
            padding: 10px;
        }
    }
})";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(customCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success) {
            std::cout << "✓ 自定义节点语法解析成功" << std::endl;
            std::cout << "  - 自定义节点数: " << ast.customNodes.size() << std::endl;
            std::cout << "  - 模板节点数: " << ast.templates.size() << std::endl;
            std::cout << "  - 根节点子节点数: " << (ast.rootNode ? ast.rootNode->getChildren().size() : 0) << std::endl;
            
            if (ast.customNodes.size() > 0) {
                auto customPtr = ast.customNodes[0];
                std::cout << "  - 自定义节点名称: '" << customPtr->getName() << "'" << std::endl;
                std::cout << "  - 自定义节点子节点数: " << customPtr->getChildren().size() << std::endl;
            }
            
        } else {
            std::cout << "❌ 自定义节点语法解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 自定义节点测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 简单自定义节点测试" << std::endl;
    std::cout << "===================" << std::endl;
    
    testSimpleCustom();
    
    std::cout << "\n自定义节点测试完成！" << std::endl;
    return 0;
}