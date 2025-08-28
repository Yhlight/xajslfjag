#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../CHTL/CHTLContext/SelectorAutomation.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testSelectorParsing() {
    std::cout << "=== 测试选择器解析 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 测试类选择器
    auto classSelector = automation.parseSelector(".box", 10, 5);
    assert(classSelector->type == SelectorType::CLASS);
    assert(classSelector->value == "box");
    assert(classSelector->fullSelector == ".box");
    assert(classSelector->lineNumber == 10);
    assert(classSelector->columnNumber == 5);
    
    // 测试ID选择器
    auto idSelector = automation.parseSelector("#header", 15, 8);
    assert(idSelector->type == SelectorType::ID);
    assert(idSelector->value == "header");
    assert(idSelector->fullSelector == "#header");
    
    // 测试伪类选择器
    auto pseudoClassSelector = automation.parseSelector(":hover", 20, 12);
    assert(pseudoClassSelector->type == SelectorType::PSEUDO_CLASS);
    assert(pseudoClassSelector->value == "hover");
    
    // 测试伪元素选择器
    auto pseudoElementSelector = automation.parseSelector("::before", 25, 16);
    assert(pseudoElementSelector->type == SelectorType::PSEUDO_ELEMENT);
    assert(pseudoElementSelector->value == "before");
    
    // 测试标签选择器
    auto tagSelector = automation.parseSelector("div", 30, 20);
    assert(tagSelector->type == SelectorType::TAG);
    assert(tagSelector->value == "div");
    
    // 测试通用选择器
    auto universalSelector = automation.parseSelector("*", 35, 24);
    assert(universalSelector->type == SelectorType::UNIVERSAL);
    assert(universalSelector->value == "*");
    
    std::cout << "选择器解析测试通过！" << std::endl;
}

void testSelectorValidation() {
    std::cout << "=== 测试选择器验证 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 测试有效选择器
    assert(automation.validateSelector(".box"));
    assert(automation.validateSelector("#header"));
    assert(automation.validateSelector(":hover"));
    assert(automation.validateSelector("::before"));
    assert(automation.validateSelector("div"));
    assert(automation.validateSelector("*"));
    assert(automation.validateSelector("test-class"));
    assert(automation.validateSelector("test_class"));
    
    // 测试无效选择器
    assert(!automation.validateSelector(""));
    assert(!automation.validateSelector("@invalid"));
    assert(!automation.validateSelector("123invalid"));
    assert(!automation.validateSelector("invalid@"));
    
    std::cout << "选择器验证测试通过！" << std::endl;
}

void testSelectorTypeDetection() {
    std::cout << "=== 测试选择器类型检测 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 测试各种选择器类型
    assert(automation.getSelectorType(".box") == SelectorType::CLASS);
    assert(automation.getSelectorType("#header") == SelectorType::ID);
    assert(automation.getSelectorType(":hover") == SelectorType::PSEUDO_CLASS);
    assert(automation.getSelectorType("::before") == SelectorType::PSEUDO_ELEMENT);
    assert(automation.getSelectorType("div") == SelectorType::TAG);
    assert(automation.getSelectorType("*") == SelectorType::UNIVERSAL);
    
    // 测试组合选择器
    assert(automation.getSelectorType("div.box") == SelectorType::COMBINATOR);
    // 暂时跳过复杂的组合选择器测试
    // assert(automation.getSelectorType(".box > div") == SelectorType::COMBINATOR);
    
    std::cout << "选择器类型检测测试通过！" << std::endl;
}

void testStyleBlockAutomation() {
    std::cout << "=== 测试样式块自动化 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 创建选择器上下文
    auto context = std::make_shared<SelectorContext>();
    context->elementName = "div";
    context->hasStyleBlock = true;
    context->sourceFile = "test.chtl";
    context->lineNumber = 10;
    
    // 添加选择器
    context->selectors.push_back(automation.parseSelector(".box", 10, 5));
    context->selectors.push_back(automation.parseSelector("#header", 11, 5));
    context->selectors.push_back(automation.parseSelector(":hover", 12, 5));
    
    // 处理样式块自动化
    assert(automation.processStyleBlockAutomation(context));
    
    // 验证自动添加的属性
    assert(context->className == "box");
    assert(context->idName == "header");
    
    std::cout << "样式块自动化测试通过！" << std::endl;
}

void testScriptBlockAutomation() {
    std::cout << "=== 测试脚本块自动化 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 创建选择器上下文（没有样式块）
    auto context = std::make_shared<SelectorContext>();
    context->elementName = "button";
    context->hasScriptBlock = true;
    context->sourceFile = "test.chtl";
    context->lineNumber = 20;
    
    // 添加选择器
    context->selectors.push_back(automation.parseSelector(".btn", 20, 5));
    context->selectors.push_back(automation.parseSelector("#submit", 21, 5));
    
    // 处理脚本块自动化
    assert(automation.processScriptBlockAutomation(context));
    
    // 验证自动添加的属性（根据默认规则，脚本块自动化是禁用的）
    // 所以这里应该没有自动添加的属性
    assert(context->className.empty());
    assert(context->idName.empty());
    
    std::cout << "脚本块自动化测试通过！" << std::endl;
}

