#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/CHTL/CHTLParser/Parser.h"
#include <iostream>
#include <memory>

int main() {
    std::cout << "🚀 最小化测试开始" << std::endl;
    
    try {
        std::string content = "<!DOCTYPE html><html><head><title>Test</title></head><body><h1>Hello CHTL</h1></body></html>";
        
        std::cout << "✅ 测试内容: " << content.size() << " 字节" << std::endl;
        
        CHTL::LexerConfig lexerConfig;
        auto lexer = std::make_unique<CHTL::Lexer>(content, lexerConfig);
        std::cout << "✅ 词法分析器创建成功" << std::endl;
        
        CHTL::ParserConfig config;
        config.strictMode = false;
        config.enableRecovery = true;
        
        CHTL::Parser parser(std::move(lexer), config);
        std::cout << "✅ 解析器创建成功" << std::endl;
        
        std::cout << "🔍 开始解析..." << std::endl;
        auto ast = parser.parse();
        
        if (ast) {
            std::cout << "🎉 解析成功！节点类型: " << static_cast<int>(ast->getType()) << std::endl;
        } else {
            std::cout << "❌ 解析失败" << std::endl;
            auto errors = parser.getErrors();
            for (const auto& error : errors) {
                std::cout << "   错误: " << error.toString() << std::endl;
            }
        }
        
        std::cout << "✅ 最小化测试完成" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
}