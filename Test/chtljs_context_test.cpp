#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../CHTL JS/CHTLJSContext/CHTLJSContext.h"

using namespace CHTLJS;

void testEnhancedSelectorParsing() {
    std::cout << "=== 测试增强选择器解析 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试类选择器
    auto classSelector = context.parseEnhancedSelector("{{.box}}");
    assert(classSelector->elementType == "class");
    assert(classSelector->elementValue == "box");
    assert(!classSelector->isIndexed);
    
    // 测试ID选择器
    auto idSelector = context.parseEnhancedSelector("{{#header}}");
    assert(idSelector->elementType == "id");
    assert(idSelector->elementValue == "header");
    
    // 测试标签选择器
    auto tagSelector = context.parseEnhancedSelector("{{div}}");
    assert(tagSelector->elementType == "tag");
    assert(tagSelector->elementValue == "div");
    
    // 测试索引选择器
    auto indexedSelector = context.parseEnhancedSelector("{{button[0]}}");
    assert(indexedSelector->elementType == "tag");
    assert(indexedSelector->elementValue == "button");
    assert(indexedSelector->isIndexed);
    assert(indexedSelector->index == 0);
    
    // 测试后代选择器
    auto descendantSelector = context.parseEnhancedSelector("{{.container button}}");
    assert(descendantSelector->parentSelector == ".container");
    assert(descendantSelector->elementType == "tag");
    assert(descendantSelector->elementValue == "button");
    
    std::cout << "增强选择器解析测试通过！" << std::endl;
}

void testDOMReferenceCreation() {
    std::cout << "=== 测试DOM引用创建 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试类选择器的DOM引用
    auto classSelector = context.parseEnhancedSelector("{{.box}}");
    std::string domRef = context.createDOMReference(classSelector);
    assert(domRef == "document.querySelector('.box')");
    
    // 测试ID选择器的DOM引用
    auto idSelector = context.parseEnhancedSelector("{{#header}}");
    domRef = context.createDOMReference(idSelector);
    assert(domRef == "document.getElementById('header')");
    
    // 测试标签选择器的DOM引用
    auto tagSelector = context.parseEnhancedSelector("{{div}}");
    domRef = context.createDOMReference(tagSelector);
    assert(domRef == "document.querySelectorAll('div')");
    
    // 测试索引选择器的DOM引用
    auto indexedSelector = context.parseEnhancedSelector("{{button[0]}}");
    domRef = context.createDOMReference(indexedSelector);
    assert(domRef == "document.querySelectorAll('button')[0]");
    
    // 测试后代选择器的DOM引用
    auto descendantSelector = context.parseEnhancedSelector("{{.container button}}");
    domRef = context.createDOMReference(descendantSelector);
    assert(domRef.find("document.querySelector('.container')") != std::string::npos);
    assert(domRef.find("querySelectorAll('button')") != std::string::npos);
    
    std::cout << "DOM引用创建测试通过！" << std::endl;
}

void testEventBindingOperator() {
    std::cout << "=== 测试事件绑定操作符 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试事件绑定操作符
    std::string eventCode = context.parseEventBindingOperator("{{.box}}", "click", "function() { console.log('clicked'); }");
    assert(eventCode.find("addEventListener('click'") != std::string::npos);
    assert(eventCode.find("console.log('clicked')") != std::string::npos);
    
    std::cout << "事件绑定操作符测试通过！" << std::endl;
}

void testEnhancedListener() {
    std::cout << "=== 测试增强监听器 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 创建事件监听器列表
    std::vector<EventListener> listeners;
    
    EventListener clickListener;
    clickListener.eventType = "click";
    clickListener.callback = "function() { console.log('clicked'); }";
    clickListener.isFunction = true;
    listeners.push_back(clickListener);
    
    EventListener mouseListener;
    mouseListener.eventType = "mouseenter";
    mouseListener.callback = "handleMouseEnter";
    mouseListener.isReference = true;
    listeners.push_back(mouseListener);
    
    // 测试增强监听器解析
    std::string listenerCode = context.parseEnhancedListener("{{.box}}", listeners);
    assert(listenerCode.find("addEventListener('click'") != std::string::npos);
    assert(listenerCode.find("addEventListener('mouseenter'") != std::string::npos);
    
    std::cout << "增强监听器测试通过！" << std::endl;
}

void testEventDelegation() {
    std::cout << "=== 测试事件委托 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 创建事件委托配置
    EventDelegation delegation;
    delegation.parentSelector = ".container";
    delegation.targetSelectors = {".button", ".link"};
    delegation.eventHandlers["click"] = "console.log('clicked');";
    delegation.eventHandlers["mouseenter"] = "console.log('mouse entered');";
    
    // 测试事件委托解析
    std::string delegationCode = context.parseEventDelegation(delegation);
    assert(delegationCode.find("document.querySelector('.container')") != std::string::npos);
    assert(delegationCode.find("addEventListener('click'") != std::string::npos);
    assert(delegationCode.find("addEventListener('mouseenter'") != std::string::npos);
    
    std::cout << "事件委托测试通过！" << std::endl;
}

