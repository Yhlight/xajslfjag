#include "src/CHTL/CHTLLexer/EnhancedLexer.h"
#include "src/CHTL/CHTLLexer/GlobalMap.h"
#include <iostream>
#include <fstream>

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "使用方法: " << argv[0] << " <chtl文件>" << std::endl;
        return 1;
    }
    
    // 读取文件
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << argv[1] << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 创建词法分析器
    auto globalMap = std::make_shared<GlobalMap>();
    auto lexer = std::make_shared<EnhancedLexer>(globalMap);
    
    lexer->SetSource(source);
    lexer->Analyze();
    
    // 输出词法分析结果
    if (lexer->HasErrors()) {
        std::cout << "=== 词法分析错误 ===" << std::endl;
        for (const auto& error : lexer->GetErrors()) {
            std::cout << error << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << "=== Token序列 ===" << std::endl;
    const auto& tokens = lexer->GetTokens();
    for (size_t i = 0; i < tokens.size() && i < 30; ++i) {  // 增加显示数量
        const auto& token = tokens[i];
        
        // 转换Token类型为名称
        std::string typeName;
        switch (token.type) {
            case TokenType::IDENTIFIER: typeName = "IDENTIFIER"; break;
            case TokenType::STRING_LITERAL: typeName = "STRING_LITERAL"; break;
            case TokenType::UNQUOTED_LITERAL: typeName = "UNQUOTED_LITERAL"; break;
            case TokenType::NUMBER: typeName = "NUMBER"; break;
            case TokenType::TEXT: typeName = "TEXT"; break;
            case TokenType::STYLE: typeName = "STYLE"; break;
            case TokenType::SCRIPT: typeName = "SCRIPT"; break;
            case TokenType::USE: typeName = "USE"; break;
            case TokenType::HTML5: typeName = "HTML5"; break;
            case TokenType::LEFT_BRACE: typeName = "LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: typeName = "RIGHT_BRACE"; break;
            case TokenType::LEFT_BRACKET: typeName = "LEFT_BRACKET"; break;
            case TokenType::RIGHT_BRACKET: typeName = "RIGHT_BRACKET"; break;
            case TokenType::LEFT_PAREN: typeName = "LEFT_PAREN"; break;
            case TokenType::RIGHT_PAREN: typeName = "RIGHT_PAREN"; break;
            case TokenType::SEMICOLON: typeName = "SEMICOLON"; break;
            case TokenType::COLON: typeName = "COLON"; break;
            case TokenType::EQUALS: typeName = "EQUALS"; break;
            case TokenType::COMMA: typeName = "COMMA"; break;
            case TokenType::DOT: typeName = "DOT"; break;
            case TokenType::HASH: typeName = "HASH"; break;
            case TokenType::AMPERSAND: typeName = "AMPERSAND"; break;
            case TokenType::PLUS: typeName = "PLUS"; break;
            case TokenType::MINUS: typeName = "MINUS"; break;
            case TokenType::STAR: typeName = "STAR"; break;
            case TokenType::SLASH: typeName = "SLASH"; break;
            case TokenType::NEWLINE: typeName = "NEWLINE"; break;
            case TokenType::WHITESPACE: typeName = "WHITESPACE"; break;
            case TokenType::EOF_TOKEN: typeName = "EOF_TOKEN"; break;
            default: typeName = "UNKNOWN(" + std::to_string(static_cast<int>(token.type)) + ")"; break;
        }
        
        std::string displayValue = token.value;
        if (token.value == "\n") displayValue = "\\n";
        if (token.value == " ") displayValue = "\\space";
        if (token.value == "\t") displayValue = "\\t";
        
        std::cout << i << ": " << typeName
                  << " [" << displayValue << "] "
                  << "行:" << token.position.line 
                  << " 列:" << token.position.column << std::endl;
    }
    
    if (tokens.size() > 20) {
        std::cout << "... 还有 " << (tokens.size() - 20) << " 个Token" << std::endl;
    }
    
    return 0;
}