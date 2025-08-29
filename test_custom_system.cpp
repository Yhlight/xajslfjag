#include "src/CHTL/CHTLNode/CustomNode.h"
#include "src/CHTL/CHTLNode/BaseNode.h"
#include <iostream>

void testCustomStyleNode() {
    using namespace CHTL;
    
    std::cout << "Testing Custom Style Node:\n";
    std::cout << "==========================\n";
    
    // 创建自定义样式组
    auto customStyle = std::make_unique<CustomStyleNode>("ButtonStyle");
    
    // 设置基础属性
    customStyle->setBaseProperty("padding", "10px 20px");
    customStyle->setBaseProperty("border", "1px solid #ccc");
    customStyle->setBaseProperty("border-radius", "4px");
    customStyle->setBaseProperty("cursor", "pointer");
    customStyle->setBaseProperty("background-color", "{bg-color}");  // 参数化属性
    customStyle->setBaseProperty("color", "{text-color}");
    
    // 添加可选属性
    customStyle->addOptionalProperty("box-shadow");
    customStyle->addOptionalProperty("transition");
    
    // 添加必需属性
    customStyle->addRequiredProperty("bg-color");
    customStyle->addRequiredProperty("text-color");
    
    // 设置默认值
    customStyle->setDefaultValue("bg-color", "#f8f9fa");
    customStyle->setDefaultValue("text-color", "#212529");
    customStyle->setDefaultValue("transition", "all 0.2s ease");
    
    // 测试功能
    std::cout << "Custom Style: " << customStyle->toString() << "\n";
    std::cout << "Has padding property: " << customStyle->hasBaseProperty("padding") << "\n";
    std::cout << "Padding value: " << customStyle->getBaseProperty("padding") << "\n";
    std::cout << "Is bg-color required: " << customStyle->isRequiredProperty("bg-color") << "\n";
    std::cout << "Has transition default: " << customStyle->hasDefaultValue("transition") << "\n";
    
    // 测试参数化
    StringUnorderedMap parameters = {
        {"bg-color", "#007bff"},
        {"text-color", "white"}
    };
    
    std::cout << "Validates parameters: " << customStyle->validateParameters(parameters) << "\n";
    auto specialized = customStyle->getSpecializedProperties(parameters);
    std::cout << "Specialized background-color: " << specialized["background-color"] << "\n";
    
    // 生成CSS
    std::cout << "\nGenerated CSS:\n";
    std::cout << customStyle->generateCSS(".btn", parameters) << "\n";
    
    // 生成内联样式
    std::cout << "Generated inline style: " << customStyle->generateInlineStyle(parameters) << "\n";
    
    // 测试删除操作
    customStyle->deleteProperty("border");
    specialized = customStyle->getSpecializedProperties(parameters);
    std::cout << "After deleting border, has border: " << (specialized.find("border") != specialized.end()) << "\n";
    
    // 测试特例化
    customStyle->addSpecialization("BaseButtonStyle");
    std::cout << "Specializes from BaseButtonStyle: " << customStyle->specializesFrom("BaseButtonStyle") << "\n";
}

void testCustomElementNode() {
    using namespace CHTL;
    
    std::cout << "\nTesting Custom Element Node:\n";
    std::cout << "============================\n";
    
    // 创建自定义元素
    auto customElement = std::make_unique<CustomElementNode>("FormCard");
    
    // 添加元素序列
    customElement->addElement("div");
    customElement->addElement("header");
    customElement->addElement("main");
    customElement->addElement("footer");
    
    // 设置元素属性
    customElement->setElementProperty("div", "class", "form-card");
    customElement->setElementProperty("div", "data-type", "{card-type}");
    customElement->setElementProperty("header", "class", "form-card-header");
    customElement->setElementProperty("main", "class", "form-card-body");
    customElement->setElementProperty("footer", "class", "form-card-footer");
    
    // 添加样式到元素
    customElement->addStyleToElement("div", "border: 1px solid #ddd; border-radius: 8px;");
    customElement->addStyleToElement("header", "background: {header-bg}; padding: 15px;");
    customElement->addStyleToElement("main", "padding: 20px;");
    
    // 测试功能
    std::cout << "Custom Element: " << customElement->toString() << "\n";
    std::cout << "Element count: " << customElement->getElementCount() << "\n";
    std::cout << "First element: " << customElement->getElementAt(0) << "\n";
    std::cout << "Div class: " << customElement->getElementProperty("div", "class") << "\n";
    
    // 测试索引访问
    customElement->setElementIndex("main", 2);
    std::cout << "Main element index: " << customElement->getElementIndex("main") << "\n";
    std::cout << "Element at index 2: " << customElement->getElementByIndex(2) << "\n";
    
    // 测试插入操作
    customElement->insertAfter("header", "section");
    customElement->insertAtTop("article");
    customElement->insertAtBottom("aside");
    
    std::cout << "Insertion points: " << customElement->getInsertions().size() << "\n";
    
    // 测试删除操作
    customElement->deleteElement("footer");
    std::cout << "Footer is deleted: " << customElement->isElementDeleted("footer") << "\n";
    
    // 生成HTML
    StringUnorderedMap parameters = {
        {"card-type", "registration"},
        {"header-bg", "#f8f9fa"}
    };
    
    std::cout << "\nGenerated HTML:\n";
    std::cout << customElement->generateHTML(parameters) << "\n";
    
    // 生成元素列表
    auto elementList = customElement->generateElementList();
    std::cout << "Active elements: ";
    for (const auto& elem : elementList) {
        std::cout << elem << " ";
    }
    std::cout << "\n";
}

