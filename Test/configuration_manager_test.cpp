#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testBasicConfiguration() {
    std::cout << "=== 测试基本配置功能 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 测试默认配置
    auto defaultConfig = manager.getDefaultConfiguration();
    std::cout << "默认配置组存在: " << (defaultConfig != nullptr ? "是" : "否") << std::endl;
    
    // 测试无名配置组
    auto unnamedGroup = manager.getConfigurationGroup("");
    std::cout << "无名配置组存在: " << (unnamedGroup != nullptr ? "是" : "否") << std::endl;
    
    // 测试获取配置项
    auto debugMode = manager.getConfig("", "DEBUG_MODE");
    std::cout << "DEBUG_MODE: " << debugMode.toString() << std::endl;
    
    auto indexCount = manager.getConfig("", "INDEX_INITIAL_COUNT");
    std::cout << "INDEX_INITIAL_COUNT: " << indexCount.toString() << std::endl;
    
    auto customStyle = manager.getConfig("", "CUSTOM_STYLE");
    std::cout << "CUSTOM_STYLE 类型: " << static_cast<int>(customStyle.type) << std::endl;
    if (customStyle.type == ConfigItemType::STRING_ARRAY) {
        std::cout << "CUSTOM_STYLE 值: " << customStyle.toString() << std::endl;
    }
    
    std::cout << std::endl;
}

void testConfigurationGroups() {
    std::cout << "=== 测试配置组管理 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 创建命名配置组
    bool success = manager.createConfigurationGroup("TestGroup");
    std::cout << "创建配置组 TestGroup: " << (success ? "成功" : "失败") << std::endl;
    
    // 检查配置组是否存在
    bool exists = manager.hasConfigurationGroup("TestGroup");
    std::cout << "TestGroup 存在: " << (exists ? "是" : "否") << std::endl;
    
    // 获取配置组
    auto group = manager.getConfigurationGroup("TestGroup");
    std::cout << "获取配置组: " << (group != nullptr ? "成功" : "失败") << std::endl;
    
    // 设置自定义配置
    success = manager.setConfig("TestGroup", "CUSTOM_DEBUG", ConfigValue(ConfigItemType::BOOLEAN, true));
    std::cout << "设置 CUSTOM_DEBUG: " << (success ? "成功" : "失败") << std::endl;
    
    // 获取自定义配置
    auto customDebug = manager.getConfig("TestGroup", "CUSTOM_DEBUG");
    std::cout << "CUSTOM_DEBUG: " << customDebug.toString() << std::endl;
    
    // 获取所有配置组
    auto allGroups = manager.getConfigurationGroupInfo("");
    std::cout << "配置组信息长度: " << allGroups.length() << std::endl;
    
    std::cout << std::endl;
}

