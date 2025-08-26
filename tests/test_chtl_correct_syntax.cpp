#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <string>

void testCHTLSyntax(const std::string& name, const std::string& source, bool expectSuccess = true) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码: " << source << std::endl;
    
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Token数量: " << tokens.size() << std::endl;
    for (const auto& token : tokens) {
        if (token.type != CHTL::CHTLTokenType::EOF_TOKEN && 
            token.type != CHTL::CHTLTokenType::WHITESPACE && 
            token.type != CHTL::CHTLTokenType::NEWLINE) {
            std::cout << "  " << token.getTypeName() << ": \"" << token.value << "\"" << std::endl;
        }
    }
    
    bool hasErrors = lexer.hasErrors();
    if (expectSuccess && !hasErrors) {
        std::cout << "✅ 语法识别正确" << std::endl;
    } else if (!expectSuccess && hasErrors) {
        std::cout << "✅ 预期错误，识别正确" << std::endl;
    } else if (hasErrors) {
        std::cout << "❌ 语法识别错误:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cout << "  " << error << std::endl;
        }
    } else {
        std::cout << "❌ 应该有错误但没有检测到" << std::endl;
    }
}

int main() {
    std::cout << "=== CHTL真实语法全面测试 ===" << std::endl;
    
    // 1. 基础CHTL元素语法
    testCHTLSyntax("基础HTML元素", 
        "div { text { Hello World } }");
    
    testCHTLSyntax("元素属性", 
        "div { class: container; id: main; text { Content } }");
    
    testCHTLSyntax("嵌套元素", 
        "html { body { div { span { text { Nested } } } } }");
    
    // 2. 方括号关键字
    testCHTLSyntax("Template关键字", 
        "[Template] @Style ButtonStyle { color: blue; }");
    
    testCHTLSyntax("Custom关键字", 
        "[Custom] @Element MyComponent { div { text { Custom } } }");
    
    testCHTLSyntax("Origin关键字", 
        "[Origin] @Html RawContent { <div>Raw HTML</div> }");
    
    testCHTLSyntax("Namespace关键字", 
        "[Namespace] app { div { text { Content } } }");
    
    testCHTLSyntax("Configuration关键字", 
        "[Configuration] { DEBUG_MODE = false; }");
    
    // 3. 类型标识符
    testCHTLSyntax("@Style类型", 
        "@Style MyStyle;");
    
    testCHTLSyntax("@Element类型", 
        "@Element MyElement;");
    
    testCHTLSyntax("@Var类型", 
        "@Var MyVar;");
    
    testCHTLSyntax("@Html类型", 
        "@Html;");
    
    testCHTLSyntax("@JavaScript类型", 
        "@JavaScript;");
    
    // 4. 局部样式和脚本
    testCHTLSyntax("局部样式块", 
        "div { style { color: red; background: blue; } text { Styled } }");
    
    testCHTLSyntax("局部脚本块", 
        "button { script { console.log('clicked'); } text { Click } }");
    
    testCHTLSyntax("样式中的选择器", 
        "div { style { .btn { color: blue; } &:hover { color: red; } } }");
    
    // 5. 选择器自动化
    testCHTLSyntax("类选择器自动化", 
        ".container { div { text { Auto class } } }");
    
    testCHTLSyntax("ID选择器自动化", 
        "#main { div { text { Auto ID } } }");
    
    testCHTLSyntax("复合选择器", 
        ".btn.primary { text { Primary button } }");
    
    // 6. 文本节点
    testCHTLSyntax("基础文本节点", 
        "text { This is text content }");
    
    testCHTLSyntax("元素内文本", 
        "div { text { Hello } span { text { World } } }");
    
    // 7. use声明
    testCHTLSyntax("use html5", 
        "use html5;");
    
    testCHTLSyntax("use配置组", 
        "use @Config Standard;");
    
    // 8. 注释
    testCHTLSyntax("行注释", 
        "// This is a comment\ndiv { text { Content } }");
    
    testCHTLSyntax("块注释", 
        "/* This is a block comment */ div { text { Content } }");
    
    testCHTLSyntax("生成器注释", 
        "-- Generator comment\ndiv { text { Content } }");
    
    // 9. 变量和插值
    testCHTLSyntax("变量组定义", 
        "[Template] @Var Colors { primary: blue; secondary: red; }");
    
    testCHTLSyntax("变量使用", 
        "div { style { color: Colors(primary); } }");
    
    // 10. 继承和修改关键字
    testCHTLSyntax("inherit关键字", 
        "inherit @Style BaseStyle;");
    
    testCHTLSyntax("delete关键字", 
        "delete color, background;");
    
    testCHTLSyntax("insert关键字", 
        "insert after div { span { text { Inserted } } }");
    
    testCHTLSyntax("多词关键字", 
        "insert at top { div { text { Top content } } }");
    
    // 11. 导入关键字
    testCHTLSyntax("from关键字", 
        "from ui.chtl");
    
    testCHTLSyntax("as关键字", 
        "as Button");
    
    testCHTLSyntax("except关键字", 
        "except span, div;");
    
    // 12. CE对等式
    testCHTLSyntax("冒号和等号等价", 
        "div { class: container; id = main; }");
    
    // 13. 复杂嵌套结构
    testCHTLSyntax("复杂模板定义", 
        "[Template] @Element Card {\n"
        "  div {\n"
        "    class: card;\n"
        "    style {\n"
        "      border: 1px solid #ccc;\n"
        "      .header { font-weight: bold; }\n"
        "    }\n"
        "    div {\n"
        "      class: header;\n"
        "      text { Card Title }\n"
        "    }\n"
        "    div {\n"
        "      text { Card Content }\n"
        "    }\n"
        "  }\n"
        "}");
    
    testCHTLSyntax("命名空间嵌套", 
        "[Namespace] app {\n"
        "  [Template] @Var theme { color: blue; }\n"
        "  div {\n"
        "    style { color: theme(color); }\n"
        "    text { Namespaced content }\n"
        "  }\n"
        "}");
    
    // 14. CHTL JS语法（基础）
    testCHTLSyntax("增强选择器", 
        "{{.button}}");
    
    testCHTLSyntax("箭头操作符", 
        "element->click();");
    
    testCHTLSyntax("vir关键字", 
        "vir test = listen({ click: function() {} });");
    
    // 15. 字符串和字面量
    testCHTLSyntax("双引号字符串", 
        "text { \"Hello World\" }");
    
    testCHTLSyntax("单引号字符串", 
        "text { 'Hello World' }");
    
    testCHTLSyntax("无引号字面量", 
        "style { color: red; font-size: 16px; }");
    
    // 16. 数字
    testCHTLSyntax("整数", 
        "style { width: 100; }");
    
    testCHTLSyntax("小数", 
        "style { opacity: 0.5; }");
    
    // 17. 错误语法测试
    testCHTLSyntax("未闭合方括号", 
        "[Template @Style", false);
    
    testCHTLSyntax("无效类型标识符", 
        "@InvalidType", true); // 应该被识别为IDENTIFIER
    
    testCHTLSyntax("未终止字符串", 
        "text { \"unterminated string", false);
    
    // 18. 完整示例
    testCHTLSyntax("完整CHTL示例", 
        "use html5;\n"
        "[Configuration] { DEBUG_MODE = false; }\n"
        "[Namespace] app {\n"
        "  [Template] @Var colors { primary: blue; }\n"
        "  [Template] @Style button {\n"
        "    padding: 10px;\n"
        "    color: colors(primary);\n"
        "  }\n"
        "  html {\n"
        "    body {\n"
        "      div {\n"
        "        class: container;\n"
        "        style {\n"
        "          max-width: 1200px;\n"
        "          .title { font-size: 2rem; }\n"
        "        }\n"
        "        h1 {\n"
        "          class: title;\n"
        "          text { Welcome to CHTL }\n"
        "        }\n"
        "        button {\n"
        "          style { @Style button; }\n"
        "          script {\n"
        "            this.addEventListener('click', () => {\n"
        "              alert('Hello CHTL!');\n"
        "            });\n"
        "          }\n"
        "          text { Click me }\n"
        "        }\n"
        "      }\n"
        "    }\n"
        "  }\n"
        "}");
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}