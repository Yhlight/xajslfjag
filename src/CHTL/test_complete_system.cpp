#include <iostream>
#include <string>
#include <sstream>

// 包含所有必要的头文件
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/TemplateManager.h"
#include "CHTLManage/LocalStyleManager.h"
#include "CHTLManage/SelectorAutomationManager.h"
#include "CHTLNode/ConfigNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/ConstraintNode.h"
#include "CHTLNode/ImportNode.h"
#include "Error/ErrorReport.h"

using namespace CHTL;

void testConfigurationSystem() {
    std::cout << "\n=== 测试配置系统 ===" << std::endl;
    
    std::string configCode = R"(
[Configuration] @Config Basic
{
    INDEX_INITIAL_COUNT = 0;
    DISABLE_NAME_GROUP = false;
    DEBUG_MODE = true;
    
    [Name]
    {
        CUSTOM_STYLE = [@Style, @style, @CSS];
        TEMPLATE_ELEMENT = @Element;
        KEYWORD_DELETE = delete;
        KEYWORD_INSERT = insert;
    }
    
    [OriginType]
    {
        ORIGINTYPE_VUE = @Vue;
        ORIGINTYPE_REACT = @React;
    }
}

[Configuration]
{
    DISABLE_DEFAULT_NAMESPACE = false;
    DISABLE_STYLE_AUTO_ADD_CLASS = false;
}
)";

    try {
        ErrorReporter errorReporter;
        CHTLLexer lexer(configCode);
        lexer.setErrorReporter(&errorReporter);
        lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 配置系统词法分析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        CHTLParser parser(lexer.getTokens());
        parser.setErrorReporter(&errorReporter);
        
        ParseOptions options;
        options.enableConfig = true;
        parser.setParseOptions(options);
        
        auto ast = parser.parse();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 配置系统解析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        if (ast) {
            std::cout << "✓ 配置系统解析成功" << std::endl;
            std::cout << "AST 节点数: " << ast->getChildren().size() << std::endl;
            
            // 验证配置节点
            for (auto child : ast->getChildren()) {
                if (auto configNode = std::dynamic_pointer_cast<ConfigurationNode>(child)) {
                    std::cout << "✓ 找到配置节点: " << configNode->getConfigName() << std::endl;
                    std::cout << "  - 索引起始计数: " << configNode->getIndexInitialCount() << std::endl;
                    std::cout << "  - 调试模式: " << (configNode->isDebugMode() ? "启用" : "禁用") << std::endl;
                }
            }
        } else {
            std::cout << "❌ 未生成AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 配置系统测试异常: " << e.what() << std::endl;
    }
}

void testNamespaceSystem() {
    std::cout << "\n=== 测试命名空间系统 ===" << std::endl;
    
    std::string namespaceCode = R"(
[Namespace] space
{
    [Namespace] room
    {
        [Template] @Element Box
        {
            div { style { width: 100px; } }
        }
    }
    
    [Namespace] room2
    {
        [Custom] @Style TextStyle
        {
            color: red;
            font-size: 16px;
        }
    }
}

body
{
    @Element Box from space.room;
    
    style
    {
        @Style TextStyle from space.room2;
    }
}
)";

    try {
        ErrorReporter errorReporter;
        CHTLLexer lexer(namespaceCode);
        lexer.setErrorReporter(&errorReporter);
        lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 命名空间系统词法分析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        CHTLParser parser(lexer.getTokens());
        parser.setErrorReporter(&errorReporter);
        
        ParseOptions options;
        options.enableNamespaces = true;
        parser.setParseOptions(options);
        
        auto ast = parser.parse();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 命名空间系统解析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        if (ast) {
            std::cout << "✓ 命名空间系统解析成功" << std::endl;
            std::cout << "AST 节点数: " << ast->getChildren().size() << std::endl;
            
            // 测试命名空间管理器
            NamespaceManager namespaceManager;
            auto spaceNamespace = namespaceManager.createOrGetNamespace("space");
            auto roomNamespace = namespaceManager.createOrGetNamespace("space.room");
            auto room2Namespace = namespaceManager.createOrGetNamespace("space.room2");
            
            std::cout << "✓ 创建命名空间成功" << std::endl;
            std::cout << "  - space 路径: " << spaceNamespace->getFullNamespacePath() << std::endl;
            std::cout << "  - space.room 路径: " << roomNamespace->getFullNamespacePath() << std::endl;
            std::cout << "  - space.room2 路径: " << room2Namespace->getFullNamespacePath() << std::endl;
        } else {
            std::cout << "❌ 未生成AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 命名空间系统测试异常: " << e.what() << std::endl;
    }
}

