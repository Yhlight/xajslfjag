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
    assert(manager.hasNamespace("test"));
    
    // 测试创建重复命名空间
    assert(!manager.createNamespace("test", "test2.chtl"));
    
    // 测试验证命名空间名称
    assert(!manager.validateNamespace(""));
    assert(!manager.validateNamespace("test@"));
    assert(manager.validateNamespace("test_namespace"));
    assert(manager.validateNamespace("test-namespace"));
    
    std::cout << "基本命名空间操作测试通过！" << std::endl;
}

void testNestedNamespaces() {
    std::cout << "=== 测试嵌套命名空间 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建嵌套命名空间
    assert(manager.createNamespace("Core", "core.chtl"));
    assert(manager.createNestedNamespace("Core", "UI", "ui.chtl"));
    assert(manager.createNestedNamespace("UI", "Components", "components.chtl"));
    
    // 添加项到嵌套命名空间
    auto coreItem = std::make_shared<NamespaceItem>("Version", NamespaceItemType::CUSTOM_ELEMENT, "core.chtl", 10, 5);
    auto uiItem = std::make_shared<NamespaceItem>("Theme", NamespaceItemType::CUSTOM_STYLE, "ui.chtl", 20, 5);
    auto componentItem = std::make_shared<NamespaceItem>("Button", NamespaceItemType::CUSTOM_ELEMENT, "components.chtl", 30, 5);
    
    manager.addNamespaceItem("Core", coreItem);
    manager.addNamespaceItem("UI", uiItem);
    manager.addNamespaceItem("Components", componentItem);
    
    // 测试嵌套命名空间项获取
    auto coreItems = manager.getNamespaceItems("Core");
    auto uiItems = manager.getNamespaceItems("UI");
    auto componentItems = manager.getNamespaceItems("Components");
    
    assert(coreItems.size() == 1);
    assert(uiItems.size() == 1);
    assert(componentItems.size() == 1);
    
    std::cout << "嵌套命名空间测试通过！" << std::endl;
}

void testNamespaceItems() {
    std::cout << "=== 测试命名空间项 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建命名空间
    assert(manager.createNamespace("test", "test.chtl"));
    
    // 创建命名空间项
    auto item1 = std::make_shared<NamespaceItem>("Box", NamespaceItemType::CUSTOM_ELEMENT, "test.chtl", 10, 5);
    item1->content = "[Custom] @Element Box { }";
    
    auto item2 = std::make_shared<NamespaceItem>("Theme", NamespaceItemType::CUSTOM_STYLE, "test.chtl", 20, 5);
    item2->content = "[Custom] @Style Theme { }";
    
    // 添加项
    assert(manager.addNamespaceItem("test", item1));
    assert(manager.addNamespaceItem("test", item2));
    
    // 测试项存在性
    auto items = manager.getNamespaceItems("test");
    bool foundBox = false, foundTheme = false;
    for (const auto& item : items) {
        if (item->name == "Box" && item->type == NamespaceItemType::CUSTOM_ELEMENT) {
            foundBox = true;
        }
        if (item->name == "Theme" && item->type == NamespaceItemType::CUSTOM_STYLE) {
            foundTheme = true;
        }
    }
    assert(foundBox);
    assert(foundTheme);
    
    // 测试获取项
    auto retrievedItem = manager.getNamespaceItem("test", "Box");
    assert(retrievedItem != nullptr);
    assert(retrievedItem->name == "Box");
    assert(retrievedItem->type == NamespaceItemType::CUSTOM_ELEMENT);
    
    std::cout << "命名空间项测试通过！" << std::endl;
}

