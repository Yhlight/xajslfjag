#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../CHTL/CHTLParser/UseStatementParser.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testHTML5Type() {
    std::cout << "=== 测试HTML5类型use语句 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    UseStatementParser parser(configManager);
    
    // 测试有效的HTML5 use语句
    std::string source1 = "use html5;\n[Custom] @Element Box { div { } }";
    auto statement1 = parser.parse(source1);
    
    if (statement1) {
        std::cout << "HTML5 use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1->type) << std::endl;
        std::cout << "HTML5类型: " << statement1->html5Type << std::endl;
        std::cout << "行号: " << statement1->lineNumber << std::endl;
        std::cout << "列号: " << statement1->columnNumber << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        // 应用语句
        bool applied = parser.applyUseStatement(statement1);
        std::cout << "应用结果: " << (applied ? "成功" : "失败") << std::endl;
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
    
    UseStatementParser parser(configManager);
    
    // 测试有效的配置组use语句
    std::string source1 = "use @Config Basic;\n[Custom] @Element Box { div { } }";
    auto statement1 = parser.parse(source1);
    
    if (statement1) {
        std::cout << "配置组use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1->type) << std::endl;
        std::cout << "配置组名称: " << statement1->configGroupName << std::endl;
        std::cout << "行号: " << statement1->lineNumber << std::endl;
        std::cout << "列号: " << statement1->columnNumber << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        // 应用语句
        bool applied = parser.applyUseStatement(statement1);
        std::cout << "应用结果: " << (applied ? "成功" : "失败") << std::endl;
        
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
    
    UseStatementParser parser(configManager);
    
    // 测试有效的全缀名配置组use语句
    std::string source1 = "use [Configuration] @Config FullConfig;\n[Custom] @Element Box { div { } }";
    auto statement1 = parser.parse(source1);
    
    if (statement1) {
        std::cout << "全缀名配置组use语句解析成功" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement1->type) << std::endl;
        std::cout << "配置组名称: " << statement1->configGroupName << std::endl;
        std::cout << "行号: " << statement1->lineNumber << std::endl;
        std::cout << "列号: " << statement1->columnNumber << std::endl;
        
        // 验证语句
        bool valid = parser.validateUseStatement(statement1);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        // 应用语句
        bool applied = parser.applyUseStatement(statement1);
        std::cout << "应用结果: " << (applied ? "成功" : "失败") << std::endl;
        
        // 检查当前激活的配置组
        std::string activeGroup = configManager->getActiveConfigurationGroup();
        std::cout << "当前激活的配置组: " << (activeGroup.empty() ? "(无名)" : activeGroup) << std::endl;
    } else {
        std::cout << "全缀名配置组use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testInvalidUseStatements() {
    std::cout << "=== 测试无效use语句 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    UseStatementParser parser(configManager);
    
    // 测试1: use语句不在文件开头
    std::string source1 = "[Custom] @Element Box { div { } }\nuse html5;";
    auto statement1 = parser.parse(source1);
    
    if (statement1) {
        std::cout << "意外的解析成功: use语句不在文件开头" << std::endl;
    } else {
        std::cout << "正确拒绝: use语句不在文件开头" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    // 测试2: 不存在的配置组
    std::string source2 = "use @Config NonExistent;\n[Custom] @Element Box { div { } }";
    auto statement2 = parser.parse(source2);
    
    if (statement2) {
        std::cout << "解析成功，但配置组不存在" << std::endl;
        bool valid = parser.validateUseStatement(statement2);
        std::cout << "验证结果: " << (valid ? "通过" : "失败") << std::endl;
        
        if (!valid) {
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "验证错误: " << error << std::endl;
            }
        }
    } else {
        std::cout << "解析失败: 不存在的配置组" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    // 测试3: 语法错误
    std::string source3 = "use html5\n[Custom] @Element Box { div { } }";
    auto statement3 = parser.parse(source3);
    
    if (statement3) {
        std::cout << "意外的解析成功: 缺少分号" << std::endl;
    } else {
        std::cout << "正确拒绝: 缺少分号" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testMultipleUseStatements() {
    std::cout << "=== 测试多个use语句 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    UseStatementParser parser(configManager);
    
    // 测试多个use语句（应该只解析第一个）
    std::string source = "use html5;\nuse @Config Basic;\n[Custom] @Element Box { div { } }";
    auto statement = parser.parse(source);
    
    if (statement) {
        std::cout << "多个use语句解析结果:" << std::endl;
        std::cout << "类型: " << static_cast<int>(statement->type) << std::endl;
        if (statement->type == UseStatementType::HTML5_TYPE) {
            std::cout << "HTML5类型: " << statement->html5Type << std::endl;
        } else {
            std::cout << "配置组名称: " << statement->configGroupName << std::endl;
        }
    } else {
        std::cout << "多个use语句解析失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "错误: " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "CHTL Use语句解析器测试程序" << std::endl;
    std::cout << "============================" << std::endl << std::endl;
    
    try {
        testHTML5Type();
        testConfigGroup();
        testFullConfigGroup();
        testInvalidUseStatements();
        testMultipleUseStatements();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}