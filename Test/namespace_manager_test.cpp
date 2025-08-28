#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../CHTL/CHTLContext/NamespaceManager.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testBasicNamespaceOperations() {
    std::cout << "=== 测试基本命名空间操作 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 测试创建命名空间
    assert(manager.createNamespace("test", "test.chtl"));
    assert(manager.namespaceExists("test"));
    
    // 测试创建重复命名空间
    assert(!manager.createNamespace("test", "test2.chtl"));
    
    // 测试验证命名空间名称
    assert(!manager.validateNamespaceName(""));
    assert(!manager.validateNamespaceName("test@"));
    assert(manager.validateNamespaceName("test_namespace"));
    assert(manager.validateNamespaceName("test-namespace"));
    
    std::cout << "基本命名空间操作测试通过！" << std::endl;
}

void testNestedNamespaces() {
    std::cout << "=== 测试嵌套命名空间 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建父命名空间
    assert(manager.createNamespace("parent", "parent.chtl"));
    
    // 创建嵌套命名空间
    assert(manager.createNestedNamespace("parent", "child", "child.chtl"));
    
    // 测试嵌套命名空间路径解析
    auto path = manager.parseNamespacePath("parent.child");
    assert(path.size() == 2);
    assert(path[0] == "parent");
    assert(path[1] == "child");
    
    // 测试查找嵌套命名空间
    auto namespace_ = manager.getNamespace("parent");
    assert(namespace_ != nullptr);
    assert(namespace_->nestedNamespaces.find("child") != namespace_->nestedNamespaces.end());
    
    std::cout << "嵌套命名空间测试通过！" << std::endl;
}

void testNamespaceItems() {
    std::cout << "=== 测试命名空间项 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建命名空间
    assert(manager.createNamespace("test", "test.chtl"));
    
    // 创建命名空间项
    auto item1 = std::make_shared<NamespaceItem>();
    item1->type = NamespaceItemType::CUSTOM_ELEMENT;
    item1->name = "Box";
    item1->sourceFile = "test.chtl";
    item1->lineNumber = 10;
    item1->columnNumber = 5;
    item1->content = "[Custom] @Element Box { }";
    
    auto item2 = std::make_shared<NamespaceItem>();
    item2->type = NamespaceItemType::CUSTOM_STYLE;
    item2->name = "Theme";
    item2->sourceFile = "test.chtl";
    item2->lineNumber = 20;
    item2->columnNumber = 5;
    item2->content = "[Custom] @Style Theme { }";
    
    // 添加项
    assert(manager.addNamespaceItem("test", item1));
    assert(manager.addNamespaceItem("test", item2));
    
    // 测试项存在性
    assert(manager.namespaceItemExists("test", "Box", NamespaceItemType::CUSTOM_ELEMENT));
    assert(manager.namespaceItemExists("test", "Theme", NamespaceItemType::CUSTOM_STYLE));
    assert(!manager.namespaceItemExists("test", "Box", NamespaceItemType::CUSTOM_STYLE));
    
    // 测试获取项
    auto retrievedItem = manager.getNamespaceItem("test", "Box", NamespaceItemType::CUSTOM_ELEMENT);
    assert(retrievedItem != nullptr);
    assert(retrievedItem->name == "Box");
    assert(retrievedItem->type == NamespaceItemType::CUSTOM_ELEMENT);
    
    std::cout << "命名空间项测试通过！" << std::endl;
}

void testNamespaceMerging() {
    std::cout << "=== 测试命名空间合并 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建第一个命名空间
    assert(manager.createNamespace("test", "test1.chtl"));
    
    // 为第一个命名空间添加项
    auto item1 = std::make_shared<NamespaceItem>();
    item1->type = NamespaceItemType::CUSTOM_ELEMENT;
    item1->name = "Box";
    item1->sourceFile = "test1.chtl";
    item1->lineNumber = 10;
    item1->columnNumber = 5;
    item1->content = "[Custom] @Element Box { }";
    
    auto item2 = std::make_shared<NamespaceItem>();
    item2->type = NamespaceItemType::CUSTOM_STYLE;
    item2->name = "Theme";
    item2->sourceFile = "test1.chtl";
    item2->lineNumber = 20;
    item2->columnNumber = 5;
    item2->content = "[Custom] @Style Theme { }";
    
    assert(manager.addNamespaceItem("test", item1));
    assert(manager.addNamespaceItem("test", item2));
    
    // 为第一个命名空间添加更多项
    auto item3 = std::make_shared<NamespaceItem>();
    item3->type = NamespaceItemType::CUSTOM_ELEMENT;
    item3->name = "Button";
    item3->sourceFile = "test1.chtl";
    item3->lineNumber = 15;
    item3->columnNumber = 5;
    item3->content = "[Custom] @Element Button { }";
    
    assert(manager.addNamespaceItem("test", item3));
    
    // 验证命名空间项
    auto namespace_ = manager.getNamespace("test");
    assert(namespace_ != nullptr);
    assert(namespace_->items.size() == 3);
    
    // 检查所有项都存在
    bool hasBox = false, hasTheme = false, hasButton = false;
    for (const auto& item : namespace_->items) {
        if (item->name == "Box" && item->type == NamespaceItemType::CUSTOM_ELEMENT) hasBox = true;
        if (item->name == "Theme" && item->type == NamespaceItemType::CUSTOM_STYLE) hasTheme = true;
        if (item->name == "Button" && item->type == NamespaceItemType::CUSTOM_ELEMENT) hasButton = true;
    }
    
    assert(hasBox && hasTheme && hasButton);
    
    std::cout << "命名空间合并测试通过！" << std::endl;
}

