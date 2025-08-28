#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../CHTL/CHTLParser/ConstraintParser.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testExactConstraints() {
    std::cout << "=== 测试精确约束 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试HTML元素约束
    std::string source1 = "div {\n    except span, [Custom] @Element Box;\n}";
    auto statements1 = parser.parse(source1, "test.chtl");
    
    if (!statements1.empty()) {
        auto statement = statements1[0];
        std::cout << "HTML元素约束解析成功" << std::endl;
        std::cout << "约束数量: " << statement.constraints.size() << std::endl;
        std::cout << "行号: " << statement.line << std::endl;
        std::cout << "列号: " << statement.column << std::endl;
        
        for (const auto& constraint : statement.constraints) {
            std::cout << "  目标: " << constraint.value << " (类型: " << static_cast<int>(constraint.type) << ")" << std::endl;
        }
        
        // 检查约束效果
        bool spanConstrained = false;
        bool boxConstrained = false;
        
        for (const auto& constraint : statement.constraints) {
            if (constraint.value == "span") spanConstrained = true;
            if (constraint.value == "[Custom] @Element Box") boxConstrained = true;
        }
        
        std::cout << "span被约束: " << (spanConstrained ? "是" : "否") << std::endl;
        std::cout << "Box被约束: " << (boxConstrained ? "是" : "否") << std::endl;
    } else {
        std::cout << "HTML元素约束解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testTypeConstraints() {
    std::cout << "=== 测试类型约束 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试类型约束
    std::string source1 = "div {\n    except @Html;\n    except [Template] @Var;\n    except [Custom];\n}";
    auto statements1 = parser.parse(source1, "test.chtl");
    
    if (!statements1.empty()) {
        std::cout << "类型约束解析成功，共 " << statements1.size() << " 个约束语句" << std::endl;
        
        for (size_t i = 0; i < statements1.size(); ++i) {
            auto statement = statements1[i];
            std::cout << "约束语句 " << (i + 1) << ":" << std::endl;
            std::cout << "  约束数量: " << statement.constraints.size() << std::endl;
            std::cout << "  行号: " << statement.line << std::endl;
            std::cout << "  列号: " << statement.column << std::endl;
            
            for (const auto& constraint : statement.constraints) {
                std::cout << "    目标: " << constraint.value << " (类型: " << static_cast<int>(constraint.type) << ")" << std::endl;
            }
        }
        
        // 检查约束效果
        bool htmlConstrained = false;
        bool varConstrained = false;
        bool customConstrained = false;
        
        for (const auto& statement : statements1) {
            for (const auto& constraint : statement.constraints) {
                if (constraint.value == "@Html") htmlConstrained = true;
                if (constraint.value == "[Template] @Var") varConstrained = true;
                if (constraint.value == "[Custom]") customConstrained = true;
            }
        }
        
        std::cout << "@Html被约束: " << (htmlConstrained ? "是" : "否") << std::endl;
        std::cout << "[Template] @Var被约束: " << (varConstrained ? "是" : "否") << std::endl;
        std::cout << "[Custom]被约束: " << (customConstrained ? "是" : "否") << std::endl;
    } else {
        std::cout << "类型约束解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testGlobalConstraints() {
    std::cout << "=== 测试全局约束 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试全局约束
    std::string source1 = "global {\n    except [Template];\n    except [Custom];\n}";
    auto statements1 = parser.parse(source1, "test.chtl");
    
    if (!statements1.empty()) {
        std::cout << "全局约束解析成功，共 " << statements1.size() << " 个约束语句" << std::endl;
        
        for (size_t i = 0; i < statements1.size(); ++i) {
            auto statement = statements1[i];
            std::cout << "约束语句 " << (i + 1) << ":" << std::endl;
            std::cout << "  约束数量: " << statement.constraints.size() << std::endl;
            std::cout << "  行号: " << statement.line << std::endl;
            std::cout << "  列号: " << statement.column << std::endl;
            
            for (const auto& constraint : statement.constraints) {
                std::cout << "    目标: " << constraint.value << " (类型: " << static_cast<int>(constraint.type) << ")" << std::endl;
            }
        }
        
        // 检查约束效果
        bool templateConstrained = false;
        bool customConstrained = false;
        
        for (const auto& statement : statements1) {
            for (const auto& constraint : statement.constraints) {
                if (constraint.value == "[Template]") templateConstrained = true;
                if (constraint.value == "[Custom]") customConstrained = true;
            }
        }
        
        std::cout << "[Template]被约束: " << (templateConstrained ? "是" : "否") << std::endl;
        std::cout << "[Custom]被约束: " << (customConstrained ? "是" : "否") << std::endl;
    } else {
        std::cout << "全局约束解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testConstraintValidation() {
    std::cout << "=== 测试约束验证 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试有效约束
    std::string validSource = "div {\n    except span, button;\n}";
    auto validStatements = parser.parse(validSource, "test.chtl");
    
    if (!validStatements.empty()) {
        std::cout << "有效约束解析成功" << std::endl;
        for (const auto& statement : validStatements) {
            for (const auto& constraint : statement.constraints) {
                auto validationErrors = parser.getValidationErrors(constraint);
                if (validationErrors.empty()) {
                    std::cout << "约束验证通过: " << constraint.value << std::endl;
                } else {
                    std::cout << "约束验证失败: " << constraint.value << std::endl;
                    for (const auto& error : validationErrors) {
                        std::cout << "  " << error << std::endl;
                    }
                }
            }
        }
    }
    
    // 测试无效约束
    std::string invalidSource = "div {\n    except invalid-element;\n}";
    auto invalidStatements = parser.parse(invalidSource, "test.chtl");
    
    if (!invalidStatements.empty()) {
        std::cout << "无效约束解析成功" << std::endl;
        for (const auto& statement : invalidStatements) {
            for (const auto& constraint : statement.constraints) {
                auto validationErrors = parser.getValidationErrors(constraint);
                if (!validationErrors.empty()) {
                    std::cout << "约束验证失败（预期）: " << constraint.value << std::endl;
                    for (const auto& error : validationErrors) {
                        std::cout << "  " << error << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << std::endl;
}

void testConstraintTypes() {
    std::cout << "=== 测试约束类型 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试不同类型的约束
    std::string source = "div {\n    except span;\n    except [Template] @Var;\n    except @Html;\n}";
    auto statements = parser.parse(source, "test.chtl");
    
    if (!statements.empty()) {
        std::cout << "约束类型解析成功" << std::endl;
        
        for (const auto& statement : statements) {
            std::cout << "行号: " << statement.line << std::endl;
            
            for (const auto& constraint : statement.constraints) {
                std::cout << "  目标: " << constraint.value << std::endl;
                std::cout << "  类型: " << static_cast<int>(constraint.type) << std::endl;
                
                // 根据类型输出描述
                switch (constraint.type) {
                    case ConstraintType::ELEMENT:
                        std::cout << "    类型描述: HTML元素" << std::endl;
                        break;
                    case ConstraintType::TYPE:
                        std::cout << "    类型描述: 类型约束" << std::endl;
                        break;
                    case ConstraintType::GLOBAL:
                        std::cout << "    类型描述: 全局约束" << std::endl;
                        break;
                    case ConstraintType::GLOBAL:
                        std::cout << "    类型描述: 全局约束" << std::endl;
                        break;
                    default:
                        std::cout << "    类型描述: 未知类型" << std::endl;
                        break;
                }
            }
        }
    }
    
    std::cout << std::endl;
}

void testConstraintScope() {
    std::cout << "=== 测试约束作用域 ===" << std::endl;
    
    ConstraintParser parser;
    
    // 测试不同作用域的约束
    std::string source = "div {\n    except span;\n}\n\nspan {\n    except button;\n}\n\nglobal {\n    except [Template];\n}";
    auto statements = parser.parse(source, "test.chtl");
    
    if (!statements.empty()) {
        std::cout << "约束作用域解析成功，共 " << statements.size() << " 个约束组" << std::endl;
        
        for (const auto& statement : statements) {
            std::cout << "行号: " << statement.line << std::endl;
            std::cout << "约束数量: " << statement.constraints.size() << std::endl;
            
            for (const auto& constraint : statement.constraints) {
                std::cout << "  " << constraint.value << std::endl;
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "开始测试约束解析器..." << std::endl << std::endl;
    
    testExactConstraints();
    testTypeConstraints();
    testGlobalConstraints();
    testConstraintValidation();
    testConstraintTypes();
    testConstraintScope();
    
    std::cout << "约束解析器测试完成！" << std::endl;
    return 0;
}