#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/AdvancedTemplateNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLManage/TemplateManager.h"
#include "../Error/ErrorReport.h"
#include <iostream>
#include <cassert>

using namespace CHTL;

void testBasicDeleteOperation() {
    std::cout << "\n=== 测试基础删除操作 ===" << std::endl;
    
    // 创建一个有子元素的父节点
    auto parentElement = std::make_shared<ElementNode>("div");
    auto child1 = std::make_shared<ElementNode>("span");
    child1->setId("child1");
    auto child2 = std::make_shared<ElementNode>("p");
    child2->setId("child2");
    auto child3 = std::make_shared<ElementNode>("a");
    child3->setId("child3");
    
    parentElement->addChild(child1);
    parentElement->addChild(child2);
    parentElement->addChild(child3);
    
    std::cout << "原始子元素数量: " << parentElement->getChildCount() << std::endl;
    
    // 创建删除操作：删除索引为1的子元素（即child2）
    SpecializationInfo deleteOp;
    deleteOp.type = SpecializationType::DELETE_PROPERTY;
    deleteOp.index = 1;
    
    // 应用删除操作
    TemplateManager templateManager;
    templateManager.applyDeleteOperation(parentElement, deleteOp);
    
    std::cout << "删除后子元素数量: " << parentElement->getChildCount() << std::endl;
    
    // 验证删除是否正确
    bool deleteSuccessful = (parentElement->getChildCount() == 2);
    std::cout << "删除操作: " << (deleteSuccessful ? "✓" : "❌") << std::endl;
    
    if (deleteSuccessful && parentElement->getChildCount() >= 2) {
        auto firstChild = std::dynamic_pointer_cast<ElementNode>(
            std::shared_ptr<BaseNode>(parentElement->getChild(0), [](BaseNode*){})
        );
        auto secondChild = std::dynamic_pointer_cast<ElementNode>(
            std::shared_ptr<BaseNode>(parentElement->getChild(1), [](BaseNode*){})
        );
        
        if (firstChild && secondChild) {
            bool correctOrder = (firstChild->getId() == "child1" && secondChild->getId() == "child3");
            std::cout << "删除顺序正确: " << (correctOrder ? "✓" : "❌") << std::endl;
        }
    }
}

void testBasicInsertOperation() {
    std::cout << "\n=== 测试基础插入操作 ===" << std::endl;
    
    // 创建父节点
    auto parentElement = std::make_shared<ElementNode>("div");
    auto child1 = std::make_shared<ElementNode>("span");
    child1->setId("child1");
    auto child2 = std::make_shared<ElementNode>("p");
    child2->setId("child2");
    
    parentElement->addChild(child1);
    parentElement->addChild(child2);
    
    std::cout << "原始子元素数量: " << parentElement->getChildCount() << std::endl;
    
    // 创建要插入的新元素
    auto newElement = std::make_shared<ElementNode>("h1");
    newElement->setId("newChild");
    
    // 创建插入操作：在索引1处插入新元素
    SpecializationInfo insertOp;
    insertOp.type = SpecializationType::INSERT_ELEMENT;
    insertOp.index = 1;
    insertOp.content.push_back(newElement);
    
    // 应用插入操作
    TemplateManager templateManager;
    templateManager.applyInsertOperation(parentElement, insertOp);
    
    std::cout << "插入后子元素数量: " << parentElement->getChildCount() << std::endl;
    
    // 验证插入是否正确
    bool insertSuccessful = (parentElement->getChildCount() == 3);
    std::cout << "插入操作: " << (insertSuccessful ? "✓" : "❌") << std::endl;
    
    if (insertSuccessful && parentElement->getChildCount() >= 3) {
        auto insertedChild = std::dynamic_pointer_cast<ElementNode>(
            std::shared_ptr<BaseNode>(parentElement->getChild(1), [](BaseNode*){})
        );
        
        if (insertedChild) {
            bool correctInsertion = (insertedChild->getId() == "newChild");
            std::cout << "插入位置正确: " << (correctInsertion ? "✓" : "❌") << std::endl;
        }
    }
}

void testIndexAccess() {
    std::cout << "\n=== 测试索引访问 ===" << std::endl;
    
    // 创建有多个子元素的父节点
    auto parentElement = std::make_shared<ElementNode>("ul");
    
    for (int i = 0; i < 5; ++i) {
        auto listItem = std::make_shared<ElementNode>("li");
        listItem->setId("item" + std::to_string(i));
        parentElement->addChild(listItem);
    }
    
    std::cout << "总子元素数量: " << parentElement->getChildCount() << std::endl;
    
    // 创建索引访问操作
    SpecializationInfo indexOp;
    indexOp.type = SpecializationType::INDEX_ACCESS;
    indexOp.index = 2; // 访问第3个元素（索引2）
    
    // 应用索引访问
    TemplateManager templateManager;
    auto accessedNode = templateManager.applyIndexAccess(parentElement, indexOp);
    
    bool accessSuccessful = (accessedNode != nullptr);
    std::cout << "索引访问: " << (accessSuccessful ? "✓" : "❌") << std::endl;
    
    if (accessSuccessful) {
        auto accessedElement = std::dynamic_pointer_cast<ElementNode>(accessedNode);
        if (accessedElement) {
            bool correctElement = (accessedElement->getId() == "item2");
            std::cout << "访问元素正确: " << (correctElement ? "✓" : "❌") << std::endl;
            std::cout << "访问到的元素ID: " << accessedElement->getId() << std::endl;
        }
    }
}

