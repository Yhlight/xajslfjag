#include "CHTLCompiler/CHTLCompiler.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// CHTL语法全面测试
class CHTLSyntaxTester {
private:
    CHTL::CHTLCompiler compiler;
    int testCount = 0;
    int passedTests = 0;
    
    void runTest(const std::string& testName, const std::string& chtlCode, bool shouldSucceed = true) {
        testCount++;
        std::cout << "测试 " << testCount << ": " << testName << std::endl;
        std::cout << "代码: " << chtlCode << std::endl;
        
        auto result = compiler.compile(chtlCode, "test_" + std::to_string(testCount) + ".chtl");
        
        if (shouldSucceed) {
            if (result.success) {
                std::cout << "✓ 通过" << std::endl;
                passedTests++;
                if (!result.html.empty()) {
                    std::cout << "生成的HTML: " << result.html.substr(0, 200) << "..." << std::endl;
                }
            } else {
                std::cout << "✗ 失败" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "  错误: " << error << std::endl;
                }
            }
        } else {
            if (!result.success) {
                std::cout << "✓ 预期失败 - 通过" << std::endl;
                passedTests++;
            } else {
                std::cout << "✗ 应该失败但成功了" << std::endl;
            }
        }
        std::cout << std::endl;
    }
    
public:
    void runAllTests() {
        std::cout << "=== CHTL语法全面测试 ===" << std::endl << std::endl;
        
        // 1. 基础HTML元素测试
        testBasicElements();
        
        // 2. Use声明测试
        testUseDeclarations();
        
        // 3. 命名空间测试
        testNamespaces();
        
        // 4. 导入系统测试
        testImports();
        
        // 5. 配置定义测试
        testConfigurations();
        
        // 6. 模板系统测试
        testTemplates();
        
        // 7. 自定义组件测试
        testCustomComponents();
        
        // 8. 原始嵌入测试
        testRawEmbedding();
        
        // 9. 变量组测试
        testVariableGroups();
        
        // 10. 样式组测试
        testStyleGroups();
        
        // 11. 本地样式测试
        testLocalStyles();
        
        // 12. 本地脚本测试
        testLocalScripts();
        
        // 13. 选择器自动化测试
        testSelectorAutomation();
        
        // 14. 注释测试
        testComments();
        
        // 15. 复杂嵌套测试
        testComplexNesting();
        
        // 16. 错误处理测试
        testErrorCases();
        
        // 报告结果
        std::cout << "=== 测试结果 ===" << std::endl;
        std::cout << "总测试数: " << testCount << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "失败测试: " << (testCount - passedTests) << std::endl;
        std::cout << "通过率: " << (passedTests * 100.0 / testCount) << "%" << std::endl;
    }
    
