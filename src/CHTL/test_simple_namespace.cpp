#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/NamespaceNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void testSimpleNamespace() {
    std::cout << "\n=== 测试简单命名空间解析 ===" << std::endl;
    
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
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success) {
            std::cout << "✓ 命名空间语法解析成功" << std::endl;
            std::cout << "  - 命名空间节点数: " << ast.namespaces.size() << std::endl;
            
            for (auto namespacePtr : ast.namespaces) {
                if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                    std::cout << "  - 命名空间名称: " << namespaceNode->getNamespaceName() << std::endl;
                    std::cout << "  - 子节点数: " << namespaceNode->getChildren().size() << std::endl;
                    
                    // 检查子节点类型
                    auto children = namespaceNode->getChildren();
                    for (size_t i = 0; i < children.size(); ++i) {
                        auto child = children[i];
                        std::cout << "    子节点[" << i << "]: 类型=" << static_cast<int>(child->getNodeType()) 
                                  << ", 名称='" << child->getName() << "'" << std::endl;
                        
                        // 如果是自定义节点，进一步检查其内容
                        if (child->getNodeType() == CHTLNodeType::CUSTOM_ELEMENT_NODE ||
                            child->getNodeType() == CHTLNodeType::ADVANCED_TEMPLATE_ELEMENT_NODE) {
                            std::cout << "      ├─ 自定义节点子节点数: " << child->getChildren().size() << std::endl;
                        }
                    }
                }
            }
            
        } else {
            std::cout << "❌ 命名空间语法解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 命名空间测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 简单命名空间测试" << std::endl;
    std::cout << "===================" << std::endl;
    
    testSimpleNamespace();
    
    std::cout << "\n命名空间测试完成！" << std::endl;
    return 0;
}