void testAnimation() {
    std::cout << "=== 测试动画配置 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 创建动画配置
    AnimationConfig config;
    config.target = "{{.box}}";
    config.duration = 1000;
    config.easing = "ease-in-out";
    config.beginState["opacity"] = "0";
    config.beginState["transform"] = "scale(0.5)";
    
    // 创建关键帧
    AnimationKeyframe keyframe1;
    keyframe1.at = 0.5;
    keyframe1.cssProperties["opacity"] = "0.5";
    keyframe1.cssProperties["transform"] = "scale(0.75)";
    config.keyframes.push_back(keyframe1);
    
    config.endState["opacity"] = "1";
    config.endState["transform"] = "scale(1)";
    config.loop = 1;
    config.delay = 100;
    
    // 测试动画配置解析
    std::string animationCode = context.parseAnimation(config);
    assert(animationCode.find("duration: 1000") != std::string::npos);
    assert(animationCode.find("easing: 'ease-in-out'") != std::string::npos);
    assert(animationCode.find("opacity: '0'") != std::string::npos);
    assert(animationCode.find("requestAnimationFrame") != std::string::npos);
    
    std::cout << "动画配置测试通过！" << std::endl;
}

void testVirtualObject() {
    std::cout << "=== 测试虚对象 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 创建虚对象
    std::string sourceCode = R"(
        click: () => { console.log('clicked'); },
        text: 'Hello World',
        count: 42
    )";
    
    auto virtualObject = context.createVirtualObject("TestObject", sourceCode);
    assert(virtualObject->name == "TestObject");
    assert(virtualObject->methods.find("click") != virtualObject->methods.end());
    assert(virtualObject->properties.find("text") != virtualObject->properties.end());
    
    // 测试虚对象访问
    std::string methodAccess = context.parseVirtualObjectAccess("TestObject", "click");
    assert(methodAccess.find("Method call: TestObject.click()") != std::string::npos);
    
    std::string propertyAccess = context.parseVirtualObjectAccess("TestObject", "text");
    assert(propertyAccess.find("Property access: TestObject.text") != std::string::npos);
    
    std::cout << "虚对象测试通过！" << std::endl;
}

void testModuleImports() {
    std::cout << "=== 测试模块导入 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 创建模块导入列表
    std::vector<ModuleImport> imports;
    
    ModuleImport import1;
    import1.path = "./utils.js";
    import1.alias = "utils";
    imports.push_back(import1);
    
    ModuleImport import2;
    import2.path = "./components.js";
    imports.push_back(import2);
    
    // 测试模块导入解析
    std::string importCode = context.parseModuleImports(imports);
    assert(importCode.find("import './utils.js' as utils") != std::string::npos);
    assert(importCode.find("import './components.js'") != std::string::npos);
    
    std::cout << "模块导入测试通过！" << std::endl;
}

void testSelectorValidation() {
    std::cout << "=== 测试选择器验证 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试有效选择器
    assert(context.validateSelector("{{.box}}"));
    assert(context.validateSelector("{{#header}}"));
    assert(context.validateSelector("{{div}}"));
    assert(context.validateSelector("{{button[0]}}"));
    assert(context.validateSelector("{{.container button}}"));
    
    // 测试无效选择器
    assert(!context.validateSelector(""));
    assert(!context.validateSelector("{{}}"));
    assert(!context.validateSelector("{{invalid@}}"));
    assert(!context.validateSelector("{{123invalid}}"));
    
    std::cout << "选择器验证测试通过！" << std::endl;
}

void testSelectorTypeDetection() {
    std::cout << "=== 测试选择器类型检测 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试选择器类型检测
    assert(context.getSelectorType("{{.box}}") == "class");
    assert(context.getSelectorType("{{#header}}") == "id");
    assert(context.getSelectorType("{{div}}") == "tag");
    assert(context.getSelectorType("{{button[0]}}") == "indexed");
    assert(context.getSelectorType("{{.container button}}") == "descendant");
    
    std::cout << "选择器类型检测测试通过！" << std::endl;
}

void testJavaScriptGeneration() {
    std::cout << "=== 测试JavaScript代码生成 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 测试JavaScript代码生成
    std::string jsCode = context.generateJavaScript("Test Context");
    assert(jsCode.find("// Generated JavaScript from CHTL JS") != std::string::npos);
    assert(jsCode.find("// Context: Test Context") != std::string::npos);
    
    std::cout << "JavaScript代码生成测试通过！" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    CHTLJSContext context;
    
    // 清除统计信息
    context.clearStatistics();
    
    // 执行一些操作来生成统计信息
    context.parseEnhancedSelector("{{.box}}");
    context.parseEnhancedSelector("{{#header}}");
    
    // 获取统计信息
    std::string stats = context.getStatistics();
    assert(!stats.empty());
    assert(stats.find("总选择器处理数量: 2") != std::string::npos);
    
    std::cout << "统计信息测试通过！" << std::endl;
    std::cout << "\n统计信息:\n" << stats << std::endl;
}

int main() {
    std::cout << "CHTL JS 上下文管理器测试程序" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;
    
    try {
        testEnhancedSelectorParsing();
        testDOMReferenceCreation();
        testEventBindingOperator();
        testEnhancedListener();
        testEventDelegation();
        testAnimation();
        testVirtualObject();
        testModuleImports();
        testSelectorValidation();
        testSelectorTypeDetection();
        testJavaScriptGeneration();
        testStatistics();
        
        std::cout << "\n所有测试完成！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}