void testNamespaceMerging() {
    std::cout << "=== 测试命名空间合并 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建源命名空间
    assert(manager.createNamespace("source", "source.chtl"));
    auto sourceItem1 = std::make_shared<NamespaceItem>("Box", NamespaceItemType::CUSTOM_ELEMENT, "source.chtl", 10, 5);
    auto sourceItem2 = std::make_shared<NamespaceItem>("Theme", NamespaceItemType::CUSTOM_STYLE, "source.chtl", 20, 5);
    manager.addNamespaceItem("source", sourceItem1);
    manager.addNamespaceItem("source", sourceItem2);
    
    // 创建目标命名空间
    assert(manager.createNamespace("target", "target.chtl"));
    auto targetItem1 = std::make_shared<NamespaceItem>("Button", NamespaceItemType::CUSTOM_ELEMENT, "target.chtl", 30, 5);
    manager.addNamespaceItem("target", targetItem1);
    
    // 合并命名空间
    bool merged = manager.mergeNamespaces("source");
    assert(merged);
    
    // 验证合并结果
    auto targetItems = manager.getNamespaceItems("target");
    assert(targetItems.size() == 3); // Box, Theme, Button
    
    bool foundBox = false, foundTheme = false, foundButton = false;
    for (const auto& item : targetItems) {
        if (item->name == "Box") foundBox = true;
        if (item->name == "Theme") foundTheme = true;
        if (item->name == "Button") foundButton = true;
    }
    assert(foundBox && foundTheme && foundButton);
    
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
    auto item1 = std::make_shared<NamespaceItem>("Box", NamespaceItemType::CUSTOM_ELEMENT, "space1.chtl", 10, 5);
    item1->content = "[Custom] @Element Box { }";
    
    auto item2 = std::make_shared<NamespaceItem>("Box", NamespaceItemType::CUSTOM_ELEMENT, "space2.chtl", 15, 5);
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
    
    // 测试默认命名空间是否启用
    bool defaultEnabled = manager.isDefaultNamespaceEnabled();
    std::cout << "默认命名空间启用状态: " << (defaultEnabled ? "启用" : "禁用") << std::endl;
    
    if (defaultEnabled) {
        // 获取默认命名空间
        auto defaultNs = manager.getDefaultNamespace();
        if (!defaultNs.empty()) {
            std::cout << "默认命名空间名称: " << defaultNs << std::endl;
        }
        
        // 设置新的默认命名空间
        assert(manager.createNamespace("newDefault", "new_default.chtl"));
        manager.setDefaultNamespace("newDefault");
        
        auto newDefaultNs = manager.getDefaultNamespace();
        assert(newDefaultNs == "newDefault");
        
        std::cout << "新默认命名空间设置成功: " << newDefaultNs << std::endl;
    }
    
    std::cout << "默认命名空间测试通过！" << std::endl;
}

