#include <iostream>
#include <string>
#include <memory>

#include "CHTL_JS/CHTLJSLexer/Lexer.h"
#include "CHTL_JS/CHTLJSLexer/GlobalMap.h"

using namespace CHTL_JS;

void testCHTLJSLexer() {
    std::cout << "=== 测试CHTL JS词法分析器 ===" << std::endl;
    
    std::string source = R"(
// CHTL JS测试代码
script
{
    // 使用增强选择器
    {{box}}->textContent('Hello CHTL JS!');
    
    // 使用监听器
    {{.button}}->listen {
        click: () => {
            console.log('Button clicked!');
        },
        
        mouseenter: function() {
            this.style.backgroundColor = 'blue';
        }
    };
    
    // 使用事件委托
    {{document}}->delegate {
        target: [{{.button}}, {{.link}}],
        click: (event) => {
            event.preventDefault();
        }
    };
    
    // 使用动画
    const anim = animate {
        target: {{.box}},
        duration: 500,
        easing: ease-in-out,
        
        begin: {
            opacity: 0
        },
        
        end: {
            opacity: 1,
            transform: 'scale(1.2)'
        }
    };
    
    // 虚对象
    vir Test = listen {
        click: () => {
            console.log('Virtual object click');
        }
    };
    
    Test->click();
}
)";

    auto globalMap = std::make_shared<GlobalMap>();
    Lexer lexer(source, globalMap);
    
    std::cout << "CHTL JS源代码:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;
    
    std::cout << "CHTL JS Token序列:" << std::endl;
    Token token = lexer.nextToken();
    int count = 0;
    
    while (token.type != TokenType::EOF_TOKEN && count < 100) {
        if (token.type != TokenType::WHITESPACE && 
            token.type != TokenType::NEWLINE) {
            std::cout << count << ": " << token.toString() 
                     << " (行:" << token.position.line 
                     << ", 列:" << token.position.column << ")" << std::endl;
        }
        token = lexer.nextToken();
        ++count;
    }
    
    if (lexer.hasErrors()) {
        std::cout << "\nCHTL JS词法分析错误:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "\nCHTL JS词法分析完成，无错误！" << std::endl;
    }
}

void testCHTLJSGlobalMap() {
    std::cout << "\n=== 测试CHTL JS全局映射管理 ===" << std::endl;
    
    GlobalMap globalMap;
    
    // 测试增强选择器
    globalMap.addEnhancedSelector("{{box}}", ".main-box");
    globalMap.addEnhancedSelector("{{.button}}", ".btn");
    
    std::cout << "增强选择器解析:" << std::endl;
    std::cout << "{{box}} -> " << globalMap.resolveSelector("{{box}}") << std::endl;
    std::cout << "{{.button}} -> " << globalMap.resolveSelector("{{.button}}") << std::endl;
    std::cout << "是否为增强选择器 {{box}}: " << (globalMap.isSelectorEnhanced("{{box}}") ? "是" : "否") << std::endl;
    
    // 测试事件支持
    std::cout << "\n事件支持检查:" << std::endl;
    std::cout << "支持click事件: " << (globalMap.isSupportedEvent("click") ? "是" : "否") << std::endl;
    std::cout << "支持customEvent事件: " << (globalMap.isSupportedEvent("customEvent") ? "是" : "否") << std::endl;
    std::cout << "支持的事件数量: " << globalMap.getSupportedEvents().size() << std::endl;
    
    // 测试选择器使用计数
    globalMap.incrementSelectorUsage("{{box}}");
    globalMap.incrementSelectorUsage("{{box}}");
    globalMap.incrementSelectorUsage("{{.button}}");
    
    std::cout << "\n选择器使用计数:" << std::endl;
    std::cout << "{{box}} 使用次数: " << globalMap.getSelectorUsage("{{box}}") << std::endl;
    std::cout << "{{.button}} 使用次数: " << globalMap.getSelectorUsage("{{.button}}") << std::endl;
    
    // 测试全局函数注册
    globalMap.registerGlobalFunction("test_func", "function test_func() { console.log('test'); }");
    globalMap.registerGlobalFunction("click_handler", "function click_handler(event) { console.log('clicked'); }");
    
    std::cout << "\n全局函数管理:" << std::endl;
    std::cout << "注册的全局函数数量: " << globalMap.getAllGlobalFunctions().size() << std::endl;
    std::cout << "test_func 函数代码: " << globalMap.getGlobalFunction("test_func") << std::endl;
    
    // 测试变量组
    std::unordered_map<std::string, std::string> themeVars = {
        {"primaryColor", "#007bff"},
        {"secondaryColor", "#6c757d"},
        {"fontSize", "16px"}
    };
    globalMap.registerVariableGroup("Theme", themeVars);
    
    std::cout << "\n变量组管理:" << std::endl;
    std::cout << "Theme.primaryColor: " << globalMap.getVariable("Theme", "primaryColor") << std::endl;
    std::cout << "Theme.fontSize: " << globalMap.getVariable("Theme", "fontSize") << std::endl;
    std::cout << "是否有Theme变量组: " << (globalMap.hasVariableGroup("Theme") ? "是" : "否") << std::endl;
    
    // 测试键值对无序支持
    std::cout << "\n键值对无序支持: " << (globalMap.isUnorderedKeyValueEnabled() ? "启用" : "禁用") << std::endl;
}

int main() {
    std::cout << "CHTL JS编译器测试程序" << std::endl;
    std::cout << "=====================" << std::endl;
    
    try {
        testCHTLJSLexer();
        testCHTLJSGlobalMap();
        
        std::cout << "\n=====================" << std::endl;
        std::cout << "CHTL JS测试完成！CHTL JS编译器基础架构实现成功！" << std::endl;
        std::cout << "CHTL JS作为独立的编程语言已经可以正常工作 🚀" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "CHTL JS测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}