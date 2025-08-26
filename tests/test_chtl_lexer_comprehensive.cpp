#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <string>

void testCHTLSyntax(const std::string& name, const std::string& source) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码: " << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    for (const auto& token : tokens) {
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN) {
            std::cout << "  " << token.getTypeName() << ": \"" << token.value << "\"" << std::endl;
        }
    }
    
    if (lexer.hasErrors()) {
        std::cout << "❌ 词法分析错误:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "✅ 词法分析通过" << std::endl;
    }
}

int main() {
    std::cout << "=== CHTL词法分析器全面语法测试 ===" << std::endl;
    
    // 1. 基础HTML元素
    testCHTLSyntax("基础HTML元素", 
        "<div class=\"container\" id=\"main\">Hello World</div>");
    
    // 2. 自闭合元素
    testCHTLSyntax("自闭合元素", 
        "<img src=\"image.jpg\" alt=\"Test\" />");
    
    // 3. Use声明
    testCHTLSyntax("Use声明", 
        "use standard");
    
    // 4. 命名空间
    testCHTLSyntax("命名空间", 
        "namespace ui { }");
    
    // 5. 导入语句
    testCHTLSyntax("导入语句", 
        "import { Button } from \"ui.chtl\"");
    
    // 6. 配置定义
    testCHTLSyntax("配置定义", 
        "configuration { name: \"app\" }");
    
    // 7. 模板定义
    testCHTLSyntax("模板定义", 
        "template Button { <button>Click</button> }");
    
    // 8. 自定义组件
    testCHTLSyntax("自定义组件", 
        "custom Dialog { <div class=\"dialog\"></div> }");
    
    // 9. 模板继承
    testCHTLSyntax("模板继承", 
        "template PrimaryButton : BaseButton { }");
    
    // 10. 变量组
    testCHTLSyntax("变量组", 
        "@var { primary-color: #007bff; font-size: 16px; }");
    
    // 11. 样式组
    testCHTLSyntax("样式组", 
        "@style { .btn { padding: 10px; border: none; } }");
    
    // 12. 选择器自动化
    testCHTLSyntax("类选择器自动化", 
        ".container { <div>Auto class</div> }");
    
    testCHTLSyntax("ID选择器自动化", 
        "#main { <div>Auto ID</div> }");
    
    // 13. 本地样式
    testCHTLSyntax("本地样式", 
        "<div> @style { color: red; } Content </div>");
    
    // 14. 本地脚本
    testCHTLSyntax("本地脚本", 
        "<button> @script { console.log('clicked'); } Click </button>");
    
    // 15. 注释
    testCHTLSyntax("行注释", 
        "// This is a comment\n<div>Content</div>");
    
    testCHTLSyntax("块注释", 
        "/* Block comment */ <div>Content</div>");
    
    // 16. 变量插值
    testCHTLSyntax("变量插值", 
        "@var { name: \"World\" } <div>Hello ${name}!</div>");
    
    // 17. 复杂嵌套
    testCHTLSyntax("复杂嵌套结构", 
        "namespace app {\n"
        "  @var { color: blue }\n"
        "  template Card {\n"
        "    <div class=\"card\">\n"
        "      <h3>Title</h3>\n"
        "    </div>\n"
        "  }\n"
        "}");
    
    // 18. Origin定义
    testCHTLSyntax("Origin定义", 
        "origin raw-html { <div>Raw content</div> }");
    
    // 19. 多个声明
    testCHTLSyntax("多个声明", 
        "use standard\nimport Button from \"ui\"\nnamespace app { }");
    
    // 20. 特殊字符和符号
    testCHTLSyntax("特殊字符", 
        "{ } ( ) [ ] < > ; : = , . / * & #");
    
    std::cout << "\n=== 全部测试完成 ===" << std::endl;
    return 0;
}