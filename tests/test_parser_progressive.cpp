#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include <iostream>

bool testParse(const std::string& name, const std::string& source) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码: " << source << std::endl;
    
    try {
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 词法分析错误" << std::endl;
            return false;
        }
        
        std::cout << "✅ 词法分析成功，Token数量: " << tokens.size() << std::endl;
        
        CHTL::CHTLParser parser(tokens);
        auto document = parser.parseDocument();
        
        if (parser.hasErrors()) {
            std::cout << "❌ 语法分析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return false;
        }
        
        std::cout << "✅ 语法分析成功，子项数量: " << document->children.size() << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::cout << "=== 渐进式Parser测试 ===" << std::endl;
    
    // 1. 最简单的use声明
    if (!testParse("use声明", "use html5;")) return 1;
    
    // 2. 简单标识符
    if (!testParse("简单标识符", "div")) return 1;
    
    // 3. 空元素
    if (!testParse("空元素", "div { }")) return 1;
    
    // 4. 文本节点
    if (!testParse("文本节点", "text { hello }")) return 1;
    
    // 5. 带文本的元素
    if (!testParse("带文本的元素", "div { text { hello } }")) return 1;
    
    // 6. 模板使用
    if (!testParse("模板使用", "@Style Button;")) return 1;
    
    // 7. 简单模板定义
    if (!testParse("简单模板定义", "[Template] @Style Button { }")) return 1;
    
    // 8. 带属性的模板定义
    if (!testParse("带属性的模板定义", "[Template] @Style Button { color: red; }")) return 1;
    
    std::cout << "\n✅ 所有测试通过！" << std::endl;
    return 0;
}