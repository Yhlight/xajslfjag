#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void debugCustomElementParsing() {
    std::cout << "\n=== 调试自定义元素解析 ===" << std::endl;
    
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
        
        std::cout << "关键词元信息:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "  [" << i << "] " << token.toString() << std::endl;
        }
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        std::cout << "\n解析结果:" << std::endl;
        std::cout << "  - 解析成功: " << (ast.success ? "是" : "否") << std::endl;
        std::cout << "  - 自定义节点数: " << ast.customNodes.size() << std::endl;
        std::cout << "  - 模板节点数: " << ast.templates.size() << std::endl;
        std::cout << "  - 根节点子节点数: " << (ast.rootNode ? ast.rootNode->getChildren().size() : 0) << std::endl;
        
        // 检查根节点的子节点类型
        if (ast.rootNode) {
            auto children = ast.rootNode->getChildren();
            for (size_t i = 0; i < children.size(); ++i) {
                auto child = children[i];
                std::cout << "  根节点子节点[" << i << "]: 类型=" << static_cast<int>(child->getNodeType()) 
                          << ", 名称='" << child->getName() << "'" << std::endl;
                          
                // 如果是自定义节点，检查其子节点
                std::cout << "    子节点数: " << child->getChildren().size() << std::endl;
                auto grandChildren = child->getChildren();
                for (size_t j = 0; j < grandChildren.size(); ++j) {
                    auto grandChild = grandChildren[j];
                    std::cout << "      子节点[" << j << "]: 类型=" << static_cast<int>(grandChild->getNodeType()) 
                              << ", 名称='" << grandChild->getName() << "'" << std::endl;
                }
            }
        }
        
        // 检查是否有错误报告
        if (errorReporter.hasErrors()) {
            std::cout << "\n❌ 解析过程中的错误:" << std::endl;
            errorReporter.printAll();
        } else {
            std::cout << "\n✓ 没有解析错误" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 调试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 自定义元素解析调试" << std::endl;
    std::cout << "=====================" << std::endl;
    
    debugCustomElementParsing();
    
    std::cout << "\n调试完成！" << std::endl;
    return 0;
}