void testOriginTypes() {
    std::cout << "=== 测试原始嵌入类型 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 设置自定义原始嵌入类型
    bool success = manager.setOriginType("", "ORIGINTYPE_REACT", "@React");
    std::cout << "设置 ORIGINTYPE_REACT: " << (success ? "成功" : "失败") << std::endl;
    
    success = manager.setOriginType("", "ORIGINTYPE_ANGULAR", "@Angular");
    std::cout << "设置 ORIGINTYPE_ANGULAR: " << (success ? "成功" : "失败") << std::endl;
    
    // 获取原始嵌入类型
    auto reactTypes = manager.getOriginType("", "ORIGINTYPE_REACT");
    std::cout << "React 类型: ";
    for (const auto& type : reactTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;
    
    auto angularTypes = manager.getOriginType("", "ORIGINTYPE_ANGULAR");
    std::cout << "Angular 类型: ";
    for (const auto& type : angularTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;
    
    std::cout << std::endl;
}

void testConfigurationActivation() {
    std::cout << "=== 测试配置组激活 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 创建配置组
    manager.createConfigurationGroup("ActiveGroup");
    
    // 激活配置组
    bool success = manager.activateConfigurationGroup("ActiveGroup");
    std::cout << "激活 ActiveGroup: " << (success ? "成功" : "失败") << std::endl;
    
    // 检查当前激活的配置组
    auto activeGroup = manager.getActiveConfigurationGroup();
    std::cout << "当前激活的配置组: " << (activeGroup.empty() ? "(无名)" : activeGroup) << std::endl;
    
    // 重新激活无名配置组
    success = manager.activateConfigurationGroup("");
    std::cout << "重新激活无名配置组: " << (success ? "成功" : "失败") << std::endl;
    
    activeGroup = manager.getActiveConfigurationGroup();
    std::cout << "重新激活后的配置组: " << (activeGroup.empty() ? "(无名)" : activeGroup) << std::endl;
    
    std::cout << std::endl;
}

void testConfigurationValidation() {
    std::cout << "=== 测试配置验证 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 测试配置组验证
    bool valid = manager.validateConfigurationGroup("");
    std::cout << "无名配置组验证: " << (valid ? "通过" : "失败") << std::endl;
    
    // 测试配置项验证
    bool success = manager.setConfig("", "INDEX_INITIAL_COUNT", ConfigValue(ConfigItemType::STRING, "invalid"));
    std::cout << "设置无效 INDEX_INITIAL_COUNT: " << (success ? "成功" : "失败") << std::endl;
    
    success = manager.setConfig("", "INDEX_INITIAL_COUNT", ConfigValue(ConfigItemType::INTEGER, 100));
    std::cout << "设置有效 INDEX_INITIAL_COUNT: " << (success ? "成功" : "失败") << std::endl;
    
    // 测试配置值解析
    auto configValue = manager.getConfig("", "INDEX_INITIAL_COUNT");
    std::cout << "解析后的 INDEX_INITIAL_COUNT: " << configValue.toString() << std::endl;
    
    std::cout << std::endl;
}

void testConfigurationMerging() {
    std::cout << "=== 测试配置组合并 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 创建源配置组
    manager.createConfigurationGroup("SourceGroup");
    manager.setConfig("SourceGroup", "CUSTOM_VALUE", ConfigValue(ConfigItemType::STRING, "source_value"));
    manager.setOriginType("SourceGroup", "ORIGINTYPE_SOURCE", "@Source");
    
    // 创建目标配置组
    manager.createConfigurationGroup("TargetGroup");
    manager.setConfig("TargetGroup", "CUSTOM_VALUE", ConfigValue(ConfigItemType::STRING, "target_value"));
    manager.setOriginType("TargetGroup", "ORIGINTYPE_TARGET", "@Target");
    
    // 合并配置组
    bool success = manager.mergeConfigurationGroups("TargetGroup", "SourceGroup");
    std::cout << "合并配置组: " << (success ? "成功" : "失败") << std::endl;
    
    // 检查合并结果
    auto mergedValue = manager.getConfig("TargetGroup", "CUSTOM_VALUE");
    std::cout << "合并后的 CUSTOM_VALUE: " << mergedValue.toString() << std::endl;
    
    auto sourceTypes = manager.getOriginType("TargetGroup", "ORIGINTYPE_SOURCE");
    std::cout << "合并后的源类型: ";
    for (const auto& type : sourceTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;
    
    std::cout << std::endl;
}

void testConfigurationInfo() {
    std::cout << "=== 测试配置信息获取 ===" << std::endl;
    
    ConfigurationManager manager;
    
    // 获取配置组信息
    std::string info = manager.getConfigurationGroupInfo("");
    std::cout << "无名配置组信息:\n" << info << std::endl;
    
    // 创建命名配置组并获取信息
    manager.createConfigurationGroup("InfoGroup");
    info = manager.getConfigurationGroupInfo("InfoGroup");
    std::cout << "InfoGroup 信息:\n" << info << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "CHTL 配置管理器测试程序" << std::endl;
    std::cout << "=========================" << std::endl << std::endl;
    
    try {
        testBasicConfiguration();
        testConfigurationGroups();
        testOriginTypes();
        testConfigurationActivation();
        testConfigurationValidation();
        testConfigurationMerging();
        testConfigurationInfo();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}