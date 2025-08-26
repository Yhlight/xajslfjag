#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include "CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void testCHTLComplete(const std::string& name, const std::string& source) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "源码:\n" << source << std::endl;
    
    try {
        // 1. 词法分析
        std::cout << "\n1. 词法分析..." << std::endl;
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (lexer.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return;
        }
        
        std::cout << "✅ 词法分析成功，Token数量: " << tokens.size() << std::endl;
        
        // 2. 语法分析
        std::cout << "\n2. 语法分析..." << std::endl;
        CHTL::CHTLParser parser(tokens);
        auto document = parser.parseDocument();
        
        if (parser.hasErrors()) {
            std::cout << "❌ 语法分析错误:" << std::endl;
            for (const auto& error : parser.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
            return;
        }
        
        std::cout << "✅ 语法分析成功，AST节点数量: " << document->children.size() << std::endl;
        
        // 3. 代码生成
        std::cout << "\n3. 代码生成..." << std::endl;
        CHTL::CHTLGenerator generator;
        auto result = generator.generate(document);
        
        if (!result.success) {
            std::cout << "❌ 代码生成错误:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "  " << error << std::endl;
            }
            return;
        }
        
        std::cout << "✅ 代码生成成功" << std::endl;
        
        // 输出生成的HTML
        std::cout << "\n生成的HTML:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << result.html << std::endl;
        
        if (!result.css.empty()) {
            std::cout << "生成的CSS:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << result.css << std::endl;
        }
        
        if (!result.javascript.empty()) {
            std::cout << "生成的JavaScript:" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            std::cout << result.javascript << std::endl;
        }
        
        // 保存输出文件
        std::string filename = "output_" + name + ".html";
        std::replace(filename.begin(), filename.end(), ' ', '_');
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << result.html;
            outFile.close();
            std::cout << "✅ 输出已保存到: " << filename << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "❌ 未知异常" << std::endl;
    }
}

int main() {
    std::cout << "=== CHTL完整编译器测试 ===" << std::endl;
    
    // 1. 简单HTML结构
    testCHTLComplete("简单HTML", 
        "div {\n"
        "  text { Hello CHTL! }\n"
        "}");
    
    // 2. 带属性的元素
    testCHTLComplete("带属性", 
        "div {\n"
        "  id: main-container;\n"
        "  class: wrapper;\n"
        "  text { Welcome to CHTL }\n"
        "}");
    
    // 3. 模板定义和使用
    testCHTLComplete("模板系统", 
        "[Template] @Style ButtonStyle {\n"
        "  background-color: #007bff;\n"
        "  color: white;\n"
        "  padding: 10px 20px;\n"
        "  border: none;\n"
        "  border-radius: 4px;\n"
        "}\n"
        "\n"
        "button {\n"
        "  @Style ButtonStyle;\n"
        "  text { Click Me }\n"
        "}");
    
    // 4. 带局部样式的元素
    testCHTLComplete("局部样式", 
        "div {\n"
        "  class: container;\n"
        "  style {\n"
        "    background: #f5f5f5;\n"
        "    padding: 20px;\n"
        "    .title {\n"
        "      font-size: 24px;\n"
        "      color: #333;\n"
        "    }\n"
        "  }\n"
        "  h1 {\n"
        "    class: title;\n"
        "    text { CHTL Demo }\n"
        "  }\n"
        "}");
    
    // 5. 原始嵌入
    testCHTLComplete("原始嵌入", 
        "[Origin] @Html CustomHTML {\n"
        "  <div class=\"custom\">\n"
        "    <span>Raw HTML Content</span>\n"
        "  </div>\n"
        "}\n"
        "\n"
        "div {\n"
        "  text { This is CHTL generated }\n"
        "}");
    
    // 6. 选择器自动化
    testCHTLComplete("选择器自动化", 
        ".header {\n"
        "  text { Header Content }\n"
        "}\n"
        "\n"
        "#main {\n"
        "  text { Main Content }\n"
        "}");
    
    std::cout << "\n=== 完整编译器测试完成 ===" << std::endl;
    return 0;
}