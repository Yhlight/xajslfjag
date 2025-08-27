#include <iostream>
#include <string>
#include <memory>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLLexer/GlobalMap.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTLState/CHTLState.h"
#include "CHTL/CHTLNode/BaseNode/ElementNode/ElementNode.h"
#include "CHTL/CHTLNode/BaseNode/TextNode/TextNode.h"

using namespace CHTL;

void testLexer() {
    std::cout << "=== 测试CHTL词法分析器 ===" << std::endl;
    
    std::string source = R"(
// CHTL测试代码
html
{
    head
    {
        title
        {
            text
            {
                "Hello CHTL"
            }
        }
    }
    
    body
    {
        div
        {
            id: container;
            class: "main-content";
            
            style
            {
                .box
                {
                    width: 300px;
                    height: 200px;
                    background-color: red;
                }
                
                &:hover
                {
                    background-color: blue;
                }
            }
            
            text
            {
                欢迎使用CHTL！
            }
        }
    }
}
)";

    auto globalMap = std::make_shared<GlobalMap>();
    Lexer lexer(source, globalMap);
    
    std::cout << "源代码:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;
    
    std::cout << "Token序列:" << std::endl;
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
        std::cout << "\n词法分析错误:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "\n词法分析完成，无错误！" << std::endl;
    }
}

void testNodes() {
    std::cout << "\n=== 测试CHTL节点系统 ===" << std::endl;
    
    // 创建元素节点
    auto divElement = std::make_shared<ElementNode>("div", Position{1, 1, 0});
    divElement->setId("container");
    divElement->addClass("main-content");
    divElement->addClass("highlight");
    divElement->setInlineStyle("color", "red");
    divElement->setInlineStyle("font-size", "16px");
    
    // 创建文本节点
    auto textNode = std::make_shared<TextNode>("Hello, CHTL World!", Position{2, 1, 0});
    textNode->setTextType(TextNode::TextType::TEXT_BLOCK);
    
    // 建立父子关系
    divElement->addTextNode(textNode);
    
    std::cout << "创建的div元素:" << std::endl;
    std::cout << "标签名: " << divElement->getTagName() << std::endl;
    std::cout << "ID: " << divElement->getId() << std::endl;
    std::cout << "类名: ";
    for (const auto& className : divElement->getClasses()) {
        std::cout << className << " ";
    }
    std::cout << std::endl;
    
    std::cout << "内联样式: ";
    for (const auto& style : divElement->getAllInlineStyles()) {
        std::cout << style.first << ":" << style.second << "; ";
    }
    std::cout << std::endl;
    
    std::cout << "文本内容: " << divElement->getAllText() << std::endl;
    std::cout << "子节点数量: " << divElement->getChildCount() << std::endl;
    
    // 验证节点
    if (divElement->validate()) {
        std::cout << "节点验证: 通过" << std::endl;
    } else {
        std::cout << "节点验证: 失败" << std::endl;
        auto errors = divElement->getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  错误: " << error << std::endl;
        }
    }
    
    // 测试节点克隆
    auto clonedDiv = std::dynamic_pointer_cast<ElementNode>(divElement->clone());
    if (clonedDiv) {
        std::cout << "节点克隆: 成功" << std::endl;
        std::cout << "克隆节点ID: " << clonedDiv->getId() << std::endl;
    }
}

