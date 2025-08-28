#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../CHTL/CHTLParser/ConstraintParser.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testExactConstraints() {
    std::cout << "=== 测试精确约束 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试HTML元素约束
    std::string source1 = "div {\n    except span, [Custom] @Element Box;\n}";
    auto statements1 = parser.parse(source1, "div");
    
    if (!statements1.empty()) {
        auto statement = statements1[0];
        std::cout << "HTML元素约束解析成功" << std::endl;
        std::cout << "约束类型: " << static_cast<int>(statement->type) << std::endl;
        std::cout << "作用域: " << statement->scope << std::endl;
        std::cout << "约束目标数量: " << statement->targets.size() << std::endl;
        
        for (const auto& target : statement->targets) {
            std::cout << "  目标: " << target.fullPath << " (类型: " << static_cast<int>(target.type) << ")" << std::endl;
        }
        
        // 应用约束
        bool applied = parser.applyConstraintStatement(statement);
        std::cout << "应用约束: " << (applied ? "成功" : "失败") << std::endl;
        
        // 检查约束效果
        bool spanConstrained = parser.isElementConstrained("span", "div");
        bool boxConstrained = parser.isElementConstrained("[Custom] @Element Box", "div");
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
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试类型约束
    std::string source1 = "div {\n    except @Html;\n    except [Template] @Var;\n    except [Custom];\n}";
    auto statements1 = parser.parse(source1, "div");
    
    if (!statements1.empty()) {
        std::cout << "类型约束解析成功，共 " << statements1.size() << " 个约束语句" << std::endl;
        
        for (size_t i = 0; i < statements1.size(); ++i) {
            auto statement = statements1[i];
            std::cout << "约束语句 " << (i + 1) << ":" << std::endl;
            std::cout << "  约束类型: " << static_cast<int>(statement->type) << std::endl;
            std::cout << "  作用域: " << statement->scope << std::endl;
            
            for (const auto& target : statement->targets) {
                std::cout << "    目标: " << target.fullPath << " (类型: " << static_cast<int>(target.type) << ")" << std::endl;
            }
            
            // 应用约束
            bool applied = parser.applyConstraintStatement(statement);
            std::cout << "  应用约束: " << (applied ? "成功" : "失败") << std::endl;
        }
        
        // 检查约束效果
        bool htmlConstrained = parser.isTypeConstrained("@Html", "div");
        bool varConstrained = parser.isTypeConstrained("[Template] @Var", "div");
        bool customConstrained = parser.isTypeConstrained("[Custom]", "div");
        
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
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试全局约束
    std::string source1 = "[Namespace] space {\n    except [Template];\n}";
    auto statements1 = parser.parse(source1, "");
    
    if (!statements1.empty()) {
        auto statement = statements1[0];
        std::cout << "全局约束解析成功" << std::endl;
        std::cout << "约束类型: " << static_cast<int>(statement->type) << std::endl;
        std::cout << "作用域: " << (statement->scope.empty() ? "(全局)" : statement->scope) << std::endl;
        std::cout << "约束目标数量: " << statement->targets.size() << std::endl;
        
        for (const auto& target : statement->targets) {
            std::cout << "  目标: " << target.fullPath << " (类型: " << static_cast<int>(target.type) << ")" << std::endl;
        }
        
        // 应用约束
        bool applied = parser.applyConstraintStatement(statement);
        std::cout << "应用约束: " << (applied ? "成功" : "失败") << std::endl;
        
        // 检查约束效果
        bool templateConstrained = parser.isTypeConstrained("[Template]", "");
        std::cout << "[Template]被全局约束: " << (templateConstrained ? "是" : "否") << std::endl;
    } else {
        std::cout << "全局约束解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testComplexConstraints() {
    std::cout << "=== 测试复杂约束 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试复杂约束（混合类型）
    std::string source1 = "div {\n    except span, [Custom] @Element Box, @Html, [Template] @Var;\n}";
    auto statements1 = parser.parse(source1, "div");
    
    if (!statements1.empty()) {
        auto statement = statements1[0];
        std::cout << "复杂约束解析成功" << std::endl;
        std::cout << "约束类型: " << static_cast<int>(statement->type) << std::endl;
        std::cout << "约束目标数量: " << statement->targets.size() << std::endl;
        
        for (const auto& target : statement->targets) {
            std::cout << "  目标: " << target.fullPath << " (类型: " << static_cast<int>(target.type) << ")" << std::endl;
        }
        
        // 应用约束
        bool applied = parser.applyConstraintStatement(statement);
        std::cout << "应用约束: " << (applied ? "成功" : "失败") << std::endl;
        
        // 检查各种约束效果
        std::cout << "约束检查结果:" << std::endl;
        std::cout << "  span: " << (parser.isElementConstrained("span", "div") ? "被约束" : "未约束") << std::endl;
        std::cout << "  Box: " << (parser.isElementConstrained("[Custom] @Element Box", "div") ? "被约束" : "未约束") << std::endl;
        std::cout << "  @Html: " << (parser.isTypeConstrained("@Html", "div") ? "被约束" : "未约束") << std::endl;
        std::cout << "  [Template] @Var: " << (parser.isTypeConstrained("[Template] @Var", "div") ? "被约束" : "未约束") << std::endl;
    } else {
        std::cout << "复杂约束解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testInvalidConstraints() {
    std::cout << "=== 测试无效约束 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试1: 空的except语句
    std::string source1 = "div {\n    except;\n}";
    auto statements1 = parser.parse(source1, "div");
    
    if (!statements1.empty()) {
        auto statement = statements1[0];
        std::cout << "空except语句解析结果:" << std::endl;
        std::cout << "  约束目标数量: " << statement->targets.size() << std::endl;
        
        bool valid = parser.validateConstraintStatement(statement);
        std::cout << "  验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "    验证错误: " << error << std::endl;
            }
        }
    }
    
    // 测试2: 语法错误的except语句
    std::string source2 = "div {\n    except span div;\n}";
    auto statements2 = parser.parse(source2, "div");
    
    if (!statements2.empty()) {
        auto statement = statements2[0];
        std::cout << "语法错误except语句解析结果:" << std::endl;
        std::cout << "  约束目标数量: " << statement->targets.size() << std::endl;
        
        for (const auto& target : statement->targets) {
            std::cout << "    目标: " << target.fullPath << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testConstraintScoping() {
    std::cout << "=== 测试约束作用域 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ConstraintParser parser(configManager);
    
    // 测试不同作用域的约束
    std::string source1 = "div {\n    except span;\n}\n\np {\n    except div;\n}";
    auto statements1 = parser.parse(source1, "div");
    auto statements2 = parser.parse(source1, "p");
    
    std::cout << "div作用域约束:" << std::endl;
    for (const auto& statement : statements1) {
        parser.applyConstraintStatement(statement);
        for (const auto& target : statement->targets) {
            std::cout << "  约束: " << target.fullPath << std::endl;
        }
    }
    
    std::cout << "p作用域约束:" << std::endl;
    for (const auto& statement : statements2) {
        parser.applyConstraintStatement(statement);
        for (const auto& target : statement->targets) {
            std::cout << "  约束: " << target.fullPath << std::endl;
        }
    }
    
    // 检查作用域隔离
    bool spanInDiv = parser.isElementConstrained("span", "div");
    bool spanInP = parser.isElementConstrained("span", "p");
    bool divInP = parser.isElementConstrained("div", "p");
    
    std::cout << "作用域隔离检查:" << std::endl;
    std::cout << "  span在div中: " << (spanInDiv ? "被约束" : "未约束") << std::endl;
    std::cout << "  span在p中: " << (spanInP ? "被约束" : "未约束") << std::endl;
    std::cout << "  div在p中: " << (divInP ? "被约束" : "未约束") << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "CHTL 约束解析器测试程序" << std::endl;
    std::cout << "========================" << std::endl << std::endl;
    
    try {
        testExactConstraints();
        testTypeConstraints();
        testGlobalConstraints();
        testComplexConstraints();
        testInvalidConstraints();
        testConstraintScoping();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}