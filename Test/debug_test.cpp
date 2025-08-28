#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../CHTL/CHTLContext/ConfigurationManager.h"
#include "../CHTL/CHTLContext/NamespaceManager.h"
#include "../CHTL/CHTLContext/ImportManager.h"
#include "../CHTL/CHTLContext/SelectorAutomation.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../Scanner/CHTLUnifiedScanner.h"
#include "../CompilerDispatcher/CompilerDispatcher.h"

// 前向声明
namespace CHTL {
    class CHTLParser;
}

// 测试CHTL解析器
void testCHTLParser() {
    std::cout << "测试CHTL解析器..." << std::endl;
    
    // 简单的模板测试
    std::string simpleTemplate = R"(
[Template] @Style Test
{
    color: "red";
}
)";
    
    std::cout << "模板代码: " << simpleTemplate << std::endl;
    
    try {
        CHTL::CHTLParser parser;
        std::cout << "解析器创建成功" << std::endl;
        
        parser.setSource(simpleTemplate);
        std::cout << "源代码设置成功" << std::endl;
        
        auto result = parser.parse();
        std::cout << "解析结果: " << (result ? "成功" : "失败") << std::endl;
        
        if (result) {
            std::cout << "AST类型: " << static_cast<int>(result->type) << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "解析异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "未知异常" << std::endl;
    }
    
    std::cout << "CHTL解析器测试完成" << std::endl;
}

// 测试CHTL JS解析器
void testCHTLJSParser() {
    std::cout << "测试CHTL JS解析器..." << std::endl;
    
    // 简单的虚拟对象测试
    std::string simpleVir = R"(
vir test = listen {
    click: () => {}
}
)";
    
    std::cout << "虚拟对象代码: " << simpleVir << std::endl;
    
    try {
        CHTLJS::CHTLJSParser parser;
        std::cout << "解析器创建成功" << std::endl;
        
        parser.setSource(simpleVir);
        std::cout << "源代码设置成功" << std::endl;
        
        auto result = parser.parse();
        std::cout << "解析结果: " << (result ? "成功" : "失败") << std::endl;
        
        if (result) {
            std::cout << "AST类型: " << static_cast<int>(result->type) << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "解析异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "未知异常" << std::endl;
    }
    
    std::cout << "CHTL JS解析器测试完成" << std::endl;
}

// 测试命名空间管理
void testNamespaceManager() {
    std::cout << "=== 测试命名空间管理器 ===" << std::endl;
    
    auto configManager = std::make_shared<CHTL::ConfigurationManager>();
    CHTL::NamespaceManager nsManager(configManager);
    
    // 创建命名空间
    assert(nsManager.createNamespace("Core", "core.chtl"));
    assert(nsManager.createNamespace("UI", "ui.chtl"));
    assert(nsManager.createNamespace("Components", "components.chtl"));
    
    // 创建命名空间项
    auto versionItem = std::make_shared<CHTL::NamespaceItem>("version", CHTL::NamespaceItemType::CUSTOM_ELEMENT, "core.chtl", 1, 5);
    versionItem->content = "[Custom] @Element version { }";
    
    auto themeItem = std::make_shared<CHTL::NamespaceItem>("theme", CHTL::NamespaceItemType::CUSTOM_STYLE, "ui.chtl", 2, 5);
    themeItem->content = "[Custom] @Style theme { }";
    
    auto buttonItem = std::make_shared<CHTL::NamespaceItem>("button", CHTL::NamespaceItemType::CUSTOM_ELEMENT, "components.chtl", 3, 5);
    buttonItem->content = "[Custom] @Element button { }";
    
    // 添加项到命名空间
    assert(nsManager.addNamespaceItem("Core", versionItem));
    assert(nsManager.addNamespaceItem("UI", themeItem));
    assert(nsManager.addNamespaceItem("Components", buttonItem));
    
    // 测试命名空间项获取
    auto versionFound = nsManager.getNamespaceItem("Core", "version");
    auto themeFound = nsManager.getNamespaceItem("UI", "theme");
    auto buttonFound = nsManager.getNamespaceItem("Components", "button");
    
    assert(versionFound != nullptr);
    assert(themeFound != nullptr);
    assert(buttonFound != nullptr);
    
    // 测试命名空间存在性
    std::cout << "Core命名空间存在: " << (nsManager.hasNamespace("Core") ? "是" : "否") << std::endl;
    std::cout << "UI命名空间存在: " << (nsManager.hasNamespace("UI") ? "是" : "否") << std::endl;
    std::cout << "Components命名空间存在: " << (nsManager.hasNamespace("Components") ? "是" : "否") << std::endl;
    
    // 测试命名空间项内容
    std::cout << "version项内容: " << versionFound->content << std::endl;
    std::cout << "theme项内容: " << themeFound->content << std::endl;
    std::cout << "button项内容: " << buttonFound->content << std::endl;
    
    std::cout << "命名空间管理器测试通过！" << std::endl;
}

// 测试统一扫描器
void testUnifiedScanner() {
    std::cout << "测试统一扫描器..." << std::endl;
    
    try {
        Scanner::CHTLUnifiedScanner scanner;
        std::cout << "统一扫描器创建成功" << std::endl;
        
        // 测试混合代码扫描
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
        
        scanner.setSource(mixedCode);
        std::cout << "源代码设置成功" << std::endl;
        
        auto fragments = scanner.scan();
        std::cout << "扫描到 " << fragments.size() << " 个代码片段" << std::endl;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            std::cout << "片段 " << i << ": 类型=" << static_cast<int>(fragment.type) 
                      << ", 位置=[" << fragment.line << "," << fragment.column << "]"
                      << ", 长度=" << fragment.content.length() << std::endl;
        }
        
        std::cout << "统一扫描器测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "统一扫描器异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "统一扫描器未知异常" << std::endl;
    }
}

// 测试编译器调度器
void testCompilerDispatcher() {
    std::cout << "测试编译器调度器..." << std::endl;
    
    try {
        CHTLCompiler::CompilerDispatcher dispatcher;
        std::cout << "编译器调度器创建成功" << std::endl;
        
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
        std::cout << "源代码设置成功" << std::endl;
        
        auto compilationResult = dispatcher.compile();
        std::cout << "编译结果: " << (compilationResult.success ? "成功" : "失败") << std::endl;
        
        if (!compilationResult.errors.empty()) {
            std::cout << "编译错误:" << std::endl;
            for (const auto& error : compilationResult.errors) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "输出长度: " << compilationResult.output.length() << " 字符" << std::endl;
        
        std::cout << "编译器调度器测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "编译器调度器异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "编译器调度器未知异常" << std::endl;
    }
}

int main() {
    std::cout << "开始详细调试测试..." << std::endl;
    
    try {
        testCHTLParser();
        testCHTLJSParser();
        testNamespaceManager();
        testUnifiedScanner();
        testCompilerDispatcher();
        
        std::cout << "所有详细测试完成!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "测试异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "未知异常" << std::endl;
        return 1;
    }
}