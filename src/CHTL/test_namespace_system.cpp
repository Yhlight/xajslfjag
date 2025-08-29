#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/NamespaceManager.h"
#include "CHTLNode/NamespaceNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void testBasicNamespace() {
    std::cout << "\n=== 测试基本命名空间 ===" << std::endl;
    
    std::string namespaceCode = R"(
[Namespace] space
{
    [Custom] @Element Box
    {
        div
        {
            style
            {
                width: 100px;
                height: 100px;
            }
        }
    }
    
    [Template] @Style DefaultText
    {
        color: black;
        font-size: 16px;
    }
}
)";
    
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
            
            CHTLNamespaceManager nsManager;
            
            for (auto namespacePtr : ast.namespaces) {
                if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                    nsManager.registerNamespace(namespaceNode, "test.chtl");
                    std::cout << "  - 注册命名空间: " << namespaceNode->getNamespaceName() << std::endl;
                    std::cout << "    完整路径: " << namespaceNode->getFullNamespacePath() << std::endl;
                }
            }
            
            // 测试命名空间查询
            bool spaceExists = nsManager.namespaceExists("space");
            std::cout << "  - 命名空间 'space' 存在: " << (spaceExists ? "是" : "否") << std::endl;
            
            auto symbols = nsManager.getNamespaceSymbols("space");
            std::cout << "  - 命名空间符号数: " << symbols.size() << std::endl;
            
        } else {
            std::cout << "❌ 命名空间语法解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 基本命名空间测试异常: " << e.what() << std::endl;
    }
}

void testNestedNamespace() {
    std::cout << "\n=== 测试嵌套命名空间 ===" << std::endl;
    
    std::string namespaceCode = R"(
[Namespace] space
{
    [Namespace] room
    {
        [Custom] @Element Table
        {
            table
            {
                style
                {
                    border-collapse: collapse;
                }
            }
        }
    }
    
    [Namespace] room2
    {
        [Custom] @Element Box
        {
            div
            {
                style
                {
                    background: blue;
                }
            }
        }
    }
}
)";
    
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
            std::cout << "✓ 嵌套命名空间解析成功" << std::endl;
            
            CHTLNamespaceManager nsManager;
            
            for (auto namespacePtr : ast.namespaces) {
                if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                    nsManager.registerNamespace(namespaceNode, "nested_test.chtl");
                    std::cout << "  - 注册命名空间: " << namespaceNode->getFullNamespacePath() << std::endl;
                }
            }
            
            // 测试嵌套命名空间查询
            bool nestedExists = nsManager.namespaceExists("space.room2");
            std::cout << "  - 嵌套命名空间 'space.room2' 存在: " << (nestedExists ? "是" : "否") << std::endl;
            
            auto hierarchy = nsManager.getNamespaceHierarchy("space.room2");
            std::cout << "  - 命名空间层次: ";
            for (size_t i = 0; i < hierarchy.size(); ++i) {
                if (i > 0) std::cout << " -> ";
                std::cout << hierarchy[i];
            }
            std::cout << std::endl;
            
            auto children = nsManager.getChildNamespaces("space");
            std::cout << "  - space的子命名空间数: " << children.size() << std::endl;
            
        } else {
            std::cout << "❌ 嵌套命名空间解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 嵌套命名空间测试异常: " << e.what() << std::endl;
    }
}

void testNamespaceReference() {
    std::cout << "\n=== 测试命名空间引用解析 ===" << std::endl;
    
    std::string referenceCode = R"(
[Namespace] ui
{
    [Custom] @Element Button
    {
        button
        {
            style
            {
                padding: 10px;
                border-radius: 4px;
            }
        }
    }
}

body
{
    @Element Button from ui;
}
)";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(referenceCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success) {
            std::cout << "✓ 命名空间引用解析成功" << std::endl;
            
            CHTLNamespaceManager nsManager;
            
            // 注册命名空间
            for (auto namespacePtr : ast.namespaces) {
                if (auto namespaceNode = std::dynamic_pointer_cast<NamespaceNode>(namespacePtr)) {
                    nsManager.registerNamespace(namespaceNode, "reference_test.chtl");
                }
            }
            
            // 测试符号解析
            auto symbol = nsManager.resolveNamespaceReference("Button", NamespaceSymbolType::CUSTOM_ELEMENT, "ui");
            if (symbol.node) {
                std::cout << "✓ 成功解析命名空间引用: " << symbol.name << " from " << symbol.namespacePath << std::endl;
            } else {
                std::cout << "❌ 命名空间引用解析失败" << std::endl;
            }
            
            // 测试通过完整路径查找
            auto pathSymbol = nsManager.findSymbolByPath("ui.Button");
            if (pathSymbol.node) {
                std::cout << "✓ 成功通过完整路径查找符号: " << pathSymbol.name << std::endl;
            } else {
                std::cout << "❌ 完整路径符号查找失败" << std::endl;
            }
            
        } else {
            std::cout << "❌ 命名空间引用解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 命名空间引用测试异常: " << e.what() << std::endl;
    }
}

