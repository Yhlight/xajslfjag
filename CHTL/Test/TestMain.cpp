#include <iostream>
#include <memory>
#include <string>

// 包含CHTL核心头文件
#include "../CHTL/CHTLLexer/Lexer.hpp"
#include "../CHTL/CHTLLexer/Token.hpp"
#include "../CHTL/CHTLLexer/GlobalMap.hpp"
#include "../CHTL/CHTLState/CHTLState.hpp"
#include "../CHTL/CHTLNode/BaseNode.hpp"
#include "../CHTL/CHTLContext/CHTLContext.hpp"

using namespace CHTL;

/**
 * 测试词法分析器
 */
void testLexer() {
    std::cout << "=== 测试CHTL词法分析器 ===\n";
    
    std::string testCode = R"(
        html {
            head {
                title {
                    text { "CHTL测试页面" }
                }
            }
            
            body {
                div {
                    id: main-container;
                    class: welcome;
                    
                    style {
                        .welcome {
                            color: red;
                            background-color: blue;
                        }
                        
                        &:hover {
                            opacity: 0.8;
                        }
                    }
                    
                    text { "欢迎使用CHTL!" }
                }
            }
        }
    )";
    
    try {
        Lexer lexer(testCode);
        auto tokens = lexer.tokenize();
        
        std::cout << "成功解析 " << tokens.size() << " 个词法单元:\n";
        
        for (size_t i = 0; i < std::min(tokens.size(), size_t(20)); ++i) {
            std::cout << "  " << tokens[i].toString() << "\n";
        }
        
        if (tokens.size() > 20) {
            std::cout << "  ... 还有 " << (tokens.size() - 20) << " 个词法单元\n";
        }
        
        std::cout << "词法分析测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "词法分析测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 测试全局映射
 */
void testGlobalMap() {
    std::cout << "=== 测试全局映射管理器 ===\n";
    
    try {
        auto& globalMap = GlobalMap::getInstance();
        
        // 测试配置设置
        GlobalMap::Configuration config;
        config.DEBUG_MODE = true;
        config.DISABLE_STYLE_AUTO_ADD_CLASS = false;
        globalMap.setConfiguration(config);
        
        // 测试模板注册
        GlobalMap::TemplateInfo templateInfo;
        templateInfo.name = "TestTemplate";
        templateInfo.type = "Style";
        templateInfo.content = "color: red;";
        templateInfo.namespacePath = "test";
        
        globalMap.registerStyleTemplate("TestTemplate", templateInfo);
        
        // 测试模板查找
        auto* foundTemplate = globalMap.getStyleTemplate("TestTemplate");
        if (foundTemplate && foundTemplate->name == "TestTemplate") {
            std::cout << "模板注册和查找 ✓ 通过\n";
        } else {
            std::cout << "模板注册和查找 ✗ 失败\n";
        }
        
        // 测试命名空间
        globalMap.enterNamespace("test");
        std::string currentNS = globalMap.getCurrentNamespace();
        if (currentNS == "test") {
            std::cout << "命名空间管理 ✓ 通过\n";
        } else {
            std::cout << "命名空间管理 ✗ 失败\n";
        }
        
        globalMap.dumpState();
        std::cout << "全局映射测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "全局映射测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 测试状态管理
 */
void testStateManager() {
    std::cout << "=== 测试状态管理器 ===\n";
    
    try {
        CHTLState state;
        
        // 测试状态切换
        state.pushState(ParseState::IN_ELEMENT, ContextType::ELEMENT);
        state.setCurrentElement("div");
        
        if (state.getCurrentState() == ParseState::IN_ELEMENT &&
            state.getCurrentElement() == "div") {
            std::cout << "状态切换 ✓ 通过\n";
        } else {
            std::cout << "状态切换 ✗ 失败\n";
        }
        
        // 测试属性管理
        state.setAttribute("class", "test-class");
        if (state.hasAttribute("class") && 
            state.getAttribute("class") == "test-class") {
            std::cout << "属性管理 ✓ 通过\n";
        } else {
            std::cout << "属性管理 ✗ 失败\n";
        }
        
        // 测试约束管理
        state.pushConstraint("no-templates");
        if (state.isConstraintActive("no-templates")) {
            std::cout << "约束管理 ✓ 通过\n";
        } else {
            std::cout << "约束管理 ✗ 失败\n";
        }
        
        state.dumpState();
        std::cout << "状态管理测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "状态管理测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 测试AST节点
 */
void testASTNodes() {
    std::cout << "=== 测试AST节点系统 ===\n";
    
    try {
        // 创建元素节点
        auto divElement = std::make_shared<ElementNode>("div");
        divElement->setAttribute("id", "test-div");
        divElement->addClass("container");
        
        // 创建文本节点
        auto textNode = std::make_shared<TextNode>("Hello, CHTL!");
        
        // 建立父子关系
        divElement->addChild(textNode);
        
        // 测试属性
        if (divElement->hasAttribute("id") && 
            divElement->getAttribute("id").value == "test-div") {
            std::cout << "元素属性 ✓ 通过\n";
        } else {
            std::cout << "元素属性 ✗ 失败\n";
        }
        
        // 测试CSS类
        if (divElement->hasClass("container")) {
            std::cout << "CSS类管理 ✓ 通过\n";
        } else {
            std::cout << "CSS类管理 ✗ 失败\n";
        }
        
        // 测试子节点
        if (divElement->getChildCount() == 1 && 
            divElement->getChild(0) == textNode) {
            std::cout << "父子关系 ✓ 通过\n";
        } else {
            std::cout << "父子关系 ✗ 失败\n";
        }
        
        // 测试HTML生成
        std::string html = divElement->toHTML();
        if (!html.empty() && html.find("test-div") != std::string::npos) {
            std::cout << "HTML生成 ✓ 通过\n";
        } else {
            std::cout << "HTML生成 ✗ 失败\n";
        }
        
        divElement->dumpTree();
        std::cout << "AST节点测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "AST节点测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 测试编译上下文
 */
void testCompilationContext() {
    std::cout << "=== 测试编译上下文 ===\n";
    
    try {
        auto context = std::make_unique<CHTLContext>();
        
        // 设置编译上下文
        CompilationContext compCtx;
        compCtx.sourceFile = "test.chtl";
        compCtx.debugMode = true;
        context->setCompilationContext(compCtx);
        
        // 测试作用域管理
        auto& scopeManager = context->getScopeManager();
        scopeManager.pushScope("test", ContextType::ELEMENT);
        scopeManager.setVariable("testVar", "testValue");
        
        if (scopeManager.hasVariable("testVar") && 
            scopeManager.getVariable("testVar") == "testValue") {
            std::cout << "作用域管理 ✓ 通过\n";
        } else {
            std::cout << "作用域管理 ✗ 失败\n";
        }
        
        // 测试错误报告
        context->reportError("测试错误");
        context->reportWarning("测试警告");
        
        if (context->hasErrors() && context->hasWarnings()) {
            std::cout << "错误报告 ✓ 通过\n";
        } else {
            std::cout << "错误报告 ✗ 失败\n";
        }
        
        context->dumpContext();
        std::cout << "编译上下文测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "编译上下文测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 综合测试
 */
void integrationTest() {
    std::cout << "=== 综合测试 ===\n";
    
    try {
        // 创建完整的编译环境
        auto context = std::make_unique<CHTLContext>();
        context->enableDebugMode(true);
        
        // 设置工作目录
        context->setWorkingDirectory("/workspace");
        context->setOutputDirectory("/workspace/output");
        
        // 创建简单的AST
        auto rootNode = std::make_shared<BaseNode>(NodeType::DOCUMENT, "document");
        auto htmlNode = std::make_shared<ElementNode>("html");
        auto bodyNode = std::make_shared<ElementNode>("body");
        auto divNode = std::make_shared<ElementNode>("div");
        
        divNode->setAttribute("class", "test-container");
        divNode->setId("main");
        
        auto textNode = std::make_shared<TextNode>("CHTL综合测试成功!");
        
        // 建立层次结构
        divNode->addChild(textNode);
        bodyNode->addChild(divNode);
        htmlNode->addChild(bodyNode);
        rootNode->addChild(htmlNode);
        
        context->setRootNode(rootNode);
        
        // 生成HTML输出
        std::string htmlOutput = rootNode->toHTML();
        context->setHTMLOutput(htmlOutput);
        
        // 验证结果
        if (!htmlOutput.empty() && 
            htmlOutput.find("test-container") != std::string::npos &&
            htmlOutput.find("CHTL综合测试成功!") != std::string::npos) {
            std::cout << "HTML输出验证 ✓ 通过\n";
        } else {
            std::cout << "HTML输出验证 ✗ 失败\n";
        }
        
        // 显示统计信息
        const auto& stats = context->getStatistics();
        std::cout << "编译统计:\n";
        std::cout << "  - 创建节点数: " << stats.nodesCreated << "\n";
        std::cout << "  - 错误数: " << stats.errorsGenerated << "\n";
        std::cout << "  - 警告数: " << stats.warningsGenerated << "\n";
        
        std::cout << "综合测试 ✓ 通过\n\n";
        
    } catch (const std::exception& e) {
        std::cout << "综合测试 ✗ 失败: " << e.what() << "\n\n";
    }
}

/**
 * 主测试函数
 */
int main() {
    std::cout << "==========================================\n";
    std::cout << "           CHTL项目架构测试程序\n";
    std::cout << "==========================================\n\n";
    
    try {
        // 依次运行各个测试
        testLexer();
        testGlobalMap();
        testStateManager();
        testASTNodes();
        testCompilationContext();
        integrationTest();
        
        std::cout << "==========================================\n";
        std::cout << "           所有测试完成!\n";
        std::cout << "==========================================\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "严重错误: " << e.what() << std::endl;
        return 1;
    }
}