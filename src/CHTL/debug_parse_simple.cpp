#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

int main() {
    // Simple test to see what happens with a minimal Configuration block  
    std::string configCode = "[Configuration] {}";
    
    std::cout << "测试最简单的配置解析:" << std::endl;
    std::cout << "代码: " << configCode << std::endl;
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return 1;
        }
        
        std::cout << "✓ 生成 " << tokens.size() << " 个token:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            std::cout << "  " << i << ": type=" << static_cast<int>(tokens[i].type) 
                      << " value='" << tokens[i].value << "'" << std::endl;
        }
        
        CHTLParser parser;
        auto ast = parser.parse(tokens);
        
        std::cout << "\n解析结果:" << std::endl;
        std::cout << "  成功: " << (ast.success ? "是" : "否") << std::endl;
        std::cout << "  处理token数: " << ast.tokensProcessed << " / " << tokens.size() << std::endl;
        std::cout << "  创建节点数: " << ast.nodesCreated << std::endl;
        std::cout << "  配置节点数: " << ast.configs.size() << std::endl;
        std::cout << "  模板节点数: " << ast.templates.size() << std::endl;
        std::cout << "  导入节点数: " << ast.imports.size() << std::endl;
        std::cout << "  命名空间节点数: " << ast.namespaces.size() << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}