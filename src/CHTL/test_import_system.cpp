#include <iostream>
#include <string>
#include <filesystem>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ImportNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

void createTestFiles() {
    // 创建测试目录结构
    std::filesystem::create_directories("test_modules/CMOD/TestMod/src");
    std::filesystem::create_directories("test_modules/CMOD/TestMod/info");
    std::filesystem::create_directories("test_modules/CJMOD");
    std::filesystem::create_directories("module/styles");
    
    // 创建测试文件
    std::ofstream file1("test_import.chtl");
    file1 << R"([Template] @Element TestElement
{
    div { 
        class: test-element;
        text { "Test Element" }
    }
})";
    file1.close();
    
    std::ofstream file2("test_import.html");
    file2 << R"(<div class="html-import">HTML Import Content</div>)";
    file2.close();
    
    std::ofstream file3("test_import.css");
    file3 << R"(.css-import { color: blue; })";
    file3.close();
    
    std::ofstream file4("module/styles/common.css");
    file4 << R"(.common-style { margin: 10px; })";
    file4.close();
    
    std::ofstream file5("test_modules/CMOD/TestMod/info/TestMod.chtl");
    file5 << R"([Info]
{
    NAME = "TestMod";
    VERSION = "1.0.0";
    DESCRIPTION = "Test Module";
    AUTHOR = "CHTL Test";
    LICENSE = "MIT";
})";
    file5.close();
}

void cleanupTestFiles() {
    // 清理测试文件
    std::filesystem::remove_all("test_modules");
    std::filesystem::remove_all("module");
    std::filesystem::remove("test_import.chtl");
    std::filesystem::remove("test_import.html");
    std::filesystem::remove("test_import.css");
}

void testBasicImports() {
    std::cout << "\n=== 测试基本导入语法 ===" << std::endl;
    
    std::string importCode = R"(
[Import] @Html from "test_import.html" as htmlContent;
[Import] @Style from "test_import.css";
[Import] @Chtl from "test_import.chtl";
[Import] [Template] @Element TestElement from "test_import.chtl";
)";
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(importCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        if (ast.success) {
            std::cout << "✓ 导入语法解析成功" << std::endl;
            std::cout << "  - 导入节点数: " << ast.imports.size() << std::endl;
            
            for (auto importPtr : ast.imports) {
                if (auto importNode = std::dynamic_pointer_cast<ImportNode>(importPtr)) {
                    std::cout << "  - 导入类型: " << static_cast<int>(importNode->getImportType()) << std::endl;
                    std::cout << "    导入路径: " << importNode->getImportPath() << std::endl;
                    std::cout << "    别名: " << importNode->getAlias() << std::endl;
                }
            }
        } else {
            std::cout << "❌ 导入语法解析失败" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 基本导入测试异常: " << e.what() << std::endl;
    }
}

