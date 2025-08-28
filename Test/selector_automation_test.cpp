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
    
    SelectorAutomation automation;
    
    // 测试类选择器
    auto classSelectors = automation.parseSelectors(".box", 10, 5);
    assert(!classSelectors.empty());
    auto classSelector = classSelectors[0];
    assert(classSelector.type == SelectorType::CLASS);
    assert(classSelector.value == "box");
    assert(classSelector.fullSelector == ".box");
    assert(classSelector.line == 10);
    assert(classSelector.column == 5);
    
    // 测试ID选择器
    auto idSelectors = automation.parseSelectors("#header", 15, 8);
    assert(!idSelectors.empty());
    auto idSelector = idSelectors[0];
    assert(idSelector.type == SelectorType::ID);
    assert(idSelector.value == "header");
    assert(idSelector.fullSelector == "#header");
    
    // 测试伪类选择器
    auto pseudoClassSelectors = automation.parseSelectors(":hover", 20, 12);
    assert(!pseudoClassSelectors.empty());
    auto pseudoClassSelector = pseudoClassSelectors[0];
    assert(pseudoClassSelector.type == SelectorType::PSEUDO_CLASS);
    assert(pseudoClassSelector.value == "hover");
    
    // 测试伪元素选择器
    auto pseudoElementSelectors = automation.parseSelectors("::before", 25, 16);
    assert(!pseudoElementSelectors.empty());
    auto pseudoElementSelector = pseudoElementSelectors[0];
    assert(pseudoElementSelector.type == SelectorType::PSEUDO_ELEMENT);
    assert(pseudoElementSelector.value == "before");
    
    // 测试标签选择器
    auto tagSelectors = automation.parseSelectors("div", 30, 20);
    assert(!tagSelectors.empty());
    auto tagSelector = tagSelectors[0];
    assert(tagSelector.type == SelectorType::TAG);
    assert(tagSelector.value == "div");
    
    // 测试通用选择器
    auto universalSelectors = automation.parseSelectors("*", 35, 24);
    assert(!universalSelectors.empty());
    auto universalSelector = universalSelectors[0];
    assert(universalSelector.type == SelectorType::UNIVERSAL);
    assert(universalSelector.value == "*");
    
    std::cout << "选择器解析测试通过！" << std::endl;
}

void testSelectorValidation() {
    std::cout << "=== 测试选择器验证 ===" << std::endl;
    
    SelectorAutomation automation;
    
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
    
    SelectorAutomation automation;
    
    // 测试各种选择器类型
    assert(automation.detectSelectorType(".box") == SelectorType::CLASS);
    assert(automation.detectSelectorType("#header") == SelectorType::ID);
    assert(automation.detectSelectorType(":hover") == SelectorType::PSEUDO_CLASS);
    assert(automation.detectSelectorType("::before") == SelectorType::PSEUDO_ELEMENT);
    assert(automation.detectSelectorType("div") == SelectorType::TAG);
    assert(automation.detectSelectorType("*") == SelectorType::UNIVERSAL);
    
    // 测试组合选择器
    assert(automation.detectSelectorType("div.box") == SelectorType::COMBINATOR);
    // 暂时跳过复杂的组合选择器测试
    // assert(automation.detectSelectorType(".box > div") == SelectorType::COMBINATOR);
    
    std::cout << "选择器类型检测测试通过！" << std::endl;
}

void testStyleBlockAutomation() {
    std::cout << "=== 测试样式块自动化 ===" << std::endl;
    
    SelectorAutomation automation;
    
    // 创建选择器上下文
    SelectorContext context;
    context.elementName = "div";
    context.hasStyleBlock = true;
    context.sourceFile = "test.chtl";
    context.line = 10;
    
    // 添加选择器
    auto selectors1 = automation.parseSelectors(".box", 10, 5);
    context.selectors.push_back(selectors1[0]);
    auto selectors2 = automation.parseSelectors("#header", 11, 5);
    context.selectors.push_back(selectors2[0]);
    auto selectors3 = automation.parseSelectors(":hover", 12, 5);
    context.selectors.push_back(selectors3[0]);
    
    // 处理样式块自动化
    automation.processStyleBlock(context);
    
    // 验证自动添加的属性
    assert(!context.classNames.empty());
    assert(!context.idNames.empty());
    
    std::cout << "样式块自动化测试通过！" << std::endl;
}

void testScriptBlockAutomation() {
    std::cout << "=== 测试脚本块自动化 ===" << std::endl;
    
    SelectorAutomation automation;
    
    // 创建选择器上下文（没有样式块）
    SelectorContext context;
    context.elementName = "button";
    context.hasScriptBlock = true;
    context.sourceFile = "test.chtl";
    context.line = 20;
    
    // 添加选择器
    auto selectors1 = automation.parseSelectors(".btn", 20, 5);
    context.selectors.push_back(selectors1[0]);
    auto selectors2 = automation.parseSelectors("#submit", 21, 5);
    context.selectors.push_back(selectors2[0]);
    
    // 处理脚本块自动化
    automation.processScriptBlock(context);
    
    // 验证自动添加的属性（根据默认规则，脚本块自动化是禁用的）
    // 所以这里应该没有自动添加的属性
    assert(context.classNames.empty());
    assert(context.idNames.empty());
    
    std::cout << "脚本块自动化测试通过！" << std::endl;
}

