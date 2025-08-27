#include "src/CHTL/CHTLLexer/Lexer.h"
#include "src/CHTL/CHTLLexer/EnhancedLexer.h"
#include <iostream>
#include <fstream>

std::string TokenTypeToString(CHTL::TokenType type) {
    switch (type) {
        case CHTL::TokenType::IDENTIFIER: return "IDENTIFIER";
        case CHTL::TokenType::TEXT: return "TEXT";
        case CHTL::TokenType::STYLE: return "STYLE";
        case CHTL::TokenType::SCRIPT: return "SCRIPT";
        case CHTL::TokenType::USE: return "USE";
        case CHTL::TokenType::HTML5: return "HTML5";
        case CHTL::TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case CHTL::TokenType::NUMBER: return "NUMBER";
        case CHTL::TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case CHTL::TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case CHTL::TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case CHTL::TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case CHTL::TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case CHTL::TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case CHTL::TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case CHTL::TokenType::SEMICOLON: return "SEMICOLON";
        case CHTL::TokenType::COLON: return "COLON";
        case CHTL::TokenType::EQUALS: return "EQUALS";
        case CHTL::TokenType::COMMA: return "COMMA";
        case CHTL::TokenType::DOT: return "DOT";
        case CHTL::TokenType::HASH: return "HASH";
        case CHTL::TokenType::AMPERSAND: return "AMPERSAND";
        case CHTL::TokenType::PLUS: return "PLUS";
        case CHTL::TokenType::MINUS: return "MINUS";
        case CHTL::TokenType::STAR: return "STAR";
        case CHTL::TokenType::SLASH: return "SLASH";
        case CHTL::TokenType::COMMENT_LINE: return "COMMENT_LINE";
        case CHTL::TokenType::COMMENT_BLOCK: return "COMMENT_BLOCK";
        case CHTL::TokenType::COMMENT_GENERATOR: return "COMMENT_GENERATOR";
        case CHTL::TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case CHTL::TokenType::NEWLINE: return "NEWLINE";
        case CHTL::TokenType::WHITESPACE: return "WHITESPACE";
        case CHTL::TokenType::TEMPLATE: return "TEMPLATE";
        case CHTL::TokenType::CUSTOM: return "CUSTOM";
        case CHTL::TokenType::ORIGIN: return "ORIGIN";
        case CHTL::TokenType::IMPORT: return "IMPORT";
        case CHTL::TokenType::NAMESPACE: return "NAMESPACE";
        case CHTL::TokenType::CONFIGURATION: return "CONFIGURATION";
        case CHTL::TokenType::AT_STYLE: return "AT_STYLE";
        case CHTL::TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case CHTL::TokenType::AT_VAR: return "AT_VAR";
        case CHTL::TokenType::AT_HTML: return "AT_HTML";
        case CHTL::TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case CHTL::TokenType::AT_CHTL: return "AT_CHTL";
        case CHTL::TokenType::AT_CJMOD: return "AT_CJMOD";
        case CHTL::TokenType::AT_CONFIG: return "AT_CONFIG";
        case CHTL::TokenType::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN_TYPE";
    }
}

int main() {
    // 读取测试文件
    std::ifstream file("test_template.chtl");
    if (!file.is_open()) {
        std::cerr << "无法打开测试文件" << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "=== 输入CHTL源代码 ===" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;
    
    // 测试增强型词法分析器
    std::cout << "=== 增强型词法分析器结果 ===" << std::endl;
    auto enhancedLexer = std::make_shared<CHTL::EnhancedLexer>();
    enhancedLexer->SetSource(source);
    enhancedLexer->Analyze();
    
    if (enhancedLexer->HasErrors()) {
        std::cout << "词法分析错误：" << std::endl;
        for (const auto& error : enhancedLexer->GetErrors()) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    const auto& tokens = enhancedLexer->GetTokens();
    std::cout << "Token数量: " << tokens.size() << std::endl;
    std::cout << std::endl;
    
    for (size_t i = 0; i < tokens.size() && i < 50; ++i) {
        const auto& token = tokens[i];
        std::cout << "Token[" << i << "]: " 
                  << TokenTypeToString(token.type) 
                  << " = \"" << token.value << "\""
                  << " (行:" << token.position.line 
                  << ", 列:" << token.position.column << ")" << std::endl;
    }
    
    return 0;
}