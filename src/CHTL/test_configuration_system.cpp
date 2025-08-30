#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/ConfigurationManager.h"
#include "CHTLNode/ConfigNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void testBasicConfiguration() {
    std::cout << "\n=== 测试基本配置系统 ===" << std::endl;
    
    std::string configCode = R"(
[Configuration]
{
    INDEX_INITIAL_COUNT = 1;
    DISABLE_NAME_GROUP = false;
    DISABLE_CUSTOM_ORIGIN_TYPE = true;
    DEBUG_MODE = true;
    DISABLE_DEFAULT_NAMESPACE = false;
    DISABLE_STYLE_AUTO_ADD_CLASS = false;
    DISABLE_STYLE_AUTO_ADD_ID = true;
    DISABLE_SCRIPT_AUTO_ADD_CLASS = true;
    DISABLE_SCRIPT_AUTO_ADD_ID = false;
}
)";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success && ast.rootNode) {
            std::cout << "✓ 配置解析成功" << std::endl;
            
            // 查找配置节点
            for (auto configPtr : ast.configs) {
                if (auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(configPtr)) {
                    ConfigurationManager configManager;
                    configManager.registerConfiguration(configNode);
                    
                    std::cout << "✓ 配置测试结果:" << std::endl;
                    std::cout << "  - 索引起始计数: " << configManager.getIndexInitialCount() << std::endl;
                    std::cout << "  - 禁用Name组: " << (configManager.isNameGroupDisabled() ? "是" : "否") << std::endl;
                    std::cout << "  - 禁用自定义原始类型: " << (configManager.isCustomOriginTypeDisabled() ? "是" : "否") << std::endl;
                    std::cout << "  - 调试模式: " << (configManager.isDebugMode() ? "是" : "否") << std::endl;
                    std::cout << "  - 样式自动添加类: " << (configManager.isStyleAutoAddClassEnabled() ? "是" : "否") << std::endl;
                    std::cout << "  - 脚本自动添加ID: " << (configManager.isScriptAutoAddIdEnabled() ? "是" : "否") << std::endl;
                    break;
                }
            }
        } else {
            std::cout << "❌ 配置解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 配置测试异常: " << e.what() << std::endl;
    }
}

void testNamedConfiguration() {
    std::cout << "\n=== 测试命名配置系统 ===" << std::endl;
    
    std::string configCode = R"(
[Configuration] @Config Advanced
{
    INDEX_INITIAL_COUNT = 0;
    DEBUG_MODE = true;
    
    [Name]
    {
        CUSTOM_STYLE = [@Style, @style, @CSS];
        CUSTOM_ELEMENT = @Element;
        TEMPLATE_STYLE = @Style;
        ORIGIN_HTML = @Html;
        KEYWORD_INHERIT = inherit;
        KEYWORD_DELETE = delete;
        OPTION_COUNT = 5;
    }
    
    [OriginType]
    {
        ORIGINTYPE_VUE = @Vue;
        ORIGINTYPE_REACT = @React;
        ORIGINTYPE_ANGULAR = @Angular;
    }
}
)";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success && ast.rootNode) {
            std::cout << "✓ 命名配置解析成功" << std::endl;
            
            // 查找配置节点
            for (auto configPtr : ast.configs) {
                if (auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(configPtr)) {
                    ConfigurationManager configManager;
                    configManager.registerConfiguration(configNode);
                    
                    std::cout << "✓ 命名配置测试结果:" << std::endl;
                    std::cout << "  - 配置名称: " << configNode->getConfigName() << std::endl;
                    std::cout << "  - 是否命名配置: " << (configNode->isNamed() ? "是" : "否") << std::endl;
                    std::cout << "  - 调试模式: " << (configManager.isDebugMode() ? "是" : "否") << std::endl;
                    
                    // 测试关键字别名
                    auto aliases = configManager.getKeywordAliases("CUSTOM_STYLE");
                    if (!aliases.empty()) {
                        std::cout << "  - CUSTOM_STYLE别名: ";
                        for (size_t i = 0; i < aliases.size(); ++i) {
                            std::cout << aliases[i];
                            if (i < aliases.size() - 1) std::cout << ", ";
                        }
                        std::cout << std::endl;
                    }
                    
                    // 测试自定义原始类型
                    auto customTypes = configManager.getCustomOriginTypes();
                    if (!customTypes.empty()) {
                        std::cout << "  - 自定义原始类型: ";
                        for (auto& pair : customTypes) {
                            std::cout << pair.first << "=" << pair.second << " ";
                        }
                        std::cout << std::endl;
                    }
                    
                    break;
                }
            }
        } else {
            std::cout << "❌ 命名配置解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 命名配置测试异常: " << e.what() << std::endl;
    }
}

void testMultipleConfigurations() {
    std::cout << "\n=== 测试多配置管理 ===" << std::endl;
    
    std::string configCode = R"(
[Configuration] @Config Basic
{
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 0;
}

[Configuration] @Config Advanced  
{
    DEBUG_MODE = true;
    INDEX_INITIAL_COUNT = 1;
    DISABLE_NAME_GROUP = true;
}

[Configuration]
{
    DEBUG_MODE = false;
    INDEX_INITIAL_COUNT = 2;
}
)";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success && ast.rootNode) {
            std::cout << "✓ 多配置解析成功" << std::endl;
            
            ConfigurationManager configManager;
            
            // 注册所有配置
            for (auto configPtr : ast.configs) {
                if (auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(configPtr)) {
                    configManager.registerConfiguration(configNode);
                    std::cout << "✓ 注册配置: " << (configNode->isNamed() ? configNode->getConfigName() : "无名配置") << std::endl;
                }
            }
            
            // 测试默认配置
            auto defaultConfig = configManager.getDefaultConfiguration();
            if (defaultConfig) {
                std::cout << "✓ 默认配置: " << (defaultConfig->isNamed() ? defaultConfig->getConfigName() : "无名配置") << std::endl;
                std::cout << "  - 索引起始计数: " << configManager.getIndexInitialCount() << std::endl;
            }
            
            // 测试配置切换
            configManager.setActiveConfiguration("Advanced");
            std::cout << "✓ 切换到Advanced配置" << std::endl;
            std::cout << "  - 调试模式: " << (configManager.isDebugMode() ? "是" : "否") << std::endl;
            std::cout << "  - 索引起始计数: " << configManager.getIndexInitialCount() << std::endl;
            
        } else {
            std::cout << "❌ 多配置解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 多配置测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 配置系统测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    testBasicConfiguration();
    testNamedConfiguration();
    testMultipleConfigurations();
    
    std::cout << "\n配置系统测试完成！" << std::endl;
    return 0;
}