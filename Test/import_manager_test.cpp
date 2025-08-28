#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cassert>
#include "../CHTL/CHTLContext/ImportManager.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

void testBasicImport() {
    std::cout << "=== 测试基本导入功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试基本导入语句添加
    ImportInfo import1(ImportType::HTML, "test.html");
    ImportInfo import2(ImportType::STYLE, "test.css");
    ImportInfo import3(ImportType::JAVASCRIPT, "test.js");
    
    // 添加导入
    manager.addImport(import1);
    manager.addImport(import2);
    manager.addImport(import3);
    
    // 获取导入列表
    auto imports = manager.getImports();
    std::cout << "添加了 " << imports.size() << " 个导入" << std::endl;
    
    for (size_t i = 0; i < imports.size(); ++i) {
        const auto& import = imports[i];
        std::cout << "导入 " << (i + 1) << ":" << std::endl;
        std::cout << "  类型: " << static_cast<int>(import.type) << std::endl;
        std::cout << "  路径: " << import.path << std::endl;
        std::cout << "  别名: " << (import.alias.empty() ? "(无)" : import.alias) << std::endl;
        std::cout << "  通配符: " << (import.isWildcard ? "是" : "否") << std::endl;
        
        // 解析路径
        auto resolvedPath = manager.resolvePath(import.path, import.type);
        std::cout << "  解析路径: " << (resolvedPath.resolvedPaths.empty() ? "无" : resolvedPath.resolvedPaths[0]) << std::endl;
    }
    
    std::cout << std::endl;
}

void testWildcardImport() {
    std::cout << "=== 测试通配符导入 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试通配符导入
    ImportInfo wildcardImport(ImportType::HTML, "*.chtl");
    wildcardImport.isWildcard = true;
    
    manager.addImport(wildcardImport);
    
    // 展开通配符
    auto expandedPaths = manager.expandWildcard("*.chtl", ImportType::HTML);
    std::cout << "通配符展开结果:" << std::endl;
    for (const auto& path : expandedPaths) {
        std::cout << "  " << path << std::endl;
    }
    
    std::cout << std::endl;
}

void testSubModuleImport() {
    std::cout << "=== 测试子模块导入 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试子模块导入
    ImportInfo subModuleImport(ImportType::CHTL, "Chtholly.Space");
    
    manager.addImport(subModuleImport);
    
    // 解析子模块路径
    auto subModulePaths = manager.resolveSubModulePath("Chtholly.Space", ImportType::CHTL);
    std::cout << "子模块路径解析结果:" << std::endl;
    for (const auto& path : subModulePaths) {
        std::cout << "  " << path << std::endl;
    }
    
    std::cout << std::endl;
}

void testAliasImport() {
    std::cout << "=== 测试别名导入 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加别名
    manager.addAlias("UI", "ui.chtl");
    manager.addAlias("Core", "core.chtl");
    
    // 测试别名解析
    std::cout << "别名解析结果:" << std::endl;
    std::cout << "  UI -> " << manager.resolveAlias("UI") << std::endl;
    std::cout << "  Core -> " << manager.resolveAlias("Core") << std::endl;
    std::cout << "  Unknown -> " << manager.resolveAlias("Unknown") << std::endl;
    
    // 检查别名存在性
    std::cout << "别名存在性检查:" << std::endl;
    std::cout << "  UI存在: " << (manager.hasAlias("UI") ? "是" : "否") << std::endl;
    std::cout << "  Core存在: " << (manager.hasAlias("Core") ? "是" : "否") << std::endl;
    std::cout << "  Unknown存在: " << (manager.hasAlias("Unknown") ? "是" : "否") << std::endl;
    
    std::cout << std::endl;
}

