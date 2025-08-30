#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ScriptNode.h"
#include "../Error/ErrorReport.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testCHTLJSModule() {
    std::cout << "\n=== 测试CHTL JS模块加载 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    module {
        load: ./module1.cjjs,
        load: ./module2.cjjs,
        load: ./framework.js
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "模块语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
    
    if (result.rootNode && result.rootNode->getChildCount() > 0) {
        auto scriptNode = result.rootNode->getChild(0);
        if (scriptNode && scriptNode->getNodeType() == CHTLNodeType::SCRIPT_NODE) {
            std::cout << "找到script节点: ✓" << std::endl;
        }
    }
}

void testCHTLJSEnhancedSelector() {
    std::cout << "\n=== 测试CHTL JS增强选择器 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    {{.box}} -> addEventListener('click', () => {
        console.log('Box clicked!');
    });
    
    {{#header}} -> style.display = 'none';
    
    {{button[0]}} -> textContent = 'First Button';
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    // 检查是否识别了增强选择器token
    bool foundEnhancedSelector = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::ENHANCED_SELECTOR_START) {
            foundEnhancedSelector = true;
            break;
        }
    }
    
    std::cout << "增强选择器Token识别: " << (foundEnhancedSelector ? "✓" : "❌") << std::endl;
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "增强选择器语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
}

void testCHTLJSListener() {
    std::cout << "\n=== 测试CHTL JS监听器 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    listen {
        click: () => {
            console.log('Clicked!');
        },
        
        mouseenter: function(e) {
            e.target.style.backgroundColor = 'red';
        },
        
        mouseleave: mouseLeaveHandler
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    // 检查是否识别了listen关键字
    bool foundListen = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::LISTEN) {
            foundListen = true;
            break;
        }
    }
    
    std::cout << "listen关键字识别: " << (foundListen ? "✓" : "❌") << std::endl;
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "监听器语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
}

void testCHTLJSVirtualObject() {
    std::cout << "\n=== 测试CHTL JS虚拟对象 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    vir MyObject = {
        prop1: 'value1',
        prop2: 42,
        
        method1: function() {
            return 'Hello from method1';
        },
        
        method2: () => {
            console.log('Arrow function method');
        }
    };
    
    MyObject->method1();
    MyObject->prop1;
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    // 检查是否识别了vir关键字和箭头操作符
    bool foundVir = false;
    bool foundArrow = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::VIR) {
            foundVir = true;
        }
        if (token.type == TokenType::ARROW) {
            foundArrow = true;
        }
    }
    
    std::cout << "vir关键字识别: " << (foundVir ? "✓" : "❌") << std::endl;
    std::cout << "箭头操作符识别: " << (foundArrow ? "✓" : "❌") << std::endl;
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "虚拟对象语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
}

void testCHTLJSEventBinding() {
    std::cout << "\n=== 测试CHTL JS事件绑定操作符 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    {{.button}} &-> click {
        console.log('Button clicked via &-> operator!');
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    // 检查是否识别了绑定操作符
    bool foundBindOperator = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::BIND_OPERATOR) {
            foundBindOperator = true;
            break;
        }
    }
    
    std::cout << "绑定操作符(&->)识别: " << (foundBindOperator ? "✓" : "❌") << std::endl;
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "事件绑定语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
}

void testCHTLJSAnimation() {
    std::cout << "\n=== 测试CHTL JS动画 ===" << std::endl;
    
    std::string chtlCode = R"(
script
{
    animate {
        target: {{.box}},
        duration: 1000,
        easing: 'ease-in-out',
        
        when: [
            {
                at: 0.0,
                opacity: 0,
                transform: 'scale(1)'
            },
            {
                at: 0.5,
                opacity: 0.5,
                transform: 'scale(1.2)'
            },
            {
                at: 1.0,
                opacity: 1,
                transform: 'scale(1)'
            }
        ]
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    // 检查是否识别了animate关键字
    bool foundAnimate = false;
    for (const auto& token : tokens) {
        if (token.type == TokenType::ANIMATE) {
            foundAnimate = true;
            break;
        }
    }
    
    std::cout << "animate关键字识别: " << (foundAnimate ? "✓" : "❌") << std::endl;
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "动画语法解析: " << (result.rootNode ? "✓" : "❌") << std::endl;
}

int main() {
    std::cout << "CHTL JS 系统测试" << std::endl;
    std::cout << "===============" << std::endl;
    
    testCHTLJSModule();
    testCHTLJSEnhancedSelector();
    testCHTLJSListener();
    testCHTLJSVirtualObject();
    testCHTLJSEventBinding();
    testCHTLJSAnimation();
    
    std::cout << "\n=== CHTL JS 测试完成 ===" << std::endl;
    
    return 0;
}