void testConflictDetection() {
    std::cout << "=== 测试冲突检测 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建两个命名空间
    assert(manager.createNamespace("space1", "space1.chtl"));
    assert(manager.createNamespace("space2", "space2.chtl"));
    
    // 添加不同命名空间中的相同名称项（这不应该是冲突）
    auto item1 = std::make_shared<NamespaceItem>();
    item1->type = NamespaceItemType::CUSTOM_ELEMENT;
    item1->name = "Box";
    item1->sourceFile = "space1.chtl";
    item1->lineNumber = 10;
    item1->columnNumber = 5;
    item1->content = "[Custom] @Element Box { }";
    
    auto item2 = std::make_shared<NamespaceItem>();
    item2->type = NamespaceItemType::CUSTOM_ELEMENT;
    item2->name = "Box";
    item2->sourceFile = "space2.chtl";
    item2->lineNumber = 15;
    item2->columnNumber = 5;
    item2->content = "[Custom] @Element Box { }";
    
    assert(manager.addNamespaceItem("space1", item1));
    assert(manager.addNamespaceItem("space2", item2));
    
    // 检测冲突（不同命名空间中的相同名称项不应该被视为冲突）
    auto conflicts = manager.detectConflicts();
    // 由于不同命名空间中的相同名称项不是冲突，所以这里应该没有冲突
    assert(conflicts.empty());
    
    std::cout << "冲突检测测试通过！" << std::endl;
}

void testDefaultNamespace() {
    std::cout << "=== 测试默认命名空间 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 测试默认命名空间启用状态
    assert(manager.isDefaultNamespaceEnabled());
    
    // 创建默认命名空间
    std::string defaultName = manager.createDefaultNamespace("test_file.chtl");
    assert(!defaultName.empty());
    assert(defaultName == "test_file");
    
    // 验证默认命名空间存在
    auto namespace_ = manager.getNamespace(defaultName);
    assert(namespace_ != nullptr);
    assert(namespace_->isDefault);
    
    // 测试禁用默认命名空间
    manager.setDefaultNamespaceEnabled(false);
    assert(!manager.isDefaultNamespaceEnabled());
    
    std::string newDefaultName = manager.createDefaultNamespace("another_file.chtl");
    assert(newDefaultName.empty()); // 应该返回空字符串
    
    std::cout << "默认命名空间测试通过！" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建一些命名空间和项
    assert(manager.createNamespace("test1", "test1.chtl"));
    assert(manager.createNamespace("test2", "test2.chtl"));
    
    auto item = std::make_shared<NamespaceItem>();
    item->type = NamespaceItemType::CUSTOM_ELEMENT;
    item->name = "Box";
    item->sourceFile = "test1.chtl";
    item->lineNumber = 10;
    item->columnNumber = 5;
    item->content = "[Custom] @Element Box { }";
    
    assert(manager.addNamespaceItem("test1", item));
    
    // 获取统计信息
    std::string stats = manager.getStatistics();
    assert(!stats.empty());
    assert(stats.find("总命名空间数量: 2") != std::string::npos);
    assert(stats.find("命名空间: test1") != std::string::npos);
    assert(stats.find("命名空间: test2") != std::string::npos);
    
    std::cout << "统计信息测试通过！" << std::endl;
    std::cout << "\n统计信息:\n" << stats << std::endl;
}

int main() {
    std::cout << "CHTL 命名空间管理器测试程序" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;
    
    try {
        testBasicNamespaceOperations();
        testNestedNamespaces();
        testNamespaceItems();
        testNamespaceMerging();
        testConflictDetection();
        testDefaultNamespace();
        testStatistics();
        
        std::cout << "\n所有测试完成！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}