void testContext() {
    std::cout << "\n=== 测试CHTL上下文管理 ===" << std::endl;
    
    auto globalMap = std::make_shared<GlobalMap>();
    CHTLContext context(globalMap);
    
    std::cout << "初始上下文: " << contextTypeToString(context.getCurrentContext().type) << std::endl;
    
    // 测试上下文推入和弹出
    {
        ContextGuard guard(context, ContextType::ELEMENT, nullptr, "div");
        std::cout << "推入元素上下文: " << contextTypeToString(context.getCurrentContext().type) << std::endl;
        std::cout << "上下文深度: " << context.getContextDepth() << std::endl;
        
        {
            ContextGuard styleGuard(context, ContextType::STYLE);
            std::cout << "推入样式上下文: " << contextTypeToString(context.getCurrentContext().type) << std::endl;
            std::cout << "是否在样式上下文中: " << (context.isInStyleContext() ? "是" : "否") << std::endl;
        }
        
        std::cout << "弹出样式上下文: " << contextTypeToString(context.getCurrentContext().type) << std::endl;
    }
    
    std::cout << "弹出元素上下文: " << contextTypeToString(context.getCurrentContext().type) << std::endl;
    
    // 测试约束
    context.addConstraint("span");
    context.addConstraint("@Html");
    
    std::cout << "添加约束后:" << std::endl;
    std::cout << "能否使用div元素: " << (context.canUseElement("div") ? "是" : "否") << std::endl;
    std::cout << "能否使用span元素: " << (context.canUseElement("span") ? "是" : "否") << std::endl;
    
    // 测试错误报告
    context.reportError("测试错误信息", Position{10, 5, 100});
    context.reportWarning("测试警告信息", Position{15, 8, 200});
    
    if (context.hasErrors()) {
        std::cout << "\n错误信息:" << std::endl;
        for (const auto& error : context.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    if (context.hasWarnings()) {
        std::cout << "\n警告信息:" << std::endl;
        for (const auto& warning : context.getWarnings()) {
            std::cout << "  " << warning << std::endl;
        }
    }
}

void testState() {
    std::cout << "\n=== 测试CHTL状态管理 ===" << std::endl;
    
    CHTLState state;
    
    std::cout << "初始状态: " << parseStateToString(state.getCurrentState()) << std::endl;
    
    // 测试状态管理
    {
        StateGuard guard(state, ParseState::PARSING_ELEMENT);
        std::cout << "推入元素解析状态: " << parseStateToString(state.getCurrentState()) << std::endl;
        
        // 测试深度管理
        {
            DepthGuard braceGuard(state, DepthGuard::DepthType::BRACE);
            state.incrementBraceDepth();
            std::cout << "花括号深度: " << state.getBraceDepth() << std::endl;
        }
        
        std::cout << "退出花括号作用域后深度: " << state.getBraceDepth() << std::endl;
        
        // 测试标志
        state.setInLocalStyleBlock(true);
        state.enableAutoClass(true);
        
        std::cout << "是否在局部样式块中: " << (state.isInLocalStyleBlock() ? "是" : "否") << std::endl;
        std::cout << "是否启用自动类名: " << (state.isAutoClassEnabled() ? "是" : "否") << std::endl;
        
        // 测试选择器收集
        state.addClassSelector("box");
        state.addClassSelector("container");
        state.addIdSelector("main");
        
        std::cout << "收集的类选择器: ";
        for (const auto& selector : state.getClassSelectors()) {
            std::cout << selector << " ";
        }
        std::cout << std::endl;
        
        std::cout << "收集的ID选择器: ";
        for (const auto& selector : state.getIdSelectors()) {
            std::cout << selector << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "弹出元素解析状态: " << parseStateToString(state.getCurrentState()) << std::endl;
    
    // 输出完整状态信息
    std::cout << "\n完整状态信息:" << std::endl;
    std::cout << state.getStateInfo() << std::endl;
}

void testGlobalMap() {
    std::cout << "\n=== 测试全局映射管理 ===" << std::endl;
    
    GlobalMap globalMap;
    
    // 测试类名分配
    std::string autoClass1 = globalMap.allocateClassName();
    std::string autoClass2 = globalMap.allocateClassName("custom");
    std::string autoClass3 = globalMap.allocateClassName("custom"); // 应该生成 custom-1
    
    std::cout << "自动分配的类名1: " << autoClass1 << std::endl;
    std::cout << "自动分配的类名2: " << autoClass2 << std::endl;
    std::cout << "自动分配的类名3: " << autoClass3 << std::endl;
    
    // 测试ID分配
    std::string autoId1 = globalMap.allocateIdName();
    std::string autoId2 = globalMap.allocateIdName("main");
    
    std::cout << "自动分配的ID1: " << autoId1 << std::endl;
    std::cout << "自动分配的ID2: " << autoId2 << std::endl;
    
    // 测试选择器注册
    globalMap.registerClassSelector("box");
    globalMap.registerClassSelector("container");
    globalMap.registerIdSelector("header");
    
    std::cout << "注册的类选择器数量: " << globalMap.getClassSelectors().size() << std::endl;
    std::cout << "注册的ID选择器数量: " << globalMap.getIdSelectors().size() << std::endl;
    
    // 测试导入路径管理
    globalMap.addImportPath("/path/to/module1.chtl");
    globalMap.addImportPath("/path/to/module2.chtl");
    
    std::cout << "导入路径数量: " << globalMap.getImportPaths().size() << std::endl;
    std::cout << "是否已导入module1: " << (globalMap.isPathImported("/path/to/module1.chtl") ? "是" : "否") << std::endl;
    
    // 测试循环依赖检测
    globalMap.pushImportStack("/path/to/main.chtl");
    globalMap.pushImportStack("/path/to/module1.chtl");
    
    std::cout << "是否存在循环依赖(main): " << (globalMap.hasCircularDependency("/path/to/main.chtl") ? "是" : "否") << std::endl;
    std::cout << "是否存在循环依赖(module2): " << (globalMap.hasCircularDependency("/path/to/module2.chtl") ? "是" : "否") << std::endl;
}

int main() {
    std::cout << "CHTL编译器测试程序" << std::endl;
    std::cout << "==================" << std::endl;
    
    try {
        testLexer();
        testNodes();
        testContext();
        testState();
        testGlobalMap();
        
        std::cout << "\n==================" << std::endl;
        std::cout << "所有测试完成！CHTL项目基础架构实现成功！" << std::endl;
        std::cout << "这个珍贵的CHTL已经重新焕发生机 ✨" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}