void testCustomVarNode() {
    using namespace CHTL;
    
    std::cout << "\nTesting Custom Variable Node:\n";
    std::cout << "=============================\n";
    
    // 创建自定义变量组
    auto customVar = std::make_unique<CustomVarNode>("AppTheme");
    
    // 设置变量
    customVar->setVariable("primary-color", "{primary}", "color");
    customVar->setVariable("secondary-color", "{secondary}", "color");
    customVar->setVariable("font-size", "16px", "size");
    customVar->setVariable("line-height", "1.5", "number");
    customVar->setVariable("border-radius", "4px", "size");
    customVar->setVariable("transition-duration", "0.3s", "time");
    
    // 设置约束
    customVar->setVariableConstraint("primary-color", "#");  // 必须包含#
    customVar->setVariableConstraint("font-size", "px|em|rem");  // 必须是这些单位之一
    
    // 测试功能
    std::cout << "Custom Variable: " << customVar->toString() << "\n";
    std::cout << "Variable count: " << customVar->getVariableNames().size() << "\n";
    std::cout << "Has primary-color: " << customVar->hasVariable("primary-color") << "\n";
    std::cout << "Primary color type: " << customVar->getVariableType("primary-color") << "\n";
    std::cout << "Font size constraint: " << customVar->getVariableConstraint("font-size") << "\n";
    
    // 测试变量解析
    StringUnorderedMap context = {
        {"primary", "#007bff"},
        {"secondary", "#6c757d"}
    };
    
    std::cout << "Resolved primary-color: " << customVar->resolveVariable("primary-color", context) << "\n";
    
    auto allResolved = customVar->resolveAllVariables(context);
    std::cout << "All resolved variables count: " << allResolved.size() << "\n";
    
    // 测试删除操作
    customVar->deleteVariable("transition-duration");
    std::cout << "Transition-duration is deleted: " << customVar->isVariableDeleted("transition-duration") << "\n";
    
    // 生成变量声明
    std::cout << "\nGenerated Variable Declarations:\n";
    std::cout << customVar->generateVariableDeclarations(context) << "\n";
    
    // 生成JSON导出
    std::cout << "Generated JSON Export:\n";
    std::cout << customVar->generateJSONExport(context) << "\n";
    
    // 测试特例化
    customVar->addSpecialization("BaseTheme");
    std::cout << "Specializes from BaseTheme: " << customVar->specializesFrom("BaseTheme") << "\n";
}