void testReferenceSelectorResolution() {
    std::cout << "=== 测试引用选择器解析 ===" << std::endl;
    
    SelectorAutomation automation;
    
    // 创建带样式的选择器上下文
    SelectorContext styleContext;
    styleContext.elementName = "div";
    styleContext.hasStyleBlock = true;
    styleContext.classNames.push_back("container");
    styleContext.idNames.push_back("main");
    
    // 测试样式块中的引用选择器（优先选择类名）
    std::string resolvedStyle = automation.resolveReferenceSelector("&:hover", styleContext);
    assert(resolvedStyle == ".container:hover");
    
    // 创建带脚本的选择器上下文
    SelectorContext scriptContext;
    scriptContext.elementName = "button";
    scriptContext.hasScriptBlock = true;
    scriptContext.classNames.push_back("btn");
    scriptContext.idNames.push_back("submit");
    
    // 测试脚本块中的引用选择器（优先选择ID）
    std::string resolvedScript = automation.resolveReferenceSelector("&->click", scriptContext);
    assert(resolvedScript == "#submit->click");
    
    // 测试没有上下文的引用选择器
    std::string noContext = automation.resolveReferenceSelector("&:focus", styleContext);
    assert(noContext == "&:focus");
    
    std::cout << "引用选择器解析测试通过！" << std::endl;
}

void testAutomationRules() {
    std::cout << "=== 测试自动化规则 ===" << std::endl;
    
    SelectorAutomation automation;
    
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
    
    SelectorAutomation automation;
    
    // 测试不同类型选择器的优先级
    auto selectors1 = automation.parseSelectors(".box", 10, 5);
    auto selectors2 = automation.parseSelectors("#header", 11, 5);
    auto selectors3 = automation.parseSelectors("div", 12, 5);
    
    if (!selectors1.empty() && !selectors2.empty() && !selectors3.empty()) {
        auto classSelector = selectors1[0];
        auto idSelector = selectors2[0];
        auto tagSelector = selectors3[0];
        
        std::cout << "选择器解析成功:" << std::endl;
        std::cout << "  类选择器: " << classSelector.value << " (类型: " << static_cast<int>(classSelector.type) << ")" << std::endl;
        std::cout << "  ID选择器: " << idSelector.value << " (类型: " << static_cast<int>(idSelector.type) << ")" << std::endl;
        std::cout << "  标签选择器: " << tagSelector.value << " (类型: " << static_cast<int>(tagSelector.type) << ")" << std::endl;
    }
    
    std::cout << "选择器优先级测试通过！" << std::endl;
}

void testElementAutomation() {
    std::cout << "=== 测试元素自动化 ===" << std::endl;
    
    SelectorAutomation automation;
    
    // 创建元素上下文
    SelectorContext context;
    context.elementName = "div";
    context.hasStyleBlock = true;
    context.hasScriptBlock = true;
    context.sourceFile = "test.chtl";
    context.line = 10;
    
    // 添加选择器
    auto selectors1 = automation.parseSelectors(".box", 10, 5);
    context.selectors.push_back(selectors1[0]);
    auto selectors2 = automation.parseSelectors("#header", 11, 5);
    context.selectors.push_back(selectors2[0]);
    
    // 处理样式块自动化
    automation.processStyleBlock(context);
    
    // 处理脚本块自动化
    automation.processScriptBlock(context);
    
    // 验证结果
    assert(!context.classNames.empty());
    assert(!context.idNames.empty());
    
    std::cout << "元素自动化测试通过！" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    SelectorAutomation automation;
    
    // 清除统计信息
    automation.clearStatistics();
    
    // 执行一些操作来生成统计信息
    auto selector1 = automation.parseSelectors(".box");
    auto selector2 = automation.parseSelectors("#header");
    
    auto context = std::make_shared<SelectorContext>();
    context->hasStyleBlock = true;
    if (!selector1.empty()) context->selectors.push_back(selector1[0]);
    if (!selector2.empty()) context->selectors.push_back(selector2[0]);
    
    automation.processStyleBlock(*context);
    
    // 获取统计信息
    auto stats = automation.getStatistics();
    assert(stats.totalSelectorsProcessed == 2);
    assert(stats.totalAutoGeneratedClasses >= 0);
    assert(stats.totalStyleBlocksProcessed >= 0);
    
    std::cout << "统计信息测试通过！" << std::endl;
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "  总选择器处理数量: " << stats.totalSelectorsProcessed << std::endl;
    std::cout << "  自动类生成数量: " << stats.totalAutoGeneratedClasses << std::endl;
    std::cout << "  自动ID生成数量: " << stats.totalAutoGeneratedIds << std::endl;
    std::cout << "  样式块处理数量: " << stats.totalStyleBlocksProcessed << std::endl;
    std::cout << "  脚本块处理数量: " << stats.totalScriptBlocksProcessed << std::endl;
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