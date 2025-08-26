#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include <iostream>
#include <vector>
#include <string>

void testOriginContent(const std::string& name, const std::string& source) {
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
    std::cout << "=== 原始嵌入内容处理测试 ===" << std::endl;
    
    // 1. 基础原始嵌入 - HTML内容
    testOriginContent("原始HTML嵌入", 
        "[Origin] @Html RawContent { <div class=\"test\">Raw HTML</div> }");
    
    // 2. 原始CSS嵌入
    testOriginContent("原始CSS嵌入", 
        "[Origin] @Style GlobalCSS { \n"
        "  * { margin: 0; padding: 0; }\n"
        "  .container { max-width: 1200px; }\n"
        "}");
    
    // 3. 原始JavaScript嵌入
    testOriginContent("原始JavaScript嵌入", 
        "[Origin] @JavaScript Utils { \n"
        "  function log(msg) { console.log(msg); }\n"
        "  const data = { key: 'value' };\n"
        "}");
    
    // 4. 复杂嵌套结构的原始嵌入
    testOriginContent("复杂嵌套原始嵌入", 
        "[Origin] @Html ComplexHTML {\n"
        "  <div class=\"modal\">\n"
        "    <script>alert('test');</script>\n"
        "    <style>.modal { display: flex; }</style>\n"
        "    <p>Content with {braces} and <tags></p>\n"
        "  </div>\n"
        "}");
    
    // 5. 自定义类型的原始嵌入
    testOriginContent("自定义类型原始嵌入", 
        "[Origin] @Vue VueComponent {\n"
        "  <template>\n"
        "    <div>{{ message }}</div>\n"
        "  </template>\n"
        "  <script>\n"
        "    export default { data() { return { message: 'Hello' }; } }\n"
        "  </script>\n"
        "}");
    
    // 6. 空内容原始嵌入
    testOriginContent("空原始嵌入", 
        "[Origin] @Html Empty { }");
    
    // 7. 带命名的原始嵌入
    testOriginContent("命名原始嵌入", 
        "[Origin] @Html navbar {\n"
        "  <nav class=\"navbar\">\n"
        "    <a href=\"/\">Home</a>\n"
        "    <a href=\"/about\">About</a>\n"
        "  </nav>\n"
        "}");
    
    // 8. 原始嵌入后继续CHTL语法
    testOriginContent("原始嵌入后继续CHTL", 
        "[Origin] @Style reset { * { margin: 0; } }\n"
        "div {\n"
        "  style { color: blue; }\n"
        "  text { After origin block }\n"
        "}");
    
    std::cout << "\n=== 原始嵌入测试完成 ===" << std::endl;
    return 0;
}