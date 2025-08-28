#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "../CHTL/CHTLContext/ImportManager.h"
#include "../CHTL/CHTLContext/ConfigurationManager.h"

using namespace CHTL;

// 创建测试目录结构
void createTestDirectoryStructure() {
    std::filesystem::create_directories("test_imports");
    std::filesystem::create_directories("test_imports/module");
    std::filesystem::create_directories("test_imports/module/CMOD");
    std::filesystem::create_directories("test_imports/module/CJMOD");
    std::filesystem::create_directories("test_imports/Modules");
    std::filesystem::create_directories("test_imports/Modules/CMOD");
    std::filesystem::create_directories("test_imports/Modules/CJMOD");
    
    // 创建测试文件
    std::ofstream("test_imports/test.html") << "<html><body>Test HTML</body></html>";
    std::ofstream("test_imports/test.css") << "body { color: red; }";
    std::ofstream("test_imports/test.js") << "console.log('Test JS');";
    std::ofstream("test_imports/test.chtl") << "[Custom] @Element Test { div { } }";
    std::ofstream("test_imports/test.cmod") << "[Custom] @Element TestMod { span { } }";
    std::ofstream("test_imports/test.cjmod") << "vir testVir { }";
    
    // 创建模块文件
    std::ofstream("test_imports/module/test.chtl") << "[Custom] @Element ModuleTest { div { } }";
    std::ofstream("test_imports/module/CMOD/test.cmod") << "[Custom] @Element CMODTest { div { } }";
    std::ofstream("test_imports/module/CJMOD/test.cjmod") << "vir moduleVir { }";
    
    // 创建子模块文件
    std::filesystem::create_directories("test_imports/module/Chtholly");
    std::filesystem::create_directories("test_imports/module/Chtholly/Space");
    std::ofstream("test_imports/module/Chtholly/Space/test.chtl") << "[Custom] @Element SpaceTest { div { } }";
    std::ofstream("test_imports/module/Chtholly/Space/test.cmod") << "[Custom] @Element SpaceModTest { span { } }";
}

// 清理测试目录
void cleanupTestDirectory() {
    std::filesystem::remove_all("test_imports");
}