void testScriptSystem() {
    std::cout << "\n=== 测试脚本系统 ===" << std::endl;
    
    std::string scriptCode = R"(
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
            background-color: red;
        }
    }
    
    script
    {
        {{.box}}->addEventListener('click', function() {
            console.log('Box clicked!');
        });
        
        {{.box}} &-> click {
            console.log('Using CHTL JS syntax!');
        }
        
        {{.box}}->listen {
            mouseenter: function() {
                console.log('Mouse entered!');
            },
            mouseleave: function() {
                console.log('Mouse left!');
            }
        };
    }
}
)";

    try {
        ErrorReporter errorReporter;
        CHTLLexer lexer(scriptCode);
        lexer.setErrorReporter(&errorReporter);
        lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 脚本系统词法分析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        CHTLParser parser(lexer.getTokens());
        parser.setErrorReporter(&errorReporter);
        
        ParseOptions options;
        options.enableScripts = true;
        parser.setParseOptions(options);
        
        auto ast = parser.parse();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 脚本系统解析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        if (ast) {
            std::cout << "✓ 脚本系统解析成功" << std::endl;
            
            // 测试CHTL JS转换
            ScriptNode scriptNode;
            scriptNode.setContent("{{.box}}->textContent = 'Hello World';");
            
            std::string convertedJS = scriptNode.convertToStandardJS();
            std::cout << "✓ CHTL JS转换测试:" << std::endl;
            std::cout << "  原始: {{.box}}->textContent = 'Hello World';" << std::endl;
            std::cout << "  转换: " << convertedJS << std::endl;
        } else {
            std::cout << "❌ 未生成AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 脚本系统测试异常: " << e.what() << std::endl;
    }
}

void testConstraintSystem() {
    std::cout << "\n=== 测试约束系统 ===" << std::endl;
    
    std::string constraintCode = R"(
[Namespace] ui
{
    except span, [Custom] @Element Box;  // 精确约束
    except @Html;                        // 类型约束
    
    div
    {
        // span 被约束，应该报错
        // span { }
        
        p
        {
            text { "允许的元素" }
        }
    }
}

[Namespace] restricted
{
    except [Template];  // 全局约束：禁止模板
    except [Custom] @Var;  // 禁止自定义变量组
    
    div
    {
        style
        {
            color: red;
        }
    }
    
    // [Template] @Style TestStyle  // 被约束，应该报错
    // {
    //     color: blue;
    // }
}
)";

    try {
        // 测试约束管理器
        ConstraintManager constraintManager;
        
        // 创建约束节点
        auto exactConstraint = std::make_shared<ConstraintNode>(ConstraintType::EXACT_CONSTRAINT);
        exactConstraint->addConstraintItem(ConstraintItem(ConstraintTargetType::HTML_ELEMENT, "span"));
        exactConstraint->addConstraintItem(ConstraintItem(ConstraintTargetType::CUSTOM_OBJECT, "Box", "@Element"));
        
        auto typeConstraint = std::make_shared<ConstraintNode>(ConstraintType::TYPE_CONSTRAINT);
        typeConstraint->addConstraintItem(ConstraintItem(ConstraintTargetType::HTML_TYPE));
        
        auto globalConstraint = std::make_shared<ConstraintNode>(ConstraintType::GLOBAL_CONSTRAINT);
        globalConstraint->addConstraintItem(ConstraintItem(ConstraintTargetType::TEMPLATE_TYPE));
        globalConstraint->setAsGlobalConstraint(true);
        
        // 添加约束
        constraintManager.addConstraint(exactConstraint, "ui");
        constraintManager.addConstraint(typeConstraint, "ui");
        constraintManager.addConstraint(globalConstraint);
        
        // 测试约束验证
        std::cout << "✓ 约束管理器创建成功" << std::endl;
        
        // 测试HTML元素约束
        bool spanAllowed = !constraintManager.isConstrained(ConstraintTargetType::HTML_ELEMENT, "span", "", "ui");
        bool pAllowed = !constraintManager.isConstrained(ConstraintTargetType::HTML_ELEMENT, "p", "", "ui");
        
        std::cout << "✓ HTML元素约束测试: span=" << (spanAllowed ? "允许" : "禁止") 
                  << ", p=" << (pAllowed ? "允许" : "禁止") << std::endl;
        
        // 测试类型约束
        bool htmlTypeAllowed = !constraintManager.isConstrained(ConstraintTargetType::HTML_TYPE, "", "", "ui");
        bool templateTypeAllowed = !constraintManager.isConstrained(ConstraintTargetType::TEMPLATE_TYPE, "", "", "");
        
        std::cout << "✓ 类型约束测试: @Html=" << (htmlTypeAllowed ? "允许" : "禁止")
                  << ", [Template]=" << (templateTypeAllowed ? "允许" : "禁止") << std::endl;
        
        // 生成约束报告
        std::string report = constraintManager.generateConstraintReport();
        std::cout << "✓ 约束报告生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 约束系统测试异常: " << e.what() << std::endl;
    }
}

