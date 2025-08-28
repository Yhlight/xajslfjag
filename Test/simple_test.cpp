#include <iostream>
#include <memory>
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL/CHTLContext/NamespaceManager.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"

int main() {
    std::cout << "CHTL项目简化测试开始..." << std::endl;
    
    try {
        // 测试统一扫描器
        std::cout << "\n测试统一扫描器..." << std::endl;
        Scanner::CHTLUnifiedScanner scanner;
        
        // 测试CHTL代码
        std::string chtlCode = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}

div
{
    style
    {
        @Style DefaultText;
    }
}
)";
        
        std::cout << "测试CHTL代码扫描..." << std::endl;
        scanner.setSource(chtlCode);
        auto fragments = scanner.scan();
        
        std::cout << "扫描到 " << fragments.size() << " 个代码片段:" << std::endl;
        for (const auto& fragment : fragments) {
            std::string typeStr;
            switch (fragment.type) {
                case Scanner::CodeFragmentType::CHTL: typeStr = "CHTL"; break;
                case Scanner::CodeFragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
                case Scanner::CodeFragmentType::CSS: typeStr = "CSS"; break;
                case Scanner::CodeFragmentType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
                default: typeStr = "UNKNOWN"; break;
            }
            
            std::cout << "类型: " << typeStr 
                      << ", 位置: [" << fragment.startPos << ", " << fragment.endPos << "]"
                      << ", 行: " << fragment.line << ", 列: " << fragment.column << std::endl;
            std::cout << "内容: " << fragment.content.substr(0, 100) 
                      << (fragment.content.length() > 100 ? "..." : "") << std::endl;
            std::cout << "---" << std::endl;
        }
        
        // 测试CHTL JS代码
        std::string chtlJSCode = R"(
script
{
    vir test = listen {
        click: () => {
            std::cout << "Clicked!";
        }
    };
    
    {{.box}}->addEventListener('click', () => {
        std::cout << "Box clicked!";
    });
}
)";
        
        std::cout << "\n测试CHTL JS代码扫描..." << std::endl;
        scanner.setSource(chtlJSCode);
        fragments = scanner.scan();
        
        std::cout << "扫描到 " << fragments.size() << " 个代码片段:" << std::endl;
        for (const auto& fragment : fragments) {
            std::string typeStr;
            switch (fragment.type) {
                case Scanner::CodeFragmentType::CHTL: typeStr = "CHTL"; break;
                case Scanner::CodeFragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
                case Scanner::CodeFragmentType::CSS: typeStr = "CSS"; break;
                case Scanner::CodeFragmentType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
                default: typeStr = "UNKNOWN"; break;
            }
            
            std::cout << "类型: " << typeStr 
                      << ", 位置: [" << fragment.startPos << ", " << fragment.endPos << "]"
                      << ", 行: " << fragment.line << ", 列: " << fragment.column << std::endl;
            std::cout << "内容: " << fragment.content.substr(0, 100) 
                      << (fragment.content.length() > 100 ? "..." : "") << std::endl;
            std::cout << "---" << std::endl;
        }
        
        // 测试CHTL解析器
        std::cout << "\n测试CHTL解析器..." << std::endl;
        CHTL::CHTLParser chtlParser;
        
        // 测试模板解析
        std::string templateCode = R"(
[Template] @Style DefaultText
{
    color: "black";
    line-height: 1.6;
}
)";
        
        chtlParser.setSource(templateCode);
        auto templateAST = chtlParser.parse();
        std::cout << "模板解析: " << (chtlParser.isSuccess() ? "成功" : "失败") << std::endl;
        if (templateAST) {
            std::cout << "AST类型: " << static_cast<int>(templateAST->type) << std::endl;
            if (auto templateNode = std::dynamic_pointer_cast<CHTL::TemplateNode>(templateAST)) {
                std::cout << "模板类型: " << templateNode->templateType << std::endl;
                std::cout << "模板名称: " << templateNode->templateName << std::endl;
            }
        }
        
        // 测试CHTL JS解析器
        std::cout << "\n测试CHTL JS解析器..." << std::endl;
        CHTLJS::CHTLJSParser chtlJSParser;
        
        // 测试虚拟对象解析
        std::string virCode = R"(
