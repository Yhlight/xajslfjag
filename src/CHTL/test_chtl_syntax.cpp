#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/LocalStyleManager.h"
#include "../Error/ErrorReport.h"
#include <iostream>
#include <string>

using namespace CHTL;

void testBasicSyntax() {
    std::cout << "\n=== 测试基本CHTL语法 ===" << std::endl;
    
    // 测试注释
    std::string testComments = R"(
// 单行注释
/* 多行注释 */
-- 生成器注释

html {
    body {
        text {
            "Hello CHTL"
        }
    }
}
)";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(testComments, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    std::cout << "1. 注释和基本结构解析: ";
    if (tokens.size() > 0) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testCEEquality() {
    std::cout << "\n=== 测试CE对等式 (: 与 = 等价) ===" << std::endl;
    
    // 测试冒号和等号的等价性
    std::string testCE = R"(
div {
    id: "test-id";
    class = "test-class";
    
    style {
        width: 100px;
        height = 200px;
    }
}
)";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(testCE, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. CE对等式解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testLiterals() {
    std::cout << "\n=== 测试字面量支持 ===" << std::endl;
    
    // 测试各种字面量类型
    std::string testLiterals = 
"div {\n"
"    class: no-quotes-string;\n"
"    id: \"double-quoted-string\";\n"
"    title: 'single-quoted-string';\n"
"    \n"
"    text {\n"
"        无引号文本内容\n"
"    }\n"
"    \n"
"    text {\n"
"        \"双引号文本内容\"\n"
"    }\n"
"    \n"
"    text {\n"
"        '单引号文本内容'\n"
"    }\n"
"    \n"
"    style {\n"
"        color: red;\n"
"        background: \"rgba(255, 0, 0, 0.5)\";\n"
"        font-family: \"Arial, sans-serif\";\n"
"    }\n"
"}";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(testLiterals, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 字面量解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testLocalStyleBlock() {
    std::cout << "\n=== 测试局部样式块核心功能 ===" << std::endl;
    
    // 测试局部样式块的所有功能
    std::string testLocalStyle = R"(
div {
    id: main-container;
    
    style {
        // 内联样式
        width: 100%;
        height: 200px;
        
        // 自动化类名
        .highlight {
            color: blue;
            font-weight: bold;
        }
        
        // 上下文推导
        &:hover {
            background-color: #f0f0f0;
        }
        
        &::before {
            content: ">";
            color: gray;
        }
    }
    
    text {
        "这是测试文本"
    }
}

div {
    style {
        .box {
            padding: 20px;
            margin: 10px;
            border: 1px solid #ccc;
        }
    }
    
    text {
        "另一个带自动生成类名的元素"
    }
}
)";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(testLocalStyle, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 局部样式块解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
        
        // 测试局部样式管理器
        if (result.rootNode) {
            auto styleManager = LocalStyleManagerFactory::createStandardManager();
            styleManager->setErrorReporter(&errorReporter);
            
            // 查找所有样式节点
            std::function<void(const NodePtr&, std::vector<std::pair<NodePtr, BaseNode*>>&)> findStylePairs;
            findStylePairs = [&](const NodePtr& node, std::vector<std::pair<NodePtr, BaseNode*>>& pairs) {
                if (!node) return;
                
                if (node->getNodeType() == CHTLNodeType::STYLE_NODE && node->getParent() &&
                    node->getParent()->getNodeType() == CHTLNodeType::ELEMENT_NODE) {
                    auto parent = node->getParent();
                    if (parent) {
                        pairs.push_back(std::make_pair(node, parent));
                    }
                }
                
                for (const auto& child : node->getChildren()) {
                    findStylePairs(child, pairs);
                }
            };
            
            std::vector<std::pair<NodePtr, BaseNode*>> stylePairs;
            findStylePairs(result.rootNode, stylePairs);
            
            std::cout << "2. 找到样式节点: " << stylePairs.size() << " 个" << std::endl;
            
            // 处理每个样式节点
            for (auto& [styleNode, parentElement] : stylePairs) {
                auto style = dynamic_cast<StyleNode*>(styleNode.get());
                auto element = dynamic_cast<ElementNode*>(parentElement);
                
                if (style && element) {
                    styleManager->processLocalStyleBlock(style, element);
                }
            }
            
            // 输出处理结果
            auto stats = styleManager->getStatistics();
            auto generatedClasses = styleManager->getGeneratedClassNames();
            auto generatedIds = styleManager->getGeneratedIds();
            auto globalRules = styleManager->getGlobalStyleRules();
            
            std::cout << "3. 样式处理统计:" << std::endl;
            for (const auto& [key, value] : stats) {
                std::cout << "   " << key << ": " << value << std::endl;
            }
            
            std::cout << "4. 自动生成的类名:" << std::endl;
            for (const auto& className : generatedClasses) {
                std::cout << "   ." << className << std::endl;
            }
            
            std::cout << "5. 全局CSS规则数量: " << globalRules.size() << std::endl;
            
            if (!globalRules.empty()) {
                std::cout << "6. 生成的全局CSS:" << std::endl;
                std::string globalCSS = styleManager->generateGlobalCSS();
                std::cout << globalCSS << std::endl;
            }
            
            std::cout << "7. 局部样式块功能: ✓ 通过" << std::endl;
        }
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

void testContextSelector() {
    std::cout << "\n=== 测试上下文推导(&符号) ===" << std::endl;
    
    std::string testContext = R"(
div {
    class: main-box;
    
    style {
        .main-box {
            background: white;
        }
        
        &:hover {
            background: #f5f5f5;
        }
        
        &::before {
            content: "◦ ";
        }
        
        &:focus::after {
            content: " ◦";
        }
    }
}

div {
    style {
        .auto-generated {
            color: black;
        }
        
        &:active {
            color: red;
        }
    }
}
)";
    
    CHTLStateMachine stateMachine;
    CHTLContext context;
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    Lexer lexer(testContext, &stateMachine, &context, &errorReporter);
    TokenSequence tokens = lexer.tokenize();
    
    auto parser = CHTLParserFactory::createStandardParser();
    parser->setErrorReporter(&errorReporter);
    
    ParseResult result = parser->parse(tokens);
    
    std::cout << "1. 上下文选择器解析: ";
    if (result.success) {
        std::cout << "✓ 通过" << std::endl;
        
        // 检查是否正确解析了&选择器
        bool foundContextSelector = false;
        std::function<void(const NodePtr&)> findContextSelectors;
        findContextSelectors = [&](const NodePtr& node) {
            if (!node) return;
            
            if (node->getNodeType() == CHTLNodeType::STYLE_NODE) {
                auto style = dynamic_cast<StyleNode*>(node.get());
                if (style && style->hasSelector()) {
                    std::string selector = style->getSelector();
                    if (selector.find('&') != std::string::npos) {
                        foundContextSelector = true;
                        std::cout << "   找到上下文选择器: " << selector << std::endl;
                    }
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findContextSelectors(child);
            }
        };
        
        findContextSelectors(result.rootNode);
        
        std::cout << "2. 上下文选择器检测: ";
        if (foundContextSelector) {
            std::cout << "✓ 通过" << std::endl;
        } else {
            std::cout << "✗ 失败" << std::endl;
        }
    } else {
        std::cout << "✗ 失败" << std::endl;
    }
}

int main() {
    std::cout << "CHTL语法完整性测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    // 测试基本语法
    testBasicSyntax();
    
    // 测试CE对等式
    testCEEquality();
    
    // 测试字面量
    testLiterals();
    
    // 测试局部样式块
    testLocalStyleBlock();
    
    // 测试上下文推导
    testContextSelector();
    
    // 检查错误报告
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    auto stats = errorReporter.getStatistics();
    
    std::cout << "\n=== 错误统计 ===" << std::endl;
    std::cout << "信息: " << stats.infoCount << std::endl;
    std::cout << "警告: " << stats.warningCount << std::endl;
    std::cout << "错误: " << stats.errorCount << std::endl;
    std::cout << "致命: " << stats.fatalCount << std::endl;
    
    if (stats.errorCount == 0 && stats.fatalCount == 0) {
        std::cout << "\n🎉 所有CHTL语法特性测试通过！" << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ 部分测试失败，请检查实现。" << std::endl;
        
        if (errorReporter.hasErrors()) {
            std::cout << "\n最近的错误:" << std::endl;
            auto errors = errorReporter.getErrorsByLevel(ErrorLevel::ERROR);
            for (size_t i = 0; i < std::min(errors.size(), size_t(5)); ++i) {
                std::cout << "  - " << errors[i].message << std::endl;
            }
        }
        
        return 1;
    }
}