void testCircularDependency() {
    std::cout << "=== 测试循环依赖检测 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加可能形成循环依赖的导入
    ImportInfo import1(ImportType::CHTL, "file1.chtl");
    ImportInfo import2(ImportType::CHTL, "file2.chtl");
    ImportInfo import3(ImportType::CHTL, "file3.chtl");
    
    manager.addImport(import1);
    manager.addImport(import2);
    manager.addImport(import3);
    
    // 检查循环依赖
    bool hasCircular = manager.checkCircularDependency("file1.chtl");
    std::cout << "file1.chtl 循环依赖检查: " << (hasCircular ? "发现循环依赖" : "无循环依赖") << std::endl;
    
    auto circularDeps = manager.getCircularDependencies("file1.chtl");
    if (!circularDeps.empty()) {
        std::cout << "发现的循环依赖:" << std::endl;
        for (const auto& dep : circularDeps) {
            std::cout << "  " << dep << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testDuplicateImport() {
    std::cout << "=== 测试重复导入检测 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加重复的导入
    ImportInfo import1(ImportType::HTML, "duplicate.html");
    ImportInfo import2(ImportType::HTML, "duplicate.html");
    ImportInfo import3(ImportType::HTML, "duplicate.html");
    
    manager.addImport(import1);
    manager.addImport(import2);
    manager.addImport(import3);
    
    // 检查重复导入
    auto duplicates = manager.getDuplicateImports();
    if (!duplicates.empty()) {
        std::cout << "发现的重复导入:" << std::endl;
        for (const auto& dup : duplicates) {
            std::cout << "  路径: " << dup.path << " (类型: " << static_cast<int>(dup.type) << ")" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testSearchPaths() {
    std::cout << "=== 测试搜索路径 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加搜索路径
    manager.addSearchPath("./modules");
    manager.addSearchPath("./libs");
    manager.addSearchPath("./components");
    
    // 获取搜索路径
    auto searchPaths = manager.getSearchPaths();
    std::cout << "当前搜索路径:" << std::endl;
    for (const auto& path : searchPaths) {
        std::cout << "  " << path << std::endl;
    }
    
    // 移除搜索路径
    manager.removeSearchPath("./libs");
    
    searchPaths = manager.getSearchPaths();
    std::cout << "移除 ./libs 后的搜索路径:" << std::endl;
    for (const auto& path : searchPaths) {
        std::cout << "  " << path << std::endl;
    }
    
    std::cout << std::endl;
}

void testFileValidation() {
    std::cout << "=== 测试文件验证 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试不同文件类型的验证
    std::cout << "文件类型验证:" << std::endl;
    std::cout << "  test.html -> " << (manager.isValidFileForType("test.html", ImportType::HTML) ? "有效" : "无效") << std::endl;
    std::cout << "  test.css -> " << (manager.isValidFileForType("test.css", ImportType::STYLE) ? "有效" : "无效") << std::endl;
    std::cout << "  test.js -> " << (manager.isValidFileForType("test.js", ImportType::JAVASCRIPT) ? "有效" : "无效") << std::endl;
    std::cout << "  test.chtl -> " << (manager.isValidFileForType("test.chtl", ImportType::CHTL) ? "有效" : "无效") << std::endl;
    
    // 获取有效扩展名
    auto htmlExts = manager.getValidExtensionsForType(ImportType::HTML);
    auto styleExts = manager.getValidExtensionsForType(ImportType::STYLE);
    auto jsExts = manager.getValidExtensionsForType(ImportType::JAVASCRIPT);
    auto chtlExts = manager.getValidExtensionsForType(ImportType::CHTL);
    
    std::cout << "有效扩展名:" << std::endl;
    std::cout << "  HTML: ";
    for (const auto& ext : htmlExts) {
        std::cout << ext << " ";
    }
    std::cout << std::endl;
    
    std::cout << "  Style: ";
    for (const auto& ext : styleExts) {
        std::cout << ext << " ";
    }
    std::cout << std::endl;
    
    std::cout << "  JavaScript: ";
    for (const auto& ext : jsExts) {
        std::cout << ext << " ";
    }
    std::cout << std::endl;
    
    std::cout << "  CHTL: ";
    for (const auto& ext : chtlExts) {
        std::cout << ext << " ";
    }
    std::cout << std::endl;
    
    std::cout << std::endl;
}

void testImportValidation() {
    std::cout << "=== 测试导入验证 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试有效导入
    ImportInfo validImport(ImportType::HTML, "valid.html");
    bool valid = manager.validateImport(validImport);
    std::cout << "有效导入验证: " << (valid ? "通过" : "失败") << std::endl;
    
    // 测试无效导入
    ImportInfo invalidImport(ImportType::HTML, "");
    bool invalid = manager.validateImport(invalidImport);
    std::cout << "无效导入验证: " << (invalid ? "通过" : "失败") << std::endl;
    
    // 获取验证错误
    auto errors = manager.getImportErrors(invalidImport);
    if (!errors.empty()) {
        std::cout << "验证错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加一些导入以生成统计信息
    ImportInfo import1(ImportType::HTML, "stats1.html");
    ImportInfo import2(ImportType::STYLE, "stats2.css");
    ImportInfo import3(ImportType::JAVASCRIPT, "stats3.js");
    
    manager.addImport(import1);
    manager.addImport(import2);
    manager.addImport(import3);
    
    // 获取统计信息
    auto stats = manager.getDebugInfo();
    std::cout << "统计信息:" << std::endl;
    std::cout << stats << std::endl;
    
    // 清除统计信息
    manager.clearStatistics();
    
    auto clearedStats = manager.getDebugInfo();
    std::cout << "清除后的统计信息:" << std::endl;
    std::cout << clearedStats << std::endl;
    
    std::cout << std::endl;
}

void testImportInfo() {
    std::cout << "=== 测试导入信息 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加导入
    ImportInfo import1(ImportType::CHTL, "info1.chtl");
    import1.alias = "Info1";
    import1.isWildcard = false;
    
    ImportInfo import2(ImportType::CHTL, "info2.chtl");
    import2.alias = "Info2";
    import2.isWildcard = true;
    
    manager.addImport(import1);
    manager.addImport(import2);
    
    // 获取导入信息
    auto info = manager.getImportInfo();
    std::cout << "导入信息:" << std::endl;
    std::cout << info << std::endl;
    
    std::cout << std::endl;
}

void testReset() {
    std::cout << "=== 测试重置功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 添加一些导入
    ImportInfo import1(ImportType::HTML, "reset1.html");
    ImportInfo import2(ImportType::STYLE, "reset2.css");
    
    manager.addImport(import1);
    manager.addImport(import2);
    
    std::cout << "重置前导入数量: " << manager.getImports().size() << std::endl;
    
    // 重置
    manager.reset();
    
    std::cout << "重置后导入数量: " << manager.getImports().size() << std::endl;
    
    std::cout << std::endl;
}

int main() {
    std::cout << "CHTL 导入管理器测试程序" << std::endl;
    std::cout << "========================" << std::endl << std::endl;
    
    try {
        testBasicImport();
        testWildcardImport();
        testSubModuleImport();
        testAliasImport();
        testCircularDependency();
        testDuplicateImport();
        testSearchPaths();
        testFileValidation();
        testImportValidation();
        testStatistics();
        testImportInfo();
        testReset();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}