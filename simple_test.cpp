#include <iostream>
#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/CHTL/CHTLParser/Parser.h"
#include "src/CHTL/CHTLGenerator/Generator.h"

using namespace CHTL;

int main() {
    try {
        std::cout << "=== CHTL简单语法测试 ===" << std::endl;
        
        // 1. 简单的词法分析测试
        std::cout << "1. 测试词法分析..." << std::endl;
        String simpleCode = "div { text: \"Hello World\" }";
        auto lexer = std::make_unique<Lexer>(simpleCode);
        
        // 创建解析器
        ParserConfig config;
        config.enableRecovery = true;
        Parser parser(std::move(lexer), config);
        
        std::cout << "   解析器创建成功" << std::endl;
        
        // 2. 简单的解析测试
        std::cout << "2. 测试语法解析..." << std::endl;
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "   AST创建成功，类型: " << static_cast<int>(ast->getType()) << std::endl;
        } else {
            std::cout << "   AST创建失败" << std::endl;
        }
        
        // 3. 简单的生成测试
        std::cout << "3. 测试代码生成..." << std::endl;
        if (ast) {
            Generator generator;
            auto output = generator.generate(ast.get());
            
            if (output.success) {
                std::cout << "   代码生成成功" << std::endl;
                std::cout << "   HTML输出长度: " << output.html.length() << std::endl;
            } else {
                std::cout << "   代码生成失败" << std::endl;
                for (const auto& error : output.errors) {
                    std::cout << "   错误: " << error << std::endl;
                }
            }
        }
        
        std::cout << std::endl << "=== 简单测试完成 ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cout << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}