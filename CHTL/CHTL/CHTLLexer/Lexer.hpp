#pragma once
#include "Token.hpp"
#include "GlobalMap.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL词法分析器
 * 严格按照CHTL语法文档实现，支持所有CHTL语法特性
 */
class Lexer {
private:
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    std::vector<Token> tokens;
    
    // 当前字符
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    
    // 字符判断
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isValidIdentifierChar(char c) const;
    
    // 词法单元识别
    Token readString();
    Token readUnquotedLiteral();
    Token readNumber();
    Token readIdentifier();
    Token readComment();
    Token readBlockType();
    Token readTypePrefix();
    
    // 特殊处理
    Token handleAtSymbol();
    Token handleBracket();
    Token handleColonEquals();
    Token handleDotSharp();
    
    // 关键字和HTML元素识别
    TokenType classifyIdentifier(const std::string& identifier) const;
    bool isKeywordContext() const;
    
    // 错误处理
    void reportError(const std::string& message) const;

public:
    Lexer();
    explicit Lexer(const std::string& source);
    
    // 主要接口
    void setSource(const std::string& source);
    std::vector<Token> tokenize();
    const std::vector<Token>& getTokens() const;
    
    // 单个token读取
    Token nextToken();
    bool hasMoreTokens() const;
    
    // 工具方法
    void reset();
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 调试
    void dumpTokens() const;
    std::string getTokensAsString() const;
};

/**
 * 词法分析器工厂
 * 提供不同配置的词法分析器实例
 */
class LexerFactory {
public:
    static std::unique_ptr<Lexer> createCHTLLexer();
    static std::unique_ptr<Lexer> createCHTLLexerWithConfig(const GlobalMap::Configuration& config);
};

}