private:
    void testBasicElements() {
        std::cout << "--- 基础HTML元素测试 ---" << std::endl;
        
        runTest("简单div元素", 
                "<div>Hello World</div>");
        
        runTest("带属性的元素",
                "<div class=\"container\" id=\"main\">Content</div>");
        
        runTest("自闭合元素",
                "<img src=\"image.jpg\" alt=\"Test\" />");
        
        runTest("嵌套元素",
                "<div><p>Paragraph <span>inside</span> div</p></div>");
        
        runTest("多个根元素",
                "<div>First</div><div>Second</div>");
    }
    
    void testUseDeclarations() {
        std::cout << "--- Use声明测试 ---" << std::endl;
        
        runTest("基础use声明",
                "use standard\n<div>Content</div>");
        
        runTest("多个use声明",
                "use standard\nuse bootstrap\n<div>Content</div>");
        
        runTest("带版本的use声明",
                "use standard@1.0\n<div>Content</div>");
    }
    
    void testNamespaces() {
        std::cout << "--- 命名空间测试 ---" << std::endl;
        
        runTest("命名空间声明",
                "namespace ui {\n  <div>UI Content</div>\n}");
        
        runTest("嵌套命名空间",
                "namespace ui {\n  namespace components {\n    <div>Component</div>\n  }\n}");
        
        runTest("命名空间使用",
                "namespace ui {\n  <div class=\"ui-element\">Content</div>\n}");
    }
    
    void testImports() {
        std::cout << "--- 导入系统测试 ---" << std::endl;
        
        runTest("基础导入",
                "import \"components.chtl\"\n<div>Content</div>");
        
        runTest("选择性导入",
                "import { Button, Input } from \"ui.chtl\"\n<div>Content</div>");
        
        runTest("别名导入",
                "import Button as Btn from \"ui.chtl\"\n<div>Content</div>");
    }
    
    void testConfigurations() {
        std::cout << "--- 配置定义测试 ---" << std::endl;
        
        runTest("基础配置组",
                "configuration {\n  name: \"app\"\n  version: \"1.0\"\n}\n<div>Content</div>");
        
        runTest("名称组配置",
                "configuration name {\n  title: \"My App\"\n  description: \"Test App\"\n}\n<div>Content</div>");
        
        runTest("Origin类型组配置",
                "configuration origin-type {\n  type: \"web\"\n  target: \"modern\"\n}\n<div>Content</div>");
    }
    
    void testTemplates() {
        std::cout << "--- 模板系统测试 ---" << std::endl;
        
        runTest("基础模板定义",
                "template Button {\n  <button class=\"btn\">Click me</button>\n}\n<Button />");
        
        runTest("带参数的模板",
                "template Card(title, content) {\n  <div class=\"card\">\n    <h3>${title}</h3>\n    <p>${content}</p>\n  </div>\n}\n<Card title=\"Test\" content=\"Hello\" />");
        
        runTest("模板继承",
                "template BaseButton {\n  <button class=\"base-btn\">Base</button>\n}\ntemplate PrimaryButton : BaseButton {\n  <button class=\"primary-btn\">Primary</button>\n}\n<PrimaryButton />");
        
        runTest("模板特化",
                "template Button {\n  <button>Default</button>\n}\ntemplate Button<primary> {\n  <button class=\"primary\">Primary</button>\n}\n<Button<primary> />");
    }
    
    void testCustomComponents() {
        std::cout << "--- 自定义组件测试 ---" << std::endl;
        
        runTest("基础自定义组件",
                "custom MyComponent {\n  <div class=\"my-component\">Custom Content</div>\n}\n<MyComponent />");
        
        runTest("带属性的自定义组件",
                "custom Dialog(title, modal) {\n  <div class=\"dialog\" data-modal=\"${modal}\">\n    <h2>${title}</h2>\n  </div>\n}\n<Dialog title=\"Test\" modal=\"true\" />");
        
        runTest("自定义组件继承",
                "custom BaseCard {\n  <div class=\"card\">Base</div>\n}\ncustom InfoCard : BaseCard {\n  <div class=\"info-card\">Info</div>\n}\n<InfoCard />");
    }
    
    void testRawEmbedding() {
        std::cout << "--- 原始嵌入测试 ---" << std::endl;
        
        runTest("原始HTML嵌入",
                "origin raw-html {\n  <div>Raw HTML content</div>\n}\n<div>Before</div>\n<raw-html />\n<div>After</div>");
        
        runTest("原始CSS嵌入",
                "origin raw-css {\n  .custom { color: red; }\n}\n<div class=\"custom\">Styled</div>");
        
        runTest("原始JavaScript嵌入",
                "origin raw-js {\n  console.log('Hello from raw JS');\n}\n<div>Content</div>");
    }
    
    void testVariableGroups() {
        std::cout << "--- 变量组测试 ---" << std::endl;
        
        runTest("基础变量组",
                "@var {\n  primary-color: #007bff\n  font-size: 16px\n}\n<div style=\"color: ${primary-color};\">Styled text</div>");
        
        runTest("嵌套变量组",
                "@var theme {\n  @var colors {\n    primary: #007bff\n    secondary: #6c757d\n  }\n}\n<div style=\"color: ${theme.colors.primary};\">Text</div>");
        
        runTest("变量插值",
                "@var {\n  name: \"World\"\n}\n<div>Hello ${name}!</div>");
    }
    
    void testStyleGroups() {
        std::cout << "--- 样式组测试 ---" << std::endl;
        
        runTest("基础样式组",
                "@style {\n  .btn {\n    padding: 10px;\n    border: none;\n  }\n}\n<button class=\"btn\">Button</button>");
        
        runTest("样式特化",
                "@style<primary> {\n  .btn {\n    background: blue;\n    color: white;\n  }\n}\n<button class=\"btn\">Primary Button</button>");
        
        runTest("嵌套样式",
                "@style {\n  .card {\n    border: 1px solid #ccc;\n    .header {\n      background: #f8f9fa;\n    }\n  }\n}\n<div class=\"card\"><div class=\"header\">Header</div></div>");
    }
    
    void testLocalStyles() {
        std::cout << "--- 本地样式测试 ---" << std::endl;
        
        runTest("元素本地样式",
                "<div>\n  @style {\n    color: red;\n    font-weight: bold;\n  }\n  Styled content\n</div>");
        
        runTest("本地样式选择器",
                "<div>\n  @style {\n    & {\n      background: yellow;\n    }\n    .child {\n      margin: 10px;\n    }\n  }\n  <div class=\"child\">Child</div>\n</div>");
    }
    
    void testLocalScripts() {
        std::cout << "--- 本地脚本测试 ---" << std::endl;
        
        runTest("元素本地脚本",
                "<button>\n  @script {\n    this.addEventListener('click', function() {\n      alert('Clicked!');\n    });\n  }\n  Click me\n</button>");
        
        runTest("本地脚本变量",
                "<div>\n  @script {\n    const message = 'Hello from script';\n    console.log(message);\n  }\n  Content\n</div>");
    }
    
    void testSelectorAutomation() {
        std::cout << "--- 选择器自动化测试 ---" << std::endl;
        
        runTest("类选择器自动化",
                ".container {\n  <div>Auto-classed content</div>\n}");
        
        runTest("ID选择器自动化",
                "#main {\n  <div>Auto-ID content</div>\n}");
        
        runTest("复合选择器",
                ".card.featured {\n  <div>Featured card</div>\n}");
        
        runTest("嵌套选择器自动化",
                ".nav {\n  .item {\n    <a href=\"#\">Nav item</a>\n  }\n}");
    }
    
    void testComments() {
        std::cout << "--- 注释测试 ---" << std::endl;
        
        runTest("行注释",
                "// This is a line comment\n<div>Content</div>");
        
        runTest("块注释",
                "/* This is a\n   block comment */\n<div>Content</div>");
        
        runTest("生成器注释",
                "/*! Generator comment */\n<div>Content</div>");
        
        runTest("混合注释",
                "// Line comment\n/* Block comment */\n<div>Content</div>\n// Another line comment");
    }
    
    void testComplexNesting() {
        std::cout << "--- 复杂嵌套测试 ---" << std::endl;
        
        runTest("复杂嵌套结构",
                "namespace app {\n"
                "  @var {\n"
                "    primary: #007bff\n"
                "  }\n"
                "  @style {\n"
                "    .container {\n"
                "      max-width: 1200px;\n"
                "      margin: 0 auto;\n"
                "    }\n"
                "  }\n"
                "  template Layout(title) {\n"
                "    <div class=\"container\">\n"
                "      <h1>${title}</h1>\n"
                "      <main>\n"
                "        <content />\n"
                "      </main>\n"
                "    </div>\n"
                "  }\n"
                "  <Layout title=\"My App\">\n"
                "    <div>Main content here</div>\n"
                "  </Layout>\n"
                "}");
        
        runTest("多层模板继承",
                "template Base {\n"
                "  <div class=\"base\">Base</div>\n"
                "}\n"
                "template Intermediate : Base {\n"
                "  <div class=\"intermediate\">Intermediate</div>\n"
                "}\n"
                "template Final : Intermediate {\n"
                "  <div class=\"final\">Final</div>\n"
                "}\n"
                "<Final />");
    }
    
    void testErrorCases() {
        std::cout << "--- 错误处理测试 ---" << std::endl;
        
        runTest("未闭合标签", 
                "<div>Unclosed", false);
        
        runTest("无效属性语法",
                "<div class=>Invalid</div>", false);
        
        runTest("未定义模板使用",
                "<UndefinedTemplate />", false);
        
        runTest("语法错误",
                "template { invalid syntax }", false);
        
        runTest("无效选择器",
                ".invalid..selector { <div>Content</div> }", false);
    }
};

int main() {
    CHTLSyntaxTester tester;
    tester.runAllTests();
    return 0;
}