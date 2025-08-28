#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../CHTL/CHTLParser/UseStatementParser.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testHTML5Type() {
    std::cout << "=== 测试HTML5类型use语句 ===" << std::endl;
    
    UseStatementParser parser;
    
    // 测试有效的HTML5 use语句
    std::string source1 = "use html5;\n[Custom] @Element Box { div { } }";
    auto statements1 = parser.parse(source1);
    
    if (!statements1.empty()) {
        auto statement1 = statements1[0];
        std::cout << "HTML5 use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1.type) << std::endl;
        std::cout << "HTML5类型: " << statement1.value << std::endl;
        std::cout << "行号: " << statement1.line << std::endl;
        std::cout << "列号: " << statement1.column << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement1);
            for (const auto& error : errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
    } else {
        std::cout << "HTML5 use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testConfigGroup() {
    std::cout << "=== 测试配置组use语句 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    
    // 创建测试配置组
    configManager->createConfigurationGroup("Basic");
    configManager->createConfigurationGroup("Advanced");
    
    UseStatementParser parser;
    
    // 测试有效的配置组use语句
    std::string source1 = "use @Config Basic;\n[Custom] @Element Box { div { } }";
    auto statements1 = parser.parse(source1);
    
    if (!statements1.empty()) {
        auto statement1 = statements1[0];
        std::cout << "配置组use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1.type) << std::endl;
        std::cout << "配置组名称: " << statement1.value << std::endl;
        std::cout << "行号: " << statement1.line << std::endl;
        std::cout << "列号: " << statement1.column << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement1);
            for (const auto& error : errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
        
        // 检查当前激活的配置组
        std::string activeGroup = configManager->getActiveConfigurationGroup();
        std::cout << "当前激活的配置组: " << (activeGroup.empty() ? "(无名)" : activeGroup) << std::endl;
    } else {
        std::cout << "配置组use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testFullConfigGroup() {
    std::cout << "=== 测试全缀名配置组use语句 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    
    // 创建测试配置组
    configManager->createConfigurationGroup("FullConfig");
    
    UseStatementParser parser;
    
    // 测试有效的全缀名配置组use语句
    std::string source1 = "use [Configuration] @Config FullConfig;\n[Custom] @Element Box { div { } }";
    auto statements1 = parser.parse(source1);
    
    if (!statements1.empty()) {
        auto statement1 = statements1[0];
        std::cout << "全缀名配置组use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1.type) << std::endl;
        std::cout << "配置组名称: " << statement1.value << std::endl;
        std::cout << "行号: " << statement1.line << std::endl;
        std::cout << "列号: " << statement1.column << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement1);
            for (const auto& error : errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
    } else {
        std::cout << "全缀名配置组use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testMixedUseStatements() {
    std::cout << "=== 测试混合use语句 ===" << std::endl;
    
    UseStatementParser parser;
    
    // 测试混合use语句
    std::string source1 = "use html5;\nuse @Config Basic;\n[Custom] @Element Box { div { } }";
    auto statements1 = parser.parse(source1);
    
    if (!statements1.empty()) {
        auto statement1 = statements1[0];
        std::cout << "混合use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1.type) << std::endl;
        std::cout << "HTML5类型: " << statement1.value << std::endl;
        std::cout << "行号: " << statement1.line << std::endl;
        std::cout << "列号: " << statement1.column << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement1);
            for (const auto& error : errors) {
                std::cout << "错误: " << error << std::endl;
            }
        }
    } else {
        std::cout << "混合use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testInvalidUseStatements() {
    std::cout << "=== 测试无效use语句 ===" << std::endl;
    
    UseStatementParser parser;
    
    // 测试1: 无效的HTML5类型
    std::string source1 = "use html4;\n[Custom] @Element Box { div { } }";
    auto statements1 = parser.parse(source1);
    
    if (!statements1.empty()) {
        auto statement1 = statements1[0];
        std::cout << "无效HTML5类型解析结果:" << std::endl;
        std::cout << "  类型: " << static_cast<int>(statement1.type) << std::endl;
        std::cout << "  HTML5类型: " << statement1.value << std::endl;
        
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "  验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement1);
            for (const auto& error : errors) {
                std::cout << "    验证错误: " << error << std::endl;
            }
        }
    }
    
    // 测试2: 无效的配置组名称
    std::string source2 = "use @Config InvalidGroup;\n[Custom] @Element Box { div { } }";
    auto statements2 = parser.parse(source2);
    
    if (!statements2.empty()) {
        auto statement2 = statements2[0];
        std::cout << "无效配置组名称解析结果:" << std::endl;
        std::cout << "  类型: " << static_cast<int>(statement2.type) << std::endl;
        std::cout << "  配置组名称: " << statement2.value << std::endl;
        
        bool valid = parser.validateUseStatement(statement2);
        std::cout << "  验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement2);
            for (const auto& error : errors) {
                std::cout << "    验证错误: " << error << std::endl;
            }
        }
    }
    
    // 测试3: 语法错误的use语句
    std::string source3 = "use;\n[Custom] @Element Box { div { } }";
    auto statements3 = parser.parse(source3);
    
    if (!statements3.empty()) {
        auto statement3 = statements3[0];
        std::cout << "语法错误use语句解析结果:" << std::endl;
        std::cout << "  类型: " << static_cast<int>(statement3.type) << std::endl;
        
        bool valid = parser.validateUseStatement(statement3);
        std::cout << "  验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getValidationErrors(statement3);
            for (const auto& error : errors) {
                std::cout << "    验证错误: " << error << std::endl;
            }
        }
    }
    
    std::cout << std::endl;
}

void testUseStatementParsing() {
    std::cout << "=== 测试use语句解析 ===" << std::endl;
    
    UseStatementParser parser;
    
    // 测试各种use语句格式
    std::vector<std::string> testSources = {
        "use html5;",
        "use @Config Basic;",
        "use [Configuration] @Config Advanced;",
        "use html5;\nuse @Config Basic;",
        "use @Config Basic;\nuse html5;"
    };
    
    for (size_t i = 0; i < testSources.size(); ++i) {
        std::cout << "测试用例 " << (i + 1) << ": " << testSources[i] << std::endl;
        
        auto statements = parser.parse(testSources[i]);
        if (!statements.empty()) {
            auto statement = statements[0];
            std::cout << "  解析成功" << std::endl;
            std::cout << "  类型: " << static_cast<int>(statement.type) << std::endl;
            
            if (statement.type == UseStatementType::HTML5) {
                std::cout << "  HTML5类型: " << statement.value << std::endl;
            } else if (statement.type == UseStatementType::CONFIG) {
                std::cout << "  配置组名称: " << statement.value << std::endl;
            }
            
            std::cout << "  行号: " << statement.line << std::endl;
            std::cout << "  列号: " << statement.column << std::endl;
            
            bool valid = parser.validateUseStatement(statement);
            std::cout << "  验证结果: " << (valid ? "通过" : "失败") << std::endl;
        } else {
            std::cout << "  解析失败" << std::endl;
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "    错误: " << error << std::endl;
            }
        }
        std::cout << std::endl;
    }
}

int main() {
    std::cout << "CHTL Use语句解析器测试程序" << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
    
    try {
        testHTML5Type();
        testConfigGroup();
        testFullConfigGroup();
        testMixedUseStatements();
        testInvalidUseStatements();
        testUseStatementParsing();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}