void testBasicImport() {
    std::cout << "=== 测试基本导入功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试基本导入语句解析
    std::string source = "[Import] @Html from test.html;\n[Import] @Style from test.css;\n[Import] @JavaScript from test.js;";
    auto statements = manager.parse(source);
    
    std::cout << "解析到 " << statements.size() << " 个导入语句" << std::endl;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto statement = statements[i];
        std::cout << "导入语句 " << (i + 1) << ":" << std::endl;
        std::cout << "  类型: " << static_cast<int>(statement->type) << std::endl;
        std::cout << "  路径: " << statement->path << std::endl;
        std::cout << "  别名: " << (statement->alias.empty() ? "(无)" : statement->alias) << std::endl;
        std::cout << "  通配符: " << (statement->isWildcard ? "是" : "否") << std::endl;
        std::cout << "  子模块: " << (statement->subModules.empty() ? "(无)" : statement->subModules[0]) << std::endl;
        
        // 执行导入
        auto result = manager.executeImport(statement);
        std::cout << "  导入结果: " << (result.success ? "成功" : "失败") << std::endl;
        if (result.success) {
            std::cout << "    文件路径: " << result.filePath << std::endl;
            std::cout << "    内容长度: " << result.content.length() << std::endl;
        } else {
            std::cout << "    错误: " << result.errorMessage << std::endl;
        }
        
        if (!result.warnings.empty()) {
            std::cout << "    警告: ";
            for (const auto& warning : result.warnings) {
                std::cout << warning << " ";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testWildcardImport() {
    std::cout << "=== 测试通配符导入功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试通配符导入
    std::string source = "[Import] @Chtl from *.chtl;\n[Import] @Chtl from *.cmod;\n[Import] @Chtl from *;";
    auto statements = manager.parse(source);
    
    std::cout << "解析到 " << statements.size() << " 个通配符导入语句" << std::endl;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto statement = statements[i];
        std::cout << "通配符导入语句 " << (i + 1) << ":" << std::endl;
        std::cout << "  模式: " << statement->wildcardPattern << std::endl;
        
        // 执行导入
        auto result = manager.executeImport(statement);
        std::cout << "  导入结果: " << (result.success ? "成功" : "失败") << std::endl;
        
        if (result.success) {
            std::cout << "    找到文件数量: " << result.importedFiles.size() << std::endl;
            for (const auto& file : result.importedFiles) {
                std::cout << "      " << file << std::endl;
            }
        } else {
            std::cout << "    错误: " << result.errorMessage << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testSubModuleImport() {
    std::cout << "=== 测试子模块导入功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试子模块导入
    std::string source = "[Import] @Chtl from Chtholly.Space;\n[Import] @Chtl from Chtholly.*;";
    auto statements = manager.parse(source);
    
    std::cout << "解析到 " << statements.size() << " 个子模块导入语句" << std::endl;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto statement = statements[i];
        std::cout << "子模块导入语句 " << (i + 1) << ":" << std::endl;
        std::cout << "  路径: " << statement->path << std::endl;
        std::cout << "  子模块数量: " << statement->subModules.size() << std::endl;
        for (const auto& subModule : statement->subModules) {
            std::cout << "    " << subModule << std::endl;
        }
        
        // 执行导入
        auto result = manager.executeImport(statement);
        std::cout << "  导入结果: " << (result.success ? "成功" : "失败") << std::endl;
        
        if (result.success) {
            std::cout << "    找到文件数量: " << result.importedFiles.size() << std::endl;
            for (const auto& file : result.importedFiles) {
                std::cout << "      " << file << std::endl;
            }
        } else {
            std::cout << "    错误: " << result.errorMessage << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testAliasImport() {
    std::cout << "=== 测试别名导入功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试别名导入
    std::string source = "[Import] @Html from test.html as mainPage;\n[Import] @Style from test.css as mainStyle;";
    auto statements = manager.parse(source);
    
    std::cout << "解析到 " << statements.size() << " 个别名导入语句" << std::endl;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto statement = statements[i];
        std::cout << "别名导入语句 " << (i + 1) << ":" << std::endl;
        std::cout << "  类型: " << static_cast<int>(statement->type) << std::endl;
        std::cout << "  路径: " << statement->path << std::endl;
        std::cout << "  别名: " << statement->alias << std::endl;
        
        // 执行导入
        auto result = manager.executeImport(statement);
        std::cout << "  导入结果: " << (result.success ? "成功" : "失败") << std::endl;
        
        if (result.success) {
            std::cout << "    文件路径: " << result.filePath << std::endl;
            std::cout << "    内容长度: " << result.content.length() << std::endl;
        } else {
            std::cout << "    错误: " << result.errorMessage << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testSearchPaths() {
    std::cout << "=== 测试搜索路径功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试不同导入类型的搜索路径
    std::vector<ImportType> types = {
        ImportType::HTML,
        ImportType::STYLE,
        ImportType::JAVASCRIPT,
        ImportType::CHTL,
        ImportType::CJMOD,
        ImportType::CONFIG
    };
    
    for (const auto& type : types) {
        auto searchPaths = manager.getSearchPaths(type);
        std::cout << "导入类型 " << static_cast<int>(type) << " 的搜索路径:" << std::endl;
        for (const auto& path : searchPaths) {
            std::cout << "  " << path << std::endl;
        }
        std::cout << std::endl;
    }
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 测试不存在的文件
    std::string source = "[Import] @Html from nonexistent.html;\n[Import] @Style from invalid.css;";
    auto statements = manager.parse(source);
    
    std::cout << "解析到 " << statements.size() << " 个导入语句" << std::endl;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        auto statement = statements[i];
        std::cout << "导入语句 " << (i + 1) << ":" << std::endl;
        std::cout << "  路径: " << statement->path << std::endl;
        
        // 执行导入
        auto result = manager.executeImport(statement);
        std::cout << "  导入结果: " << (result.success ? "成功" : "失败") << std::endl;
        
        if (!result.success) {
            std::cout << "    错误: " << result.errorMessage << std::endl;
        }
    }
    
    // 检查错误信息
    auto errors = manager.getErrors();
    if (!errors.empty()) {
        std::cout << "错误信息:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void testImportHistory() {
    std::cout << "=== 测试导入历史功能 ===" << std::endl;
    
    auto configManager = std::make_shared<ConfigurationManager>();
    ImportManager manager(configManager, "test_imports");
    
    // 执行一些导入操作
    std::string source = "[Import] @Html from test.html;\n[Import] @Style from test.css;";
    auto statements = manager.parse(source);
    
    for (const auto& statement : statements) {
        manager.executeImport(statement);
    }
    
    // 检查导入历史
    auto importHistory = manager.getImportHistory();
    std::cout << "导入历史:" << std::endl;
    for (const auto& pair : importHistory) {
        std::cout << "  文件: " << pair.first << std::endl;
        for (const auto& imported : pair.second) {
            std::cout << "    导入: " << imported << std::endl;
        }
    }
    
    std::cout << std::endl;
}

int main() {
    std::cout << "CHTL 增强导入管理器测试程序" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;
    
    try {
        // 创建测试目录结构
        createTestDirectoryStructure();
        
        testBasicImport();
        testWildcardImport();
        testSubModuleImport();
        testAliasImport();
        testSearchPaths();
        testErrorHandling();
        testImportHistory();
        
        std::cout << "所有测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    // 清理测试目录
    cleanupTestDirectory();
    
    return 0;
}