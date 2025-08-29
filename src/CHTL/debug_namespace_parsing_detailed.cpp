#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/NamespaceNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void debugDetailedNamespaceParsing() {
    std::cout << "\n=== 详细命名空间解析调试 ===" << std::endl;
    
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
        
        std::cout << "关键词元信息:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            if (token.type == TokenType::NAMESPACE || 
                token.type == TokenType::CUSTOM ||
                token.type == TokenType::AT_ELEMENT ||
                token.type == TokenType::IDENTIFIER ||
                token.type == TokenType::LEFT_BRACE ||
                token.type == TokenType::RIGHT_BRACE) {
                std::cout << "  [" << i << "] " << token.toString() << std::endl;
            }
        }
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        // 使用默认选项创建解析器
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        std::cout << "\n解析结果详情:" << std::endl;
        std::cout << "  - 命名空间节点数: " << ast.namespaces.size() << std::endl;
        
        if (ast.namespaces.size() > 0) {
            auto namespacePtr = ast.namespaces[0];
            if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                std::cout << "  - 命名空间名称: '" << namespaceNode->getNamespaceName() << "'" << std::endl;
                std::cout << "  - 子节点数: " << namespaceNode->getChildren().size() << std::endl;
                
                // 遍历所有子节点
                auto children = namespaceNode->getChildren();
                for (size_t i = 0; i < children.size(); ++i) {
                    auto child = children[i];
                    std::cout << "    子节点[" << i << "]: 类型=" << static_cast<int>(child->getNodeType()) 
                              << ", 名称='" << child->getName() << "'" << std::endl;
                }
                
                // 检查子命名空间
                std::cout << "  - 子命名空间数: " << namespaceNode->getChildNamespaces().size() << std::endl;
            }
        }
        
        // 检查所有类型的顶级节点
        std::cout << "\n所有解析结果:" << std::endl;
        std::cout << "  - 自定义节点数: " << ast.customNodes.size() << std::endl;
        std::cout << "  - 模板节点数: " << ast.templates.size() << std::endl;
        
        for (size_t i = 0; i < ast.customNodes.size(); ++i) {
            auto customPtr = ast.customNodes[i];
            std::cout << "    自定义节点[" << i << "]: 名称='" << customPtr->getName() << "'" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 调试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 详细命名空间解析调试" << std::endl;
    std::cout << "=========================" << std::endl;
    
    debugDetailedNamespaceParsing();
    
    std::cout << "\n调试完成！" << std::endl;
    return 0;
}