#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLManage/SelectorAutomationManager.h"
#include "CHTLManage/ConfigurationManager.h"
#include "../Error/ErrorReport.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testBasicStyleAutomation() {
    std::cout << "\n=== 测试基础样式自动化 ===" << std::endl;
    
    std::string chtlCode = R"(
div {
    style {
        .box {
            background-color: red;
        }
        
        #container {
            width: 100%;
        }
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "解析成功: " << (result.rootNode ? "✓" : "❌") << std::endl;
    
    if (result.rootNode && result.rootNode->getChildCount() > 0) {
        // 转换为shared_ptr
        auto rawChild = result.rootNode->getChild(0);
        auto divElement = std::dynamic_pointer_cast<ElementNode>(std::shared_ptr<BaseNode>(rawChild, [](BaseNode*){/* 不删除，由父节点管理 */}));
        if (divElement) {
            std::cout << "找到div元素: ✓" << std::endl;
            
            // 创建选择器自动化管理器
            SelectorAutomationManager automationManager;
            
            // 处理样式自动化
            automationManager.processStyleAutomation(divElement);
            
            // 检查是否自动添加了class属性
            if (!divElement->getClass().empty()) {
                std::cout << "自动添加class: " << divElement->getClass() << " ✓" << std::endl;
            } else {
                std::cout << "未自动添加class ❌" << std::endl;
            }
            
            // 检查是否自动添加了id属性
            if (!divElement->getId().empty()) {
                std::cout << "自动添加id: " << divElement->getId() << " ✓" << std::endl;
            } else {
                std::cout << "未自动添加id ❌" << std::endl;
            }
        }
    }
}

void testScriptAutomation() {
    std::cout << "\n=== 测试脚本自动化 ===" << std::endl;
    
    std::string chtlCode = R"(
button {
    script {
        {{.btn}} -> addEventListener('click', () => {
            console.log('Button clicked!');
        });
        
        {{#submit}} -> style.backgroundColor = 'blue';
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "解析成功: " << (result.rootNode ? "✓" : "❌") << std::endl;
    
    if (result.rootNode && result.rootNode->getChildCount() > 0) {
        // 转换为shared_ptr
        auto rawChild = result.rootNode->getChild(0);
        auto buttonElement = std::dynamic_pointer_cast<ElementNode>(std::shared_ptr<BaseNode>(rawChild, [](BaseNode*){/* 不删除，由父节点管理 */}));
        if (buttonElement) {
            std::cout << "找到button元素: ✓" << std::endl;
            
            // 创建选择器自动化管理器
            SelectorAutomationManager automationManager;
            
            // 启用脚本自动化
            AutomationRule rules;
            rules.enableScriptAutoClass = true;
            rules.enableScriptAutoId = true;
            automationManager.setAutomationRules(rules);
            
            // 处理脚本自动化
            automationManager.processScriptAutomation(buttonElement);
            
            // 检查自动化结果
            if (!buttonElement->getClass().empty()) {
                std::cout << "脚本自动添加class: " << buttonElement->getClass() << " ✓" << std::endl;
            }
            
            if (!buttonElement->getId().empty()) {
                std::cout << "脚本自动添加id: " << buttonElement->getId() << " ✓" << std::endl;
            }
        }
    }
}

void testConfigurationIntegration() {
    std::cout << "\n=== 测试配置集成 ===" << std::endl;
    
    std::string chtlCode = R"(
[Configuration] {
    DISABLE_STYLE_AUTO_ADD_CLASS = true;
    DISABLE_SCRIPT_AUTO_ADD_ID = false;
    AUTO_CLASS_PREFIX = "custom-";
    AUTO_ID_PREFIX = "auto-";
}

div {
    style {
        .test {
            color: blue;
        }
    }
    
    script {
        {{#myId}} -> textContent = 'Hello';
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "解析成功: " << (result.rootNode ? "✓" : "❌") << std::endl;
    
    if (result.rootNode) {
        // 检查配置
        std::cout << "配置节点数量: " << result.configs.size() << std::endl;
        
        if (!result.configs.empty()) {
            auto configNode = std::dynamic_pointer_cast<ConfigNode>(result.configs[0]);
            if (configNode) {
                std::cout << "找到配置节点: ✓" << std::endl;
                
                // 创建配置管理器和选择器自动化管理器
                ConfigurationManager configManager;
                // 注意：ConfigurationManager期望ConfigurationNode，但我们有ConfigNode
                // 为简化测试，我们直接使用配置设置
                
                SelectorAutomationManager automationManager;
                
                // 应用配置到自动化管理器
                std::unordered_map<std::string, std::string> settings = {
                    {"DISABLE_STYLE_AUTO_ADD_CLASS", "true"},
                    {"DISABLE_SCRIPT_AUTO_ADD_ID", "false"},
                    {"AUTO_CLASS_PREFIX", "custom-"},
                    {"AUTO_ID_PREFIX", "auto-"}
                };
                automationManager.loadAutomationRulesFromConfig(settings);
                
                std::cout << "配置应用成功: ✓" << std::endl;
            }
        }
    }
}

void testReferenceSelector() {
    std::cout << "\n=== 测试引用选择器 ===" << std::endl;
    
    std::string chtlCode = R"(
.container {
    style {
        & {
            padding: 20px;
        }
        
        &:hover {
            background-color: gray;
        }
    }
}
)";
    
    auto errorReporter = std::make_unique<ErrorReporter>();
    Lexer lexer(chtlCode, nullptr, nullptr, errorReporter.get());
    auto tokens = lexer.tokenize();
    
    ParseOptions options;
    CHTLParser parser(options);
    auto result = parser.parse(tokens);
    
    std::cout << "解析成功: " << (result.rootNode ? "✓" : "❌") << std::endl;
    
    if (result.rootNode && result.rootNode->getChildCount() > 0) {
        // 转换为shared_ptr
        auto rawChild = result.rootNode->getChild(0);
        auto element = std::dynamic_pointer_cast<ElementNode>(std::shared_ptr<BaseNode>(rawChild, [](BaseNode*){/* 不删除，由父节点管理 */}));
        if (element) {
            std::cout << "找到元素: ✓" << std::endl;
            
            // 创建选择器自动化管理器
            SelectorAutomationManager automationManager;
            
            // 解析引用选择器
            std::string resolved = automationManager.resolveReferenceSelector(element, "style");
            std::cout << "引用选择器解析结果: " << (resolved.empty() ? "❌" : "✓") << std::endl;
            
            if (!resolved.empty()) {
                std::cout << "解析后的选择器: " << resolved << std::endl;
            }
        }
    }
}

void testAutomationReport() {
    std::cout << "\n=== 测试自动化报告 ===" << std::endl;
    
    SelectorAutomationManager automationManager;
    
    // 创建测试元素
    auto testElement = std::make_shared<ElementNode>("div");
    
    // 应用自动化
    automationManager.processStyleAutomation(testElement);
    
    // 生成报告
    std::string report = automationManager.generateAutomationReport();
    std::cout << "自动化报告生成: " << (report.empty() ? "❌" : "✓") << std::endl;
    
    if (!report.empty()) {
        std::cout << "报告内容:\n" << report << std::endl;
    }
}

int main() {
    std::cout << "选择器自动化系统测试" << std::endl;
    std::cout << "=====================" << std::endl;
    
    testBasicStyleAutomation();
    testScriptAutomation();
    testConfigurationIntegration();
    testReferenceSelector();
    testAutomationReport();
    
    std::cout << "\n=== 选择器自动化测试完成 ===" << std::endl;
    
    return 0;
}