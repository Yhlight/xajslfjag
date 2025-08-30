#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLNode/ElementNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void testConstraintParsing() {
    std::cout << "\n=== 测试约束解析 ===" << std::endl;
    
    std::string constraintCode = R"(div
{
    except span, [Custom] @Element Box;
    
    span
    {
        // 这应该被约束阻止
    }
})";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(constraintCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success) {
            std::cout << "✓ 约束解析成功" << std::endl;
            std::cout << "  - 根节点子节点数: " << (ast.rootNode ? ast.rootNode->getChildren().size() : 0) << std::endl;
            
            if (ast.rootNode && ast.rootNode->getChildren().size() > 0) {
                auto divElement = ast.rootNode->getChildren()[0];
                std::cout << "  - div元素子节点数: " << divElement->getChildren().size() << std::endl;
                
                // 检查是否有约束节点
                for (auto child : divElement->getChildren()) {
                    if (child->getNodeType() == CHTLNodeType::CONSTRAINT_NODE) {
                        std::cout << "  - 找到约束节点: " << child->getName() << std::endl;
                        
                        if (auto constraintNode = std::dynamic_pointer_cast<ConstraintNode>(child)) {
                            std::cout << "    - 约束项数: " << constraintNode->getConstraintItems().size() << std::endl;
                            for (const auto& item : constraintNode->getConstraintItems()) {
                                std::cout << "      - " << item.toString() << std::endl;
                            }
                        }
                    }
                }
            }
            
        } else {
            std::cout << "❌ 约束解析失败" << std::endl;
        }
        
        if (errorReporter.hasErrors()) {
            std::cout << "\n解析过程中的错误:" << std::endl;
            errorReporter.printAll();
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 约束测试异常: " << e.what() << std::endl;
    }
}

void testConstraintValidation() {
    std::cout << "\n=== 测试约束验证 ===" << std::endl;
    
    try {
        // 创建约束管理器
        ConstraintManager manager;
        ErrorReporter errorReporter;
        manager.setErrorReporter(&errorReporter);
        
        // 创建约束节点
        auto constraintNode = std::make_shared<ConstraintNode>();
        
        // 添加span约束
        ConstraintItem spanConstraint(ConstraintTargetType::HTML_ELEMENT, "span");
        constraintNode->addConstraintItem(spanConstraint);
        
        // 注册约束
        manager.addConstraint(constraintNode, "test_scope");
        
        // 创建一个span元素
        auto spanElement = std::make_shared<ElementNode>("span");
        
        // 验证是否被约束
        bool isConstrained = manager.isConstrained(ConstraintTargetType::HTML_ELEMENT, "span", "", "test_scope");
        std::cout << "  - span是否被约束: " << (isConstrained ? "是" : "否") << std::endl;
        
        // 验证节点
        bool isValid = manager.validateNode(spanElement, "test_scope");
        std::cout << "  - span节点验证结果: " << (isValid ? "通过" : "失败") << std::endl;
        
        // 创建一个div元素（应该不被约束）
        auto divElement = std::make_shared<ElementNode>("div");
        bool divValid = manager.validateNode(divElement, "test_scope");
        std::cout << "  - div节点验证结果: " << (divValid ? "通过" : "失败") << std::endl;
        
        // 输出约束报告
        std::cout << "\n约束报告:\n" << manager.generateConstraintReport() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 约束验证异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 约束系统测试" << std::endl;
    std::cout << "=================" << std::endl;
    
    testConstraintParsing();
    testConstraintValidation();
    
    std::cout << "\n约束系统测试完成！" << std::endl;
    return 0;
}