void testAttributeOperation() {
    std::cout << "\n=== 测试属性操作 ===" << std::endl;
    
    // 创建元素
    auto element = std::make_shared<ElementNode>("button");
    element->setClass("btn primary");
    element->setId("submit-btn");
    
    std::cout << "原始class: '" << element->getClass() << "'" << std::endl;
    std::cout << "原始id: '" << element->getId() << "'" << std::endl;
    
    // 创建删除class属性的操作
    SpecializationInfo deleteClassOp;
    deleteClassOp.type = SpecializationType::DELETE_PROPERTY;
    deleteClassOp.target = "class";
    
    // 应用删除class操作
    TemplateManager templateManager;
    templateManager.applyAttributeOperation(element, deleteClassOp);
    
    std::cout << "删除class后: '" << element->getClass() << "'" << std::endl;
    
    // 创建添加class的操作
    SpecializationInfo addClassOp;
    addClassOp.type = SpecializationType::INSERT_ELEMENT;
    addClassOp.target = "class";
    addClassOp.value = "new-class";
    
    // 应用添加class操作
    templateManager.applyAttributeOperation(element, addClassOp);
    
    std::cout << "添加class后: '" << element->getClass() << "'" << std::endl;
    
    bool operationSuccessful = (element->getClass() == "new-class");
    std::cout << "属性操作: " << (operationSuccessful ? "✓" : "❌") << std::endl;
}

void testElementFinderBySelector() {
    std::cout << "\n=== 测试选择器元素查找 ===" << std::endl;
    
    // 创建DOM结构
    auto container = std::make_shared<ElementNode>("div");
    container->setClass("container");
    
    auto header = std::make_shared<ElementNode>("h1");
    header->setId("main-title");
    header->setClass("title");
    
    auto paragraph = std::make_shared<ElementNode>("p");
    paragraph->setClass("content");
    
    auto button = std::make_shared<ElementNode>("button");
    button->setId("action-btn");
    button->setClass("btn primary");
    
    container->addChild(header);
    container->addChild(paragraph);
    container->addChild(button);
    
    TemplateManager templateManager;
    
    // 测试标签选择器
    auto buttons = templateManager.findElementsBySelector(container, "button");
    std::cout << "按标签查找button数量: " << buttons.size() << std::endl;
    
    // 测试ID选择器
    auto titleById = templateManager.findElementsBySelector(container, "#main-title");
    std::cout << "按ID查找#main-title数量: " << titleById.size() << std::endl;
    
    // 测试类选择器
    auto titleByClass = templateManager.findElementsBySelector(container, ".title");
    std::cout << "按类查找.title数量: " << titleByClass.size() << std::endl;
    
    bool findingWorks = (buttons.size() == 1) && (titleById.size() == 1) && (titleByClass.size() == 1);
    std::cout << "选择器查找: " << (findingWorks ? "✓" : "❌") << std::endl;
}

void testCloneAndSpecialize() {
    std::cout << "\n=== 测试克隆与特化 ===" << std::endl;
    
    // 创建源节点
    auto sourceElement = std::make_shared<ElementNode>("div");
    sourceElement->setClass("original");
    sourceElement->setId("source");
    
    auto child = std::make_shared<ElementNode>("span");
    child->setId("child");
    sourceElement->addChild(child);
    
    std::cout << "源节点子元素数量: " << sourceElement->getChildCount() << std::endl;
    std::cout << "源节点class: '" << sourceElement->getClass() << "'" << std::endl;
    
    // 创建特化操作：修改class属性
    std::vector<SpecializationInfo> specializations;
    
    SpecializationInfo changeClassOp;
    changeClassOp.type = SpecializationType::INSERT_ELEMENT;
    changeClassOp.target = "class";
    changeClassOp.value = "specialized";
    specializations.push_back(changeClassOp);
    
    // 克隆并应用特化
    TemplateManager templateManager;
    auto specializedNode = templateManager.cloneAndSpecialize(sourceElement, specializations);
    
    bool cloneSuccessful = (specializedNode != nullptr);
    std::cout << "克隆成功: " << (cloneSuccessful ? "✓" : "❌") << std::endl;
    
    if (cloneSuccessful) {
        auto specializedElement = std::dynamic_pointer_cast<ElementNode>(specializedNode);
        if (specializedElement) {
            std::cout << "特化后class: '" << specializedElement->getClass() << "'" << std::endl;
            std::cout << "特化后子元素数量: " << specializedElement->getChildCount() << std::endl;
            
            // 验证源节点未被修改
            bool sourceUnchanged = (sourceElement->getClass() == "original");
            std::cout << "源节点未变: " << (sourceUnchanged ? "✓" : "❌") << std::endl;
            
            // 验证特化节点被正确修改
            bool specializationApplied = specializedElement->hasClass("specialized");
            std::cout << "特化应用成功: " << (specializationApplied ? "✓" : "❌") << std::endl;
        }
    }
}

int main() {
    std::cout << "模板特化操作系统测试" << std::endl;
    std::cout << "=====================" << std::endl;
    
    testBasicDeleteOperation();
    testBasicInsertOperation();
    testIndexAccess();
    testAttributeOperation();
    testElementFinderBySelector();
    testCloneAndSpecialize();
    
    std::cout << "\n=== 模板特化测试完成 ===" << std::endl;
    
    return 0;
}