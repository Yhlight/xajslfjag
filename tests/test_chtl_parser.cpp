#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include <iostream>
#include <vector>
#include <string>

void testCHTLParser(const std::string& name, const std::string& source) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码: " << source << std::endl;
    
    try {
        // 词法分析
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return;
        }
        
        std::cout << "✅ 词法分析通过，Token数量: " << tokens.size() << std::endl;
        
        // 语法分析
        CHTL::CHTLParser parser(tokens);
        auto document = parser.parseDocument();
        
        if (parser.hasErrors()) {
            std::cout << "❌ 语法分析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        } else {
            std::cout << "✅ 语法分析通过" << std::endl;
        }
        
        if (document) {
            std::cout << "文档节点子项数量: " << document->children.size() << std::endl;
            
            // 打印AST结构（简化版）
            std::cout << "AST结构:" << std::endl;
            for (size_t i = 0; i < document->children.size() && i < 5; ++i) {
                auto child = document->children[i];
                std::cout << "  - " << child->getTypeName() << ": " << child->name;
                if (!child->value.empty()) {
                    if (child->value.length() > 30) {
                        std::cout << " (" << child->value.substr(0, 30) << "...)";
                    } else {
                        std::cout << " (" << child->value << ")";
                    }
                }
                std::cout << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "❌ 未知异常" << std::endl;
    }
}

int main() {
    std::cout << "=== CHTL Parser测试 ===" << std::endl;
    
    // 1. 简单元素测试
    testCHTLParser("简单元素", 
        "div {\n"
        "  text { Hello World }\n"
        "}");
    
    // 2. 带属性的元素
    testCHTLParser("带属性元素", 
        "div {\n"
        "  id: container;\n"
        "  class: main-content;\n"
        "  text { Content }\n"
        "}");
    
    // 3. use声明
    testCHTLParser("use声明", "use html5;");
    
    // 4. 模板定义
    testCHTLParser("模板定义", 
        "[Template] @Style ButtonStyle {\n"
        "  background-color: #007bff;\n"
        "  border: none;\n"
        "}");
    
    // 5. 模板使用
    testCHTLParser("模板使用", "@Style ButtonStyle;");
    
    // 6. 命名空间
    testCHTLParser("命名空间", 
        "[Namespace] app {\n"
        "  div { text { App Content } }\n"
        "}");
    
    // 7. 选择器自动化
    testCHTLParser("选择器自动化", 
        ".container {\n"
        "  text { Auto Class }\n"
        "}");
    
    // 8. 样式块
    testCHTLParser("样式块", 
        "div {\n"
        "  style {\n"
        "    color: red;\n"
        "    .child { font-size: 14px; }\n"
        "  }\n"
        "}");
    
    // 9. 原始嵌入测试
    testCHTLParser("原始嵌入", 
        "[Origin] @Html RawContent {\n"
        "  <div class=\"raw\">Raw HTML</div>\n"
        "}");
    
    std::cout << "\n=== Parser测试完成 ===" << std::endl;
    return 0;
}