void testNamespaceValidation() {
    std::cout << "=== 测试命名空间验证 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 测试有效命名空间
    assert(manager.createNamespace("valid", "valid.chtl"));
    auto validResult = manager.validateNamespace("valid");
    assert(validResult);
    
    auto validationErrors = manager.getValidationErrors("valid");
    if (validationErrors.empty()) {
        std::cout << "有效命名空间验证通过" << std::endl;
    } else {
        std::cout << "有效命名空间验证失败:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    // 测试无效命名空间
    auto invalidResult = manager.validateNamespace("invalid");
    assert(!invalidResult);
    
    auto invalidErrors = manager.getValidationErrors("invalid");
    if (!invalidErrors.empty()) {
        std::cout << "无效命名空间验证失败（预期）:" << std::endl;
        for (const auto& error : invalidErrors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    std::cout << "命名空间验证测试通过！" << std::endl;
}

void testNamespaceInheritance() {
    std::cout << "=== 测试命名空间继承 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建继承链
    assert(manager.createNamespace("Base", "base.chtl"));
    assert(manager.createNamespace("Derived", "derived.chtl"));
    
    // 设置继承关系
    manager.inheritNamespace("Derived", "Base");
    
    // 添加项到基命名空间
    auto baseItem = std::make_shared<NamespaceItem>("BaseClass", NamespaceItemType::CUSTOM_ELEMENT, "base.chtl", 10, 5);
    manager.addNamespaceItem("Base", baseItem);
    
    // 添加项到派生命名空间
    auto derivedItem = std::make_shared<NamespaceItem>("DerivedClass", NamespaceItemType::CUSTOM_ELEMENT, "derived.chtl", 20, 5);
    manager.addNamespaceItem("Derived", derivedItem);
    
    // 测试继承
    auto inheritanceChain = manager.getInheritanceChain("Derived");
    assert(!inheritanceChain.empty());
    
    std::cout << "Derived命名空间继承链: ";
    for (const auto& ns : inheritanceChain) {
        std::cout << ns << " ";
    }
    std::cout << std::endl;
    
    std::cout << "命名空间继承测试通过！" << std::endl;
}

void testNamespaceExportImport() {
    std::cout << "=== 测试命名空间导出导入 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建源命名空间
    assert(manager.createNamespace("Source", "source.chtl"));
    auto sourceItem = std::make_shared<NamespaceItem>("ExportedItem", NamespaceItemType::CUSTOM_ELEMENT, "source.chtl", 10, 5);
    manager.addNamespaceItem("Source", sourceItem);
    
    // 导出命名空间
    auto exportedData = manager.exportNamespace("Source");
    assert(!exportedData.empty());
    
    // 创建目标命名空间
    assert(manager.createNamespace("Target", "target.chtl"));
    
    // 导入命名空间
    bool imported = manager.importNamespace("Target", exportedData);
    assert(imported);
    
    // 验证导入结果
    auto targetItems = manager.getNamespaceItems("Target");
    assert(!targetItems.empty());
    
    std::cout << "命名空间导出导入测试通过！" << std::endl;
}

void testNamespaceStatistics() {
    std::cout << "=== 测试命名空间统计 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建一些命名空间以生成统计信息
    assert(manager.createNamespace("Stats1", "stats1.chtl"));
    assert(manager.createNamespace("Stats2", "stats2.chtl"));
    
    auto item1 = std::make_shared<NamespaceItem>("Item1", NamespaceItemType::CUSTOM_ELEMENT, "stats1.chtl", 10, 5);
    auto item2 = std::make_shared<NamespaceItem>("Item2", NamespaceItemType::CUSTOM_STYLE, "stats2.chtl", 20, 5);
    
    manager.addNamespaceItem("Stats1", item1);
    manager.addNamespaceItem("Stats2", item2);
    
    // 获取统计信息
    auto stats = manager.getStatistics();
    std::cout << "命名空间统计信息:" << std::endl;
    std::cout << "  总命名空间数: " << stats.totalNamespaces << std::endl;
    std::cout << "  总项数: " << stats.totalItems << std::endl;
    std::cout << "  冲突数: " << stats.totalConflicts << std::endl;
    
    // 清除统计信息
    manager.clearStatistics();
    
    auto clearedStats = manager.getStatistics();
    std::cout << "清除后的统计信息:" << std::endl;
    std::cout << "  总命名空间数: " << clearedStats.totalNamespaces << std::endl;
    std::cout << "  总项数: " << clearedStats.totalItems << std::endl;
    
    std::cout << std::endl;
}

void testNamespaceDebugInfo() {
    std::cout << "=== 测试命名空间调试信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    NamespaceManager manager(configManager);
    
    // 创建测试命名空间
    assert(manager.createNamespace("Debug", "debug.chtl"));
    auto debugItem = std::make_shared<NamespaceItem>("DebugItem", NamespaceItemType::CUSTOM_ELEMENT, "debug.chtl", 10, 5);
    manager.addNamespaceItem("Debug", debugItem);
    
    // 获取调试信息
    auto debugInfo = manager.getDebugInfo();
    std::cout << "调试信息:" << std::endl;
    std::cout << debugInfo << std::endl;
    
    // 获取命名空间信息
    auto nsInfo = manager.getNamespaceInfo("Debug");
    std::cout << "命名空间信息:" << std::endl;
    std::cout << nsInfo << std::endl;
    
    std::cout << std::endl;
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
        testNamespaceValidation();
        testNamespaceInheritance();
        testNamespaceExportImport();
        testNamespaceStatistics();
        testNamespaceDebugInfo();
        
        std::cout << "\n所有测试完成！" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}