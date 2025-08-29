#include "src/CHTL/CHTLNode/TemplateNode.h"
#include "src/CHTL/CHTLNode/BaseNode.h"
#include <iostream>

void testStyleTemplate() {
    using namespace CHTL;
    
    std::cout << "Testing Style Template:\n";
    std::cout << "======================\n";
    
    // 创建样式模板
    auto styleTemplate = std::make_unique<StyleTemplateNode>("DefaultText");
    
    // 设置样式属性
    styleTemplate->setStyleProperty("color", "#333");
    styleTemplate->setStyleProperty("font-family", "Arial, sans-serif");
    styleTemplate->setStyleProperty("line-height", "1.5");
    styleTemplate->setStyleProperty("font-size", "16px");
    
    // 添加伪类
    styleTemplate->addPseudoClass("hover");
    styleTemplate->addPseudoClass("focus");
    
    // 添加伪元素
    styleTemplate->addPseudoElement("before");
    styleTemplate->addPseudoElement("after");
    
    // 测试功能
    std::cout << "Template: " << styleTemplate->toString() << "\n";
    std::cout << "Has color property: " << styleTemplate->hasStyleProperty("color") << "\n";
    std::cout << "Color value: " << styleTemplate->getStyleProperty("color") << "\n";
    std::cout << "Has hover pseudo-class: " << styleTemplate->hasPseudoClass("hover") << "\n";
    std::cout << "Has before pseudo-element: " << styleTemplate->hasPseudoElement("before") << "\n";
    
    // 生成CSS
    std::cout << "\nGenerated CSS:\n";
    std::cout << styleTemplate->generateCSS() << "\n";
    
    // 生成内联样式
    std::cout << "Generated inline style: " << styleTemplate->generateInlineStyle() << "\n";
    
    // 测试继承
    styleTemplate->addInheritance("BaseStyle");
    styleTemplate->addInheritance("FontStyle");
    std::cout << "Inherits from BaseStyle: " << styleTemplate->inheritsFrom("BaseStyle") << "\n";
    std::cout << "Inheritance chain size: " << styleTemplate->getInheritanceChain().size() << "\n";
    
    // 验证
    std::cout << "Template is valid: " << styleTemplate->validate() << "\n";
}

void testElementTemplate() {
    using namespace CHTL;
    
    std::cout << "\nTesting Element Template:\n";
    std::cout << "========================\n";
    
    // 创建元素模板
    auto elementTemplate = std::make_unique<ElementTemplateNode>("Card");
    
    // 添加元素序列
    elementTemplate->addElement("div");
    elementTemplate->addElement("header");
    elementTemplate->addElement("main");
    elementTemplate->addElement("footer");
    
    // 设置元素属性
    elementTemplate->setElementProperty("div", "class", "card");
    elementTemplate->setElementProperty("div", "role", "article");
    elementTemplate->setElementProperty("header", "class", "card-header");
    elementTemplate->setElementProperty("main", "class", "card-body");
    elementTemplate->setElementProperty("footer", "class", "card-footer");
    
    // 测试功能
    std::cout << "Template: " << elementTemplate->toString() << "\n";
    std::cout << "Element count: " << elementTemplate->getElementCount() << "\n";
    std::cout << "First element: " << elementTemplate->getElement(0) << "\n";
    std::cout << "Div has class property: " << elementTemplate->hasElementProperty("div", "class") << "\n";
    std::cout << "Div class value: " << elementTemplate->getElementProperty("div", "class") << "\n";
    
    // 生成HTML
    std::cout << "\nGenerated HTML:\n";
    std::cout << elementTemplate->generateHTML() << "\n";
    
    // 测试继承
    elementTemplate->addInheritance("BaseCard");
    std::cout << "Inherits from BaseCard: " << elementTemplate->inheritsFrom("BaseCard") << "\n";
    
    // 验证
    std::cout << "Template is valid: " << elementTemplate->validate() << "\n";
}

void testVarTemplate() {
    using namespace CHTL;
    
    std::cout << "\nTesting Variable Template:\n";
    std::cout << "==========================\n";
    
    // 创建变量模板
    auto varTemplate = std::make_unique<VarTemplateNode>("ThemeColors");
    
    // 设置变量
    varTemplate->setVariable("primary", "#007bff", "color");
    varTemplate->setVariable("secondary", "#6c757d", "color");
    varTemplate->setVariable("success", "#28a745", "color");
    varTemplate->setVariable("danger", "#dc3545", "color");
    varTemplate->setVariable("warning", "#ffc107", "color");
    varTemplate->setVariable("info", "#17a2b8", "color");
    varTemplate->setVariable("light", "#f8f9fa", "color");
    varTemplate->setVariable("dark", "#343a40", "color");
    
    // 设置变量注释
    varTemplate->setVariableComment("primary", "主要品牌色");
    varTemplate->setVariableComment("secondary", "次要色");
    varTemplate->setVariableComment("success", "成功状态色");
    
    // 测试功能
    std::cout << "Template: " << varTemplate->toString() << "\n";
    std::cout << "Variable count: " << varTemplate->getVariableNames().size() << "\n";
    std::cout << "Has primary variable: " << varTemplate->hasVariable("primary") << "\n";
    std::cout << "Primary value: " << varTemplate->getVariable("primary") << "\n";
    std::cout << "Primary type: " << varTemplate->getVariableType("primary") << "\n";
    std::cout << "Primary comment: " << varTemplate->getVariableComment("primary") << "\n";
    
    // 测试变量解析
    StringUnorderedMap context = {{"primary", "#0056b3"}, {"accent", "#ff6b35"}};
    std::cout << "Resolved primary (with context): " << varTemplate->resolveVariable("primary", context) << "\n";
    std::cout << "Resolved accent (from context): " << varTemplate->resolveVariable("accent", context) << "\n";
    
    // 生成变量声明
    std::cout << "\nGenerated Variable Declarations:\n";
    std::cout << varTemplate->generateVariableDeclarations() << "\n";
    
    // 生成JSON导出
    std::cout << "Generated JSON Export:\n";
    std::cout << varTemplate->generateJSONExport() << "\n";
    
    // 测试继承
    varTemplate->addInheritance("BaseColors");
    std::cout << "Inherits from BaseColors: " << varTemplate->inheritsFrom("BaseColors") << "\n";
    
    // 验证
    std::cout << "Template is valid: " << varTemplate->validate() << "\n";
}

void testTemplateInheritance() {
    using namespace CHTL;
    
    std::cout << "\nTesting Template Inheritance:\n";
    std::cout << "=============================\n";
    
    // 创建基础样式模板
    auto baseStyle = std::make_unique<StyleTemplateNode>("BaseText");
    baseStyle->setStyleProperty("font-family", "Arial, sans-serif");
    baseStyle->setStyleProperty("line-height", "1.4");
    
    // 创建继承的样式模板
    auto headingStyle = std::make_unique<StyleTemplateNode>("HeadingText");
    headingStyle->addInheritance("BaseText");
    headingStyle->setStyleProperty("font-weight", "bold");
    headingStyle->setStyleProperty("color", "#2c3e50");
    
    // 创建另一个继承的样式模板
    auto paragraphStyle = std::make_unique<StyleTemplateNode>("ParagraphText");
    paragraphStyle->addInheritance("BaseText");
    paragraphStyle->setStyleProperty("margin", "1em 0");
    paragraphStyle->setStyleProperty("color", "#34495e");
    
    std::cout << "Base template: " << baseStyle->toString() << "\n";
    std::cout << "Heading template: " << headingStyle->toString() << "\n";
    std::cout << "Paragraph template: " << paragraphStyle->toString() << "\n";
    
    // 测试继承链
    std::cout << "Heading inherits from BaseText: " << headingStyle->inheritsFrom("BaseText") << "\n";
    std::cout << "Paragraph inherits from BaseText: " << paragraphStyle->inheritsFrom("BaseText") << "\n";
    std::cout << "Heading inherits from Unknown: " << headingStyle->inheritsFrom("Unknown") << "\n";
}

void testTemplateCloning() {
    using namespace CHTL;
    
    std::cout << "\nTesting Template Cloning:\n";
    std::cout << "=========================\n";
    
    // 创建原始模板
    auto original = std::make_unique<StyleTemplateNode>("OriginalStyle");
    original->setStyleProperty("color", "red");
    original->setStyleProperty("background", "white");
    original->addInheritance("BaseStyle");
    original->setExported(true);
    
    // 克隆模板
    auto cloned = original->clone();
    StyleTemplateNode* clonedStyle = static_cast<StyleTemplateNode*>(cloned.get());
    
    std::cout << "Original: " << original->toString() << "\n";
    std::cout << "Cloned: " << clonedStyle->toString() << "\n";
    std::cout << "Cloned has color property: " << clonedStyle->hasStyleProperty("color") << "\n";
    std::cout << "Cloned color value: " << clonedStyle->getStyleProperty("color") << "\n";
    std::cout << "Cloned inherits from BaseStyle: " << clonedStyle->inheritsFrom("BaseStyle") << "\n";
    std::cout << "Cloned is exported: " << clonedStyle->getExported() << "\n";
    
    // 修改克隆的模板
    clonedStyle->setStyleProperty("color", "blue");
    clonedStyle->removeInheritance("BaseStyle");
    
    std::cout << "\nAfter modifying cloned template:\n";
    std::cout << "Original color: " << original->getStyleProperty("color") << "\n";
    std::cout << "Cloned color: " << clonedStyle->getStyleProperty("color") << "\n";
    std::cout << "Original inherits from BaseStyle: " << original->inheritsFrom("BaseStyle") << "\n";
    std::cout << "Cloned inherits from BaseStyle: " << clonedStyle->inheritsFrom("BaseStyle") << "\n";
}

int main() {
    std::cout << "CHTL Template System Test Suite\n";
    std::cout << "================================\n\n";
    
    try {
        testStyleTemplate();
        testElementTemplate();
        testVarTemplate();
        testTemplateInheritance();
        testTemplateCloning();
        
        std::cout << "\nAll template tests completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}