void testNamespaceMerging() {
    std::cout << "\n=== 测试命名空间合并 ===" << std::endl;
    
    try {
        CHTLNamespaceManager nsManager;
        
        // 创建两个同名的命名空间
        auto namespace1 = std::make_shared<NamespaceNode>("common");
        auto namespace2 = std::make_shared<NamespaceNode>("common");
        
        // 注册第一个命名空间
        nsManager.registerNamespace(namespace1, "file1.chtl");
        
        // 创建一些符号信息
        NamespaceSymbolInfo symbol1("Button", NamespaceSymbolType::CUSTOM_ELEMENT, namespace1, "common", "file1.chtl");
        nsManager.registerSymbol("common", symbol1);
        
        // 注册第二个同名命名空间
        nsManager.registerNamespace(namespace2, "file2.chtl");
        
        NamespaceSymbolInfo symbol2("Card", NamespaceSymbolType::CUSTOM_ELEMENT, namespace2, "common", "file2.chtl");
        nsManager.registerSymbol("common", symbol2);
        
        std::cout << "✓ 同名命名空间注册完成" << std::endl;
        
        // 执行合并
        nsManager.mergeNamespaces();
        
        // 检查合并结果
        auto symbols = nsManager.getNamespaceSymbols("common");
        std::cout << "  - 合并后的符号数: " << symbols.size() << std::endl;
        
        for (const auto& symbol : symbols) {
            std::cout << "    " << symbol.name << " (" << symbol.sourceFile << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 命名空间合并测试异常: " << e.what() << std::endl;
    }
}

void testDefaultFileNamespace() {
    std::cout << "\n=== 测试默认文件命名空间 ===" << std::endl;
    
    try {
        CHTLNamespaceManager nsManager;
        
        // 测试文件命名空间生成
        std::string defaultNs1 = nsManager.createDefaultFileNamespace("components/button.chtl");
        std::string defaultNs2 = nsManager.createDefaultFileNamespace("utils/helpers.chtl");
        
        std::cout << "✓ 默认命名空间生成成功" << std::endl;
        std::cout << "  - button.chtl -> " << defaultNs1 << std::endl;
        std::cout << "  - helpers.chtl -> " << defaultNs2 << std::endl;
        
        // 测试禁用默认命名空间
        nsManager.setDefaultNamespaceEnabled(false);
        std::string disabledNs = nsManager.createDefaultFileNamespace("test.chtl");
        
        std::cout << "  - 禁用后: test.chtl -> '" << disabledNs << "'" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 默认文件命名空间测试异常: " << e.what() << std::endl;
    }
}

void testNamespaceReports() {
    std::cout << "\n=== 测试命名空间报告生成 ===" << std::endl;
    
    try {
        CHTLNamespaceManager nsManager;
        
        // 创建一些测试数据
        auto namespace1 = std::make_shared<NamespaceNode>("components");
        auto namespace2 = std::make_shared<NamespaceNode>("utils");
        
        nsManager.registerNamespace(namespace1, "components.chtl");
        nsManager.registerNamespace(namespace2, "utils.chtl");
        
        // 添加符号
        NamespaceSymbolInfo symbol1("Button", NamespaceSymbolType::CUSTOM_ELEMENT, namespace1, "components", "components.chtl");
        NamespaceSymbolInfo symbol2("Card", NamespaceSymbolType::CUSTOM_ELEMENT, namespace1, "components", "components.chtl");
        NamespaceSymbolInfo symbol3("Helper", NamespaceSymbolType::TEMPLATE_STYLE, namespace2, "utils", "utils.chtl");
        
        nsManager.registerSymbol("components", symbol1);
        nsManager.registerSymbol("components", symbol2);
        nsManager.registerSymbol("utils", symbol3);
        
        // 生成使用报告
        std::string usageReport = nsManager.generateUsageReport();
        std::cout << "✓ 使用报告生成成功:" << std::endl;
        std::cout << usageReport << std::endl;
        
        // 测试冲突检测（创建同名符号）
        NamespaceSymbolInfo conflictSymbol("Button", NamespaceSymbolType::TEMPLATE_ELEMENT, namespace1, "components", "conflict.chtl");
        nsManager.registerSymbol("components", conflictSymbol);
        
        std::string conflictReport = nsManager.generateConflictReport();
        std::cout << "✓ 冲突报告生成成功:" << std::endl;
        std::cout << conflictReport << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 命名空间报告测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 命名空间系统测试" << std::endl;
    std::cout << "====================" << std::endl;
    
    try {
        testBasicNamespace();
        testNestedNamespace();
        testNamespaceReference();
        testNamespaceMerging();
        testDefaultFileNamespace();
        testNamespaceReports();
        
        std::cout << "\n命名空间系统测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试过程中发生异常: " << e.what() << std::endl;
    }
    
    return 0;
}