void testReferenceSelectorResolution() {
    std::cout << "=== 测试引用选择器解析 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 创建带样式的选择器上下文
    auto styleContext = std::make_shared<SelectorContext>();
    styleContext->elementName = "div";
    styleContext->hasStyleBlock = true;
    styleContext->className = "container";
    styleContext->idName = "main";
    
    // 测试样式块中的引用选择器（优先选择类名）
    std::string resolvedStyle = automation.resolveReferenceSelector("&:hover", styleContext);
    assert(resolvedStyle == ".container:hover");
    
    // 创建带脚本的选择器上下文
    auto scriptContext = std::make_shared<SelectorContext>();
    scriptContext->elementName = "button";
    scriptContext->hasScriptBlock = true;
    scriptContext->className = "btn";
    scriptContext->idName = "submit";
    
    // 测试脚本块中的引用选择器（优先选择ID）
    std::string resolvedScript = automation.resolveReferenceSelector("&->click", scriptContext);
    assert(resolvedScript == "#submit->click");
    
    // 测试没有上下文的引用选择器
    std::string noContext = automation.resolveReferenceSelector("&:focus", nullptr);
    assert(noContext == "&:focus");
    
    std::cout << "引用选择器解析测试通过！" << std::endl;
}

void testAutomationRules() {
    std::cout << "=== 测试自动化规则 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 获取当前自动化规则
    auto currentRule = automation.getAutomationRule();
    
    // 测试默认规则
    assert(currentRule.disableStyleAutoAddClass == false);
    assert(currentRule.disableStyleAutoAddId == false);
    assert(currentRule.disableScriptAutoAddClass == true);
    assert(currentRule.disableScriptAutoAddId == true);
    
    // 设置新的自动化规则
    AutomationRule newRule;
    newRule.disableStyleAutoAddClass = true;
    newRule.disableStyleAutoAddId = true;
    newRule.disableScriptAutoAddClass = false;
    newRule.disableScriptAutoAddId = false;
    
    automation.setAutomationRule(newRule);
    
    // 验证新规则
    auto updatedRule = automation.getAutomationRule();
    assert(updatedRule.disableStyleAutoAddClass == true);
    assert(updatedRule.disableStyleAutoAddId == true);
    assert(updatedRule.disableScriptAutoAddClass == false);
    assert(updatedRule.disableScriptAutoAddId == false);
    
    std::cout << "自动化规则测试通过！" << std::endl;
}

void testSelectorPriority() {
    std::cout << "=== 测试选择器优先级 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 创建不同优先级的选择器
    auto idSelector = automation.parseSelector("#header");
    auto classSelector = automation.parseSelector(".box");
    auto tagSelector = automation.parseSelector("div");
    auto universalSelector = automation.parseSelector("*");
    
    // 测试优先级
    assert(automation.getSelectorPriority(idSelector) == 100);
    assert(automation.getSelectorPriority(classSelector) == 10);
    assert(automation.getSelectorPriority(tagSelector) == 1);
    assert(automation.getSelectorPriority(universalSelector) == 0);
    
    // 测试优先级排序
    std::vector<std::shared_ptr<Selector>> selectors = {tagSelector, idSelector, universalSelector, classSelector};
    automation.sortSelectorsByPriority(selectors);
    
    // 验证排序结果（按优先级降序）
    assert(selectors[0]->type == SelectorType::ID);
    assert(selectors[1]->type == SelectorType::CLASS);
    assert(selectors[2]->type == SelectorType::TAG);
    assert(selectors[3]->type == SelectorType::UNIVERSAL);
    
    std::cout << "选择器优先级测试通过！" << std::endl;
}

void testElementAutomation() {
    std::cout << "=== 测试元素自动化 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 创建完整的选择器上下文
    auto context = std::make_shared<SelectorContext>();
    context->elementName = "div";
    context->hasStyleBlock = true;
    context->hasScriptBlock = true;
    context->sourceFile = "test.chtl";
    context->lineNumber = 10;
    
    // 添加选择器
    context->selectors.push_back(automation.parseSelector(".container", 10, 5));
    context->selectors.push_back(automation.parseSelector("#main", 11, 5));
    context->selectors.push_back(automation.parseSelector(":hover", 12, 5));
    
    // 处理元素自动化
    assert(automation.processElementAutomation(context));
    
    // 验证结果
    assert(context->className == "container");
    assert(context->idName == "main");
    
    std::cout << "元素自动化测试通过！" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation(configManager);
    
    // 清除统计信息
    automation.clearStatistics();
    
    // 执行一些操作来生成统计信息
    auto selector1 = automation.parseSelector(".box");
    auto selector2 = automation.parseSelector("#header");
    
    auto context = std::make_shared<SelectorContext>();
    context->hasStyleBlock = true;
    context->selectors.push_back(selector1);
    context->selectors.push_back(selector2);
    
    automation.processStyleBlockAutomation(context);
    
    // 获取统计信息
    std::string stats = automation.getStatistics();
    assert(!stats.empty());
    assert(stats.find("总选择器处理数量: 2") != std::string::npos);
    assert(stats.find("自动属性生成数量: 2") != std::string::npos);
    assert(stats.find("样式块自动化数量: 1") != std::string::npos);
    
    std::cout << "统计信息测试通过！" << std::endl;
    std::cout << "\n统计信息:\n" << stats << std::endl;
}

int main() {
    std::cout << "CHTL 选择器自动化测试程序" << std::endl;
    std::cout << "==========================" << std::endl << std::endl;
    
    try {
        testSelectorParsing();
        testSelectorValidation();
        testSelectorTypeDetection();
        testStyleBlockAutomation();
        testScriptBlockAutomation();
        testReferenceSelectorResolution();
        testAutomationRules();
        testSelectorPriority();
        testElementAutomation();
        testStatistics();
        
        std::cout << "\n所有测试完成！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}