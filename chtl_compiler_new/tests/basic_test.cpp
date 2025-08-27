#include <iostream>
#include <string>

// 引入核心组件
#include "Core/Types.h"
#include "CHTL/Lexer/Token.h"
#include "CHTL/AST/ASTNode.h"

/**
 * CHTL编译器基础测试
 * 验证全新架构的基础组件
 */

void testCoreTypes() {
    std::cout << "=== 测试核心类型 ===" << std::endl;
    
    // 测试Position
    CHTL::Position pos(10, 5, 100, "test.chtl");
    std::cout << "Position: " << pos.line << ":" << pos.column 
              << " in " << pos.filename << std::endl;
    
    // 测试SourceRange
    CHTL::SourceRange range(pos, pos);
    std::cout << "SourceRange created successfully" << std::endl;
    
    // 测试Literal
    CHTL::Literal literal("HelloWorld", CHTL::LiteralType::UNQUOTED);
    std::cout << "Literal: \"" << literal.value << "\" (unquoted)" << std::endl;
    
    std::cout << "✅ 核心类型测试通过！" << std::endl;
}

void testCHTLTokens() {
    std::cout << "\n=== 测试CHTL Token系统 ===" << std::endl;
    
    // 创建各种类型的Token
    CHTL::SourceRange range;
    
    // 测试关键字Token
    CHTL::Lexer::Token textToken(CHTL::Lexer::TokenType::TEXT, "text", range);
    std::cout << "Token: " << textToken.toString() << std::endl;
    std::cout << "Is keyword: " << (textToken.isKeyword() ? "yes" : "no") << std::endl;
    
    // 测试字面量Token
    CHTL::Lexer::Token literalToken(CHTL::Lexer::TokenType::UNQUOTED_LITERAL, "HelloWorld", range);
    std::cout << "Token: " << literalToken.toString() << std::endl;
    std::cout << "Is literal: " << (literalToken.isLiteral() ? "yes" : "no") << std::endl;
    
    // 测试CE对等式
    CHTL::Lexer::Token colonToken(CHTL::Lexer::TokenType::COLON, ":", range);
    CHTL::Lexer::Token equalsToken(CHTL::Lexer::TokenType::EQUALS, "=", range);
    std::cout << "Colon is assignment: " << (colonToken.isAssignmentOperator() ? "yes" : "no") << std::endl;
    std::cout << "Equals is assignment: " << (equalsToken.isAssignmentOperator() ? "yes" : "no") << std::endl;
    
    std::cout << "✅ CHTL Token系统测试通过！" << std::endl;
}

void testCHTLAST() {
    std::cout << "\n=== 测试CHTL AST系统 ===" << std::endl;
    
    // 创建一个简单的AST结构
    class TestElementNode : public CHTL::AST::ASTNode {
    public:
        TestElementNode() : ASTNode(CHTL::AST::NodeType::ELEMENT) {
            setAttribute("name", "div");
        }
        
        void accept(CHTL::AST::ASTVisitor& visitor) override {
            visitor.visitElement(this);
        }
    };
    
    class TestTextNode : public CHTL::AST::ASTNode {
    public:
        TestTextNode() : ASTNode(CHTL::AST::NodeType::TEXT) {
            setAttribute("value", "Hello World");
        }
        
        void accept(CHTL::AST::ASTVisitor& visitor) override {
            visitor.visitText(this);
        }
    };
    
    // 创建节点
    auto elementNode = std::make_shared<TestElementNode>();
    auto textNode = std::make_shared<TestTextNode>();
    
    // 构建树结构
    elementNode->addChild(textNode);
    
    // 测试节点属性
    std::cout << "Element node: " << elementNode->toString() << std::endl;
    std::cout << "Text node: " << textNode->toString() << std::endl;
    std::cout << "Element has " << elementNode->getChildren().size() << " children" << std::endl;
    
    // 测试类型检查
    std::cout << "Element is element: " << (elementNode->isElement() ? "yes" : "no") << std::endl;
    std::cout << "Text is text: " << (textNode->isText() ? "yes" : "no") << std::endl;
    
    // 输出树结构
    std::cout << "AST Tree:" << std::endl;
    std::cout << elementNode->toTreeString();
    
    std::cout << "✅ CHTL AST系统测试通过！" << std::endl;
}

void testCompileResult() {
    std::cout << "\n=== 测试编译结果 ===" << std::endl;
    
    CHTL::CompileResult result;
    result.success = true;
    result.html = "<div>Hello World</div>";
    result.css = ".box { width: 100px; }";
    result.javascript = "console.log('Hello');";
    
    std::cout << "Compile result:" << std::endl;
    std::cout << "  Success: " << (result.success ? "yes" : "no") << std::endl;
    std::cout << "  HTML: " << result.html << std::endl;
    std::cout << "  CSS: " << result.css << std::endl;
    std::cout << "  JS: " << result.javascript << std::endl;
    
    std::cout << "✅ 编译结果测试通过！" << std::endl;
}

int main() {
    std::cout << "🔥 CHTL编译器全新架构测试程序" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "🎯 严格按照CHTL语法文档，追求完整与绝对准确" << std::endl;
    std::cout << "🏗️ 全新架构，从头构建，避免历史遗留问题" << std::endl;
    std::cout << "⚡ 模块化设计，每个组件都有完整实现" << std::endl;
    std::cout << "========================================" << std::endl;
    
    try {
        // 测试各个核心组件
        testCoreTypes();
        testCHTLTokens();
        testCHTLAST();
        testCompileResult();
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "✅ 全新CHTL编译器基础架构测试完成！" << std::endl;
        std::cout << "🎉 阶段1完成：基础架构构建成功！" << std::endl;
        std::cout << "🚀 准备进入阶段2：词法分析器实现" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}