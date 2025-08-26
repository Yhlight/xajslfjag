#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"
#include "CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== CHTL简化语法测试 ===" << std::endl;
    
    // 测试1: 词法分析器基础功能
    {
        std::cout << "\n--- 测试1: 词法分析器 ---" << std::endl;
        std::string source = "<div class=\"test\">Hello World</div>";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "Token数量: " << tokens.size() << std::endl;
        for (const auto& token : tokens) {
            std::cout << "  " << token.getTypeName() << ": \"" << token.value << "\"" << std::endl;
        }
        
        if (lexer.hasErrors()) {
            std::cout << "词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "  " << error << std::endl;
            }
        } else {
            std::cout << "✓ 词法分析通过" << std::endl;
        }
    }
    
    // 测试2: 语法分析器基础功能
    {
        std::cout << "\n--- 测试2: 语法分析器 ---" << std::endl;
        std::string source = "<div>Simple content</div>";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (!lexer.hasErrors()) {
            CHTL::CHTLParser parser(tokens);
            auto ast = parser.parseDocument();
            
            if (parser.hasErrors()) {
                std::cout << "语法分析错误:" << std::endl;
                for (const auto& error : parser.getErrors()) {
                    std::cout << "  " << error << std::endl;
                }
            } else if (ast) {
                std::cout << "✓ 语法分析通过，生成AST" << std::endl;
                std::cout << "AST节点类型: " << ast->getTypeName() << std::endl;
                std::cout << "子节点数量: " << ast->children.size() << std::endl;
            } else {
                std::cout << "✗ AST为空" << std::endl;
            }
        }
    }
    
    // 测试3: 代码生成器基础功能
    {
        std::cout << "\n--- 测试3: 代码生成器 ---" << std::endl;
        std::string source = "<h1>Test Title</h1><p>Test content</p>";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        if (!lexer.hasErrors()) {
            CHTL::CHTLParser parser(tokens);
            auto ast = parser.parseDocument();
            
            if (!parser.hasErrors() && ast) {
                CHTL::CHTLGenerator generator;
                auto result = generator.generate(ast);
                
                if (result.success) {
                    std::cout << "✓ 代码生成通过" << std::endl;
                    std::cout << "生成的HTML:\n" << result.html << std::endl;
                    if (!result.css.empty()) {
                        std::cout << "生成的CSS:\n" << result.css << std::endl;
                    }
                } else {
                    std::cout << "✗ 代码生成失败:" << std::endl;
                    for (const auto& error : result.errors) {
                        std::cout << "  " << error << std::endl;
                    }
                }
            }
        }
    }
    
    // 测试4: CHTL关键字识别
    {
        std::cout << "\n--- 测试4: CHTL关键字识别 ---" << std::endl;
        std::string source = "template Button { <button>Click</button> }";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        bool foundTemplate = false;
        bool foundButton = false;
        
        for (const auto& token : tokens) {
            if (token.type == CHTL::CHTLTokenType::TEMPLATE) {
                foundTemplate = true;
                std::cout << "找到TEMPLATE关键字" << std::endl;
            }
            if (token.type == CHTL::CHTLTokenType::IDENTIFIER && token.value == "Button") {
                foundButton = true;
                std::cout << "找到Button标识符" << std::endl;
            }
        }
        
        if (foundTemplate && foundButton) {
            std::cout << "✓ CHTL关键字识别正常" << std::endl;
        } else {
            std::cout << "✗ CHTL关键字识别失败" << std::endl;
        }
    }
    
    // 测试5: 变量语法识别
    {
        std::cout << "\n--- 测试5: 变量语法识别 ---" << std::endl;
        std::string source = "@var { color: red }";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        bool foundAtVar = false;
        for (const auto& token : tokens) {
            if (token.type == CHTL::CHTLTokenType::AT_VAR) {
                foundAtVar = true;
                std::cout << "找到@var关键字" << std::endl;
                break;
            }
        }
        
        if (foundAtVar) {
            std::cout << "✓ 变量语法识别正常" << std::endl;
        } else {
            std::cout << "✗ 变量语法识别失败" << std::endl;
        }
    }
    
    // 测试6: 样式语法识别
    {
        std::cout << "\n--- 测试6: 样式语法识别 ---" << std::endl;
        std::string source = "@style { .btn { color: blue; } }";
        std::cout << "源码: " << source << std::endl;
        
        CHTL::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        bool foundAtStyle = false;
        for (const auto& token : tokens) {
            if (token.type == CHTL::CHTLTokenType::AT_STYLE) {
                foundAtStyle = true;
                std::cout << "找到@style关键字" << std::endl;
                break;
            }
        }
        
        if (foundAtStyle) {
            std::cout << "✓ 样式语法识别正常" << std::endl;
        } else {
            std::cout << "✗ 样式语法识别失败" << std::endl;
        }
    }
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    return 0;
}