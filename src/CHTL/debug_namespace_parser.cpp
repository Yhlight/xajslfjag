#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/NamespaceNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void debugNamespaceParser() {
    std::cout << "\n=== 调试命名空间解析 ===" << std::endl;
    
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
        
        std::cout << "词法分析完成，token数量: " << tokens.size() << std::endl;
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        std::cout << "解析结果:" << std::endl;
        std::cout << "  - 解析成功: " << (ast.success ? "是" : "否") << std::endl;
        std::cout << "  - 根节点: " << (ast.rootNode ? "存在" : "不存在") << std::endl;
        std::cout << "  - 配置节点数: " << ast.configs.size() << std::endl;
        std::cout << "  - 模板节点数: " << ast.templates.size() << std::endl;
        std::cout << "  - 自定义节点数: " << ast.customNodes.size() << std::endl;
        std::cout << "  - 导入节点数: " << ast.imports.size() << std::endl;
        std::cout << "  - 命名空间节点数: " << ast.namespaces.size() << std::endl;
        
        if (ast.rootNode) {
            std::cout << "  - 根节点子节点数: " << ast.rootNode->getChildren().size() << std::endl;
        }
        
        // 详细检查所有节点
        std::cout << "\n详细节点信息:" << std::endl;
        for (size_t i = 0; i < ast.namespaces.size(); ++i) {
            auto namespacePtr = ast.namespaces[i];
            std::cout << "  命名空间[" << i << "]: ";
            if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                std::cout << "名称='" << namespaceNode->getNamespaceName() << "' ";
                std::cout << "子节点数=" << namespaceNode->getChildren().size();
            } else {
                std::cout << "类型转换失败，节点类型=" << static_cast<int>(namespacePtr->getNodeType());
            }
            std::cout << std::endl;
        }
        
        // 输出解析器可能的错误信息
        if (!ast.success) {
            std::cout << "\n解析失败!" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 调试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 命名空间解析调试" << std::endl;
    std::cout << "====================" << std::endl;
    
    debugNamespaceParser();
    
    std::cout << "\n调试完成！" << std::endl;
    return 0;
}