void testSelectorAutomation() {
    std::cout << "\n=== 测试选择器自动化 ===" << std::endl;
    
    try {
        // 创建选择器自动化管理器
        SelectorAutomationManager automationManager;
        
        // 设置自动化规则
        AutomationRule rules;
        rules.enableStyleAutoClass = true;
        rules.enableStyleAutoId = true;
        rules.enableScriptAutoClass = false;
        rules.enableScriptAutoId = false;
        automationManager.setAutomationRules(rules);
        
        std::cout << "✓ 选择器自动化管理器创建成功" << std::endl;
        
        // 创建测试元素
        auto element = std::make_shared<ElementNode>("div");
        
        // 创建样式节点（模拟包含.box选择器的样式）
        auto styleNode = std::make_shared<StyleNode>();
        element->addChild(styleNode);
        
        // 处理样式自动化
        automationManager.processStyleAutomation(element);
        
        std::cout << "✓ 样式自动化处理完成" << std::endl;
        
        // 测试引用选择器解析
        ReferenceRuleProcessor refProcessor(&automationManager);
        
        std::string styleContent = "& { background-color: red; } &:hover { background-color: blue; }";
        std::string resolvedStyle = refProcessor.processStyleReferences(element, styleContent);
        
        std::cout << "✓ 引用选择器解析测试:" << std::endl;
        std::cout << "  原始: " << styleContent << std::endl;
        std::cout << "  解析: " << resolvedStyle << std::endl;
        
        // 生成自动化报告
        std::string report = automationManager.generateAutomationReport();
        std::cout << "✓ 自动化报告生成成功" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 选择器自动化测试异常: " << e.what() << std::endl;
    }
}

void testImportSystem() {
    std::cout << "\n=== 测试导入系统 ===" << std::endl;
    
    std::string importCode = R"(
[Import] @Html from "common.html" as CommonHtml;
[Import] @Style from "theme.css" as ThemeStyle;
[Import] @Chtl from "components.chtl";

[Import] [Template] @Element Button from "ui.chtl" as UIButton;
[Import] [Custom] @Style TextStyles from "styles.chtl";
[Import] [Origin] @Vue ComponentA from "components.chtl" as VueComponent;

[Import] [Template] from "all-templates.chtl";
[Import] [Custom] @Element from "all-elements.chtl";
)";

    try {
        ErrorReporter errorReporter;
        CHTLLexer lexer(importCode);
        lexer.setErrorReporter(&errorReporter);
        lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 导入系统词法分析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        CHTLParser parser(lexer.getTokens());
        parser.setErrorReporter(&errorReporter);
        
        ParseOptions options;
        options.enableImports = true;
        parser.setParseOptions(options);
        
        auto ast = parser.parse();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 导入系统解析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        if (ast) {
            std::cout << "✓ 导入系统解析成功" << std::endl;
            
            int importNodes = 0;
            for (auto child : ast->getChildren()) {
                if (child->getNodeType() >= CHTLNodeType::IMPORT_HTML_NODE && 
                    child->getNodeType() <= CHTLNodeType::IMPORT_ORIGIN_NODE) {
                    importNodes++;
                    if (auto importNode = std::dynamic_pointer_cast<ImportNode>(child)) {
                        std::cout << "  - " << importNode->toString() << std::endl;
                    }
                }
            }
            
            std::cout << "✓ 导入节点数量: " << importNodes << std::endl;
        } else {
            std::cout << "❌ 未生成AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 导入系统测试异常: " << e.what() << std::endl;
    }
}