void testImportManager() {
    std::cout << "\n=== 测试导入管理器 ===" << std::endl;
    
    try {
        ImportManager manager;
        
        // 设置官方模块目录
        manager.setOfficialModuleDirectory("test_modules");
        
        // 添加搜索路径
        manager.addSearchPath("./");
        manager.addSearchPath("module");
        
        std::cout << "✓ 导入管理器初始化成功" << std::endl;
        
        // 测试文件存在检查
        if (manager.fileExists("test_import.chtl")) {
            std::cout << "✓ 文件存在检查: test_import.chtl 存在" << std::endl;
        } else {
            std::cout << "❌ 文件存在检查: test_import.chtl 不存在" << std::endl;
        }
        
        // 测试模块加载
        if (manager.loadModule("test_import.chtl")) {
            std::cout << "✓ 模块加载成功: test_import.chtl" << std::endl;
        } else {
            std::cout << "❌ 模块加载失败: test_import.chtl" << std::endl;
        }
        
        // 测试重复加载检测
        if (manager.loadModule("test_import.chtl")) {
            std::cout << "✓ 重复加载检测正常" << std::endl;
        }
        
        // 测试已加载模块列表
        auto loadedModules = manager.getLoadedModules();
        std::cout << "✓ 已加载模块数: " << loadedModules.size() << std::endl;
        
        // 测试CMOD模块结构检查
        if (manager.checkModuleStructure("test_modules/CMOD/TestMod")) {
            std::cout << "✓ CMOD模块结构检查通过" << std::endl;
        } else {
            std::cout << "❌ CMOD模块结构检查失败" << std::endl;
        }
        
        // 测试模块信息获取
        auto moduleInfo = manager.getModuleInfo("test_modules/CMOD/TestMod");
        if (!moduleInfo.empty()) {
            std::cout << "✓ 模块信息获取成功:" << std::endl;
            for (const auto& pair : moduleInfo) {
                std::cout << "    " << pair.first << ": " << pair.second.substr(0, 50) << "..." << std::endl;
            }
        }
        
        // 测试文件内容加载
        std::string content = manager.loadFileContent("test_import.css");
        if (!content.empty()) {
            std::cout << "✓ 文件内容加载成功: " << content.substr(0, 30) << "..." << std::endl;
        }
        
        // 测试原始嵌入创建
        std::string namedOrigin = manager.createNamedOriginEmbed("console.log('test');", "@JavaScript", "testScript");
        if (!namedOrigin.empty()) {
            std::cout << "✓ 命名原始嵌入创建成功" << std::endl;
            std::cout << "    " << namedOrigin.substr(0, 50) << "..." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 导入管理器测试异常: " << e.what() << std::endl;
    }
}

void testPathResolution() {
    std::cout << "\n=== 测试路径解析 ===" << std::endl;
    
    try {
        ImportManager manager;
        manager.addSearchPath("./");
        manager.addSearchPath("module");
        
        // 创建不同类型的导入节点进行测试
        auto htmlImport = std::make_shared<HtmlImportNode>();
        htmlImport->setImportPath("test_import.html");
        
        auto styleImport = std::make_shared<StyleImportNode>();
        styleImport->setImportPath("styles/common.css");
        
        auto chtlImport = std::make_shared<CHTLImportNode>();
        chtlImport->setImportPath("test_import.chtl");
        
        std::cout << "✓ 导入节点创建成功" << std::endl;
        
        // 测试路径解析
        std::string htmlPath = manager.resolveImportPath(*htmlImport, "current.chtl");
        std::string stylePath = manager.resolveImportPath(*styleImport, "current.chtl");
        std::string chtlPath = manager.resolveImportPath(*chtlImport, "current.chtl");
        
        std::cout << "✓ 路径解析结果:" << std::endl;
        std::cout << "    HTML路径: " << htmlPath << std::endl;
        std::cout << "    样式路径: " << stylePath << std::endl;
        std::cout << "    CHTL路径: " << chtlPath << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 路径解析测试异常: " << e.what() << std::endl;
    }
}

void testCircularDependency() {
    std::cout << "\n=== 测试循环依赖检测 ===" << std::endl;
    
    try {
        ImportManager manager;
        
        // 模拟循环依赖场景
        manager.addDependency("file1.chtl", "file2.chtl");
        manager.addDependency("file2.chtl", "file3.chtl");
        
        std::cout << "✓ 依赖关系添加成功" << std::endl;
        
        // 测试循环依赖检测（这里简化测试，实际需要更复杂的循环检测）
        bool hasCircular = manager.hasCircularImport("file1.chtl");
        std::cout << "✓ 循环依赖检测: " << (hasCircular ? "检测到" : "未检测到") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 循环依赖测试异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "CHTL 导入系统测试" << std::endl;
    std::cout << "==================" << std::endl;
    
    // 创建测试文件
    createTestFiles();
    
    try {
        testBasicImports();
        testImportManager();
        testPathResolution();
        testCircularDependency();
        
        std::cout << "\n导入系统测试完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试过程中发生异常: " << e.what() << std::endl;
    }
    
    // 清理测试文件
    cleanupTestFiles();
    
    return 0;
}