vir test = listen {
    click: () => {
        std::cout << "Clicked!";
    }
}
)";
        
        chtlJSParser.setSource(virCode);
        auto virAST = chtlJSParser.parse();
        std::cout << "虚拟对象解析: " << (chtlJSParser.isSuccess() ? "成功" : "失败") << std::endl;
        if (virAST) {
            std::cout << "AST类型: " << static_cast<int>(virAST->type) << std::endl;
            if (auto virNode = std::dynamic_pointer_cast<CHTLJS::VirtualObjectNode>(virAST)) {
                std::cout << "对象名称: " << virNode->objectName << std::endl;
            }
        }
        
        // 测试命名空间管理
        std::cout << "\n测试命名空间管理..." << std::endl;
        auto configManager = std::make_shared<CHTL::ConfigurationManager>();
        CHTL::NamespaceManager nsManager(configManager);
        
        // 创建嵌套命名空间
        nsManager.createNamespace("Core", "test.chtl");
        nsManager.createNestedNamespace("Core", "UI", "test.chtl");
        nsManager.createNestedNamespace("UI", "Components", "test.chtl");
        
        // 添加命名空间项
        auto versionItem = std::make_shared<CHTL::NamespaceItem>();
        versionItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
        versionItem->name = "version";
        versionItem->sourceFile = "test.chtl";
        versionItem->lineNumber = 1;
        versionItem->content = "1.0.0";
        nsManager.addNamespaceItem("Core", versionItem);
        
        auto themeItem = std::make_shared<CHTL::NamespaceItem>();
        themeItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
        themeItem->name = "theme";
        themeItem->sourceFile = "test.chtl";
        themeItem->lineNumber = 2;
        themeItem->content = "dark";
        nsManager.addNamespaceItem("UI", themeItem);
        
        auto buttonItem = std::make_shared<CHTL::NamespaceItem>();
        buttonItem->type = CHTL::NamespaceItemType::CUSTOM_ELEMENT;
        buttonItem->name = "button";
        buttonItem->sourceFile = "test.chtl";
        buttonItem->lineNumber = 3;
        buttonItem->content = "ButtonComponent";
        nsManager.addNamespaceItem("Components", buttonItem);
        
        // 测试命名空间项查找
        auto versionFound = nsManager.getNamespaceItem("Core", "version", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
        std::cout << "Core::version: " << (versionFound ? versionFound->content : "not found") << std::endl;
        
        auto themeFound = nsManager.getNamespaceItem("UI", "theme", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
        std::cout << "UI::theme: " << (versionFound ? themeFound->content : "not found") << std::endl;
        
        auto buttonFound = nsManager.getNamespaceItem("Components", "button", CHTL::NamespaceItemType::CUSTOM_ELEMENT);
        std::cout << "Components::button: " << (buttonFound ? buttonFound->content : "not found") << std::endl;
        
        // 测试编译器调度器
        std::cout << "\n测试编译器调度器..." << std::endl;
        CHTLCompiler::CompilerDispatcher dispatcher;
        
        // 测试混合代码编译
        std::string mixedCode = R"(
[Template] @Style Button
{
    background: "blue";
    color: "white";
}

button
{
    style
    {
        @Style Button;
    }
    
    script
    {
        vir btn = listen {
            click: () => {
                std::cout << "Button clicked!";
            }
        };
    }
}
)";
        
        dispatcher.setSource(mixedCode);
        auto compilationResult = dispatcher.compile();
        
        std::cout << "编译结果: " << (compilationResult.success ? "成功" : "失败") << std::endl;
        if (!compilationResult.errors.empty()) {
            std::cout << "编译错误:" << std::endl;
            for (const auto& error : compilationResult.errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "输出长度: " << compilationResult.output.length() << " 字符" << std::endl;
        
        std::cout << "\n简化测试完成!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "测试异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "未知异常" << std::endl;
        return 1;
    }
}