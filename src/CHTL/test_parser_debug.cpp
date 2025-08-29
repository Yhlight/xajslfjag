#include <iostream>
#include <string>

#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLNode/ConfigNode.h"
#include "../Error/ErrorReport.h"

using namespace CHTL;

class DebugParser : public CHTLParser {
public:
    DebugParser() : CHTLParser() {}
    
    // Override parseTopLevel to add debug output
    NodePtr parseTopLevelDebug() {
        if (isAtEnd()) {
            std::cout << "Parser: At end of tokens" << std::endl;
            return nullptr;
        }
        
        const Token& token = currentToken();
        std::cout << "Parser: Processing token type " << static_cast<int>(token.type) 
                  << " value '" << token.value << "'" << std::endl;
        
        if (token.type == TokenType::CONFIGURATION) {
            std::cout << "Parser: Found CONFIGURATION token, calling parseConfiguration" << std::endl;
            auto config = parseConfiguration();
            if (config) {
                std::cout << "Parser: parseConfiguration returned valid node" << std::endl;
                return config;
            } else {
                std::cout << "Parser: parseConfiguration returned nullptr" << std::endl;
                return nullptr;
            }
        } else {
            std::cout << "Parser: Token type not handled, advancing" << std::endl;
            advance();
            return nullptr;
        }
    }
    
    // Make some protected methods accessible for testing
    using CHTLParser::parseConfiguration;
    using CHTLParser::isAtEnd;
    using CHTLParser::currentToken;
    using CHTLParser::advance;
};

int main() {
    std::string configCode = R"([Configuration]
{
    DEBUG_MODE = true;
})";
    
    std::cout << "测试解析器调试:" << std::endl;
    std::cout << "===============" << std::endl;
    
    try {
        ErrorReporter errorReporter;
        Lexer lexer(configCode);
        auto tokens = lexer.tokenize();
        
        if (errorReporter.hasErrors()) {
            std::cout << "❌ 词法分析错误:" << std::endl;
            errorReporter.printAll();
            return 1;
        }
        
        std::cout << "✓ 词法分析成功，生成 " << tokens.size() << " 个token" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            std::cout << "  " << i << ": " << static_cast<int>(tokens[i].type) 
                      << " '" << tokens[i].value << "'" << std::endl;
        }
        
        DebugParser parser;
        auto ast = parser.parse(tokens);
        
        std::cout << "\n解析结果:" << std::endl;
        std::cout << "  成功: " << (ast.success ? "是" : "否") << std::endl;
        std::cout << "  根节点: " << (ast.rootNode ? "存在" : "不存在") << std::endl;
        std::cout << "  处理token数: " << ast.tokensProcessed << std::endl;
        std::cout << "  创建节点数: " << ast.nodesCreated << std::endl;
        
        if (ast.rootNode) {
            std::cout << "  根节点子节点数: " << ast.rootNode->getChildren().size() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}