void testIntegratedSystem() {
    std::cout << "\n=== 测试集成系统 ===" << std::endl;
    
    std::string integratedCode = R"(
use html5;

[Configuration] @Config Main
{
    DEBUG_MODE = true;
    INDEX_INITIAL_COUNT = 0;
    
    [Name]
    {
        CUSTOM_STYLE = [@Style, @style];
    }
    
    [OriginType]
    {
        ORIGINTYPE_VUE = @Vue;
    }
}

[Namespace] ui
{
    [Template] @Style ButtonTheme
    {
        background-color: #007bff;
        color: white;
        border: none;
        padding: 8px 16px;
        border-radius: 4px;
    }
    
    [Custom] @Element Button
    {
        button
        {
            style
            {
                @Style ButtonTheme;
                
                &:hover
                {
                    background-color: #0056b3;
                }
            }
            
            script
            {
                {{&}}->addEventListener('click', function() {
                    console.log('Button clicked!');
                });
            }
        }
    }
}

[Origin] @Vue ButtonComponent
{
<template>
  <button @click="handleClick">
    <slot></slot>
  </button>
</template>

<script>
export default {
  methods: {
    handleClick() {
      this.$emit('click');
    }
  }
}
</script>
}

html
{
    head
    {
        text { "CHTL Complete System Test" }
    }
    
    body
    {
        @Element Button from ui
        {
            text { "Click Me" }
        }
        
        [Origin] @Vue ButtonComponent;
    }
}
)";

    try {
        ErrorReporter errorReporter;
        CHTLLexer lexer(integratedCode);
        lexer.setErrorReporter(&errorReporter);
        lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 集成系统词法分析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        CHTLParser parser(lexer.getTokens());
        parser.setErrorReporter(&errorReporter);
        
        ParseOptions options;
        options.enableTemplates = true;
        options.enableOrigins = true;
        options.enableConfig = true;
        options.enableNamespaces = true;
        options.enableScripts = true;
        parser.setParseOptions(options);
        
        auto ast = parser.parse();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 集成系统解析失败" << std::endl;
            errorReporter.printAllErrors();
            return;
        }
        
        if (ast) {
            std::cout << "✓ 集成系统解析成功" << std::endl;
            std::cout << "AST 节点数: " << ast->getChildren().size() << std::endl;
            
            int configNodes = 0, namespaceNodes = 0, originNodes = 0, templateNodes = 0;
            
            for (auto child : ast->getChildren()) {
                switch (child->getNodeType()) {
                    case CHTLNodeType::CONFIG_NODE:
                        configNodes++;
                        break;
                    case CHTLNodeType::NAMESPACE_NODE:
                        namespaceNodes++;
                        break;
                    case CHTLNodeType::ORIGIN_HTML_NODE:
                    case CHTLNodeType::ORIGIN_STYLE_NODE:
                    case CHTLNodeType::ORIGIN_JAVASCRIPT_NODE:
                    case CHTLNodeType::ORIGIN_CUSTOM_NODE:
                        originNodes++;
                        break;
                    case CHTLNodeType::TEMPLATE_STYLE_NODE:
                    case CHTLNodeType::TEMPLATE_ELEMENT_NODE:
                    case CHTLNodeType::TEMPLATE_VAR_NODE:
                        templateNodes++;
                        break;
                    default:
                        break;
                }
            }
            
            std::cout << "✓ 节点类型统计:" << std::endl;
            std::cout << "  - 配置节点: " << configNodes << std::endl;
            std::cout << "  - 命名空间节点: " << namespaceNodes << std::endl;
            std::cout << "  - 原始嵌入节点: " << originNodes << std::endl;
            std::cout << "  - 模板节点: " << templateNodes << std::endl;
        } else {
            std::cout << "❌ 未生成AST" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 集成系统测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 完整系统测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    testConfigurationSystem();
    testNamespaceSystem();
    testScriptSystem();
    testConstraintSystem();
    testSelectorAutomation();
    testImportSystem();
    testIntegratedSystem();
    
    std::cout << "\n测试完成！" << std::endl;
    return 0;
}