void testCustomSpecialization() {
    using namespace CHTL;
    
    std::cout << "\nTesting Custom Specialization:\n";
    std::cout << "==============================\n";
    
    // 创建基础自定义样式
    auto baseStyle = std::make_unique<CustomStyleNode>("BaseButton");
    baseStyle->setBaseProperty("padding", "8px 16px");
    baseStyle->setBaseProperty("border", "1px solid #ccc");
    baseStyle->setBaseProperty("cursor", "pointer");
    baseStyle->setBaseProperty("display", "inline-block");
    
    // 创建特例化样式
    auto primaryButton = std::make_unique<CustomStyleNode>("PrimaryButton");
    primaryButton->addSpecialization("BaseButton");
    primaryButton->setBaseProperty("background-color", "#007bff");
    primaryButton->setBaseProperty("color", "white");
    primaryButton->setBaseProperty("border-color", "#007bff");
    
    // 创建危险按钮样式（继承主要按钮，但覆盖颜色）
    auto dangerButton = std::make_unique<CustomStyleNode>("DangerButton");
    dangerButton->addSpecialization("PrimaryButton");
    dangerButton->setBaseProperty("background-color", "#dc3545");
    dangerButton->setBaseProperty("border-color", "#dc3545");
    
    // 删除某些继承属性
    dangerButton->deleteProperty("border");  // 危险按钮不要边框
    
    std::cout << "Base button: " << baseStyle->toString() << "\n";
    std::cout << "Primary button: " << primaryButton->toString() << "\n";
    std::cout << "Danger button: " << dangerButton->toString() << "\n";
    
    // 生成CSS
    std::cout << "\nBase Button CSS:\n";
    std::cout << baseStyle->generateCSS(".btn") << "\n";
    
    std::cout << "Primary Button CSS:\n";
    std::cout << primaryButton->generateCSS(".btn-primary") << "\n";
    
    std::cout << "Danger Button CSS:\n";
    std::cout << dangerButton->generateCSS(".btn-danger") << "\n";
    
    // 测试特例化链
    std::cout << "Primary button specialization chain: ";
    for (const auto& spec : primaryButton->getSpecializationChain()) {
        std::cout << spec << " ";
    }
    std::cout << "\n";
    
    std::cout << "Danger button specialization chain: ";
    for (const auto& spec : dangerButton->getSpecializationChain()) {
        std::cout << spec << " ";
    }
    std::cout << "\n";
}

void testCustomCloning() {
    using namespace CHTL;
    
    std::cout << "\nTesting Custom Cloning:\n";
    std::cout << "=======================\n";
    
    // 创建原始自定义样式
    auto original = std::make_unique<CustomStyleNode>("OriginalCard");
    original->setBaseProperty("padding", "20px");
    original->setBaseProperty("border", "1px solid #ddd");
    original->setBaseProperty("background", "white");
    original->addRequiredProperty("width");
    original->setDefaultValue("width", "300px");
    original->addSpecialization("BaseCard");
    original->setExported(true);
    
    // 克隆自定义样式
    auto cloned = original->clone();
    CustomStyleNode* clonedStyle = static_cast<CustomStyleNode*>(cloned.get());
    
    std::cout << "Original: " << original->toString() << "\n";
    std::cout << "Cloned: " << clonedStyle->toString() << "\n";
    
    // 验证克隆结果
    std::cout << "Cloned has padding: " << clonedStyle->hasBaseProperty("padding") << "\n";
    std::cout << "Cloned padding value: " << clonedStyle->getBaseProperty("padding") << "\n";
    std::cout << "Cloned is width required: " << clonedStyle->isRequiredProperty("width") << "\n";
    std::cout << "Cloned width default: " << clonedStyle->getDefaultValue("width") << "\n";
    std::cout << "Cloned specializes from BaseCard: " << clonedStyle->specializesFrom("BaseCard") << "\n";
    std::cout << "Cloned is exported: " << clonedStyle->getExported() << "\n";
    
    // 修改克隆的样式
    clonedStyle->setBaseProperty("padding", "30px");
    clonedStyle->removeSpecialization("BaseCard");
    clonedStyle->setExported(false);
    
    std::cout << "\nAfter modifying cloned style:\n";
    std::cout << "Original padding: " << original->getBaseProperty("padding") << "\n";
    std::cout << "Cloned padding: " << clonedStyle->getBaseProperty("padding") << "\n";
    std::cout << "Original specializes from BaseCard: " << original->specializesFrom("BaseCard") << "\n";
    std::cout << "Cloned specializes from BaseCard: " << clonedStyle->specializesFrom("BaseCard") << "\n";
    std::cout << "Original is exported: " << original->getExported() << "\n";
    std::cout << "Cloned is exported: " << clonedStyle->getExported() << "\n";
}

int main() {
    std::cout << "CHTL Custom System Test Suite\n";
    std::cout << "=============================\n\n";
    
    try {
        testCustomStyleNode();
        testCustomElementNode();
        testCustomVarNode();
        testCustomSpecialization();
        testCustomCloning();
        
        std::cout << "\nAll custom system tests completed successfully!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}