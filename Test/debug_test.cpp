#include <iostream>
#include <memory>
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL/CHTLContext/NamespaceManager.h"
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
    std::cout << "测试命名空间管理..." << std::endl;
    
    try {
        CHTL::NamespaceManager nsManager;
        std::cout << "命名空间管理器创建成功" << std::endl;
        
        // 创建嵌套命名空间
        nsManager.createNamespace("Core");
        std::cout << "Core命名空间创建成功" << std::endl;
        
        nsManager.createNamespace("UI", "Core");
        std::cout << "UI命名空间创建成功" << std::endl;
        
        nsManager.createNamespace("Components", "UI");
        std::cout << "Components命名空间创建成功" << std::endl;
        
        // 添加符号
        nsManager.addSymbol("Core", "version", "1.0.0");
        std::cout << "Core::version添加成功" << std::endl;
        
        nsManager.addSymbol("UI", "theme", "dark");
        std::cout << "UI::theme添加成功" << std::endl;
        
        nsManager.addSymbol("Components", "button", "ButtonComponent");
        std::cout << "Components::button添加成功" << std::endl;
        
        // 测试符号查找
        std::cout << "Core::version: " << nsManager.findSymbol("Core", "version") << std::endl;
        std::cout << "UI::theme: " << nsManager.findSymbol("UI", "theme") << std::endl;
        std::cout << "Components::button: " << nsManager.findSymbol("Components", "button") << std::endl;
        
        // 测试继承查找
        std::cout << "Components继承Core::version: " << nsManager.findSymbol("Components", "version") << std::endl;
        
        std::cout << "命名空间管理测试完成" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "命名空间管理异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "命名空间管理未知异常" << std::endl;
    }
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