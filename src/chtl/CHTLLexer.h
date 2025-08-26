#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <unordered_map>
#include "CHTLToken.h"

namespace chtl {

// 前向声明
class CHTLContext;

// 词法分析器配置
struct LexerConfig {
    bool skipComments = false;         // 是否跳过注释
    bool preserveWhitespace = false;   // 是否保留空白字符
    bool trackIndentation = true;      // 是否跟踪缩进
    bool allowUnquotedLiterals = true; // 是否允许无修饰字面量
    bool enableCEEquality = true;      // 是否启用CE对等式
};

// 词法分析器
class CHTLLexer {
public:
    CHTLLexer();
    explicit CHTLLexer(const LexerConfig& config);
    CHTLLexer(std::istream& input, const std::string& filename = "");
    ~CHTLLexer();
    
    // 设置输入
    void setInput(std::istream& input, const std::string& filename = "");
    void setInput(const std::string& input, const std::string& filename = "");
    
    // 配置
    void setConfig(const LexerConfig& config) { this->config = config; }
    const LexerConfig& getConfig() const { return config; }
    
    // 主要接口
    Token nextToken();
    Token peekToken(int offset = 0);
    std::vector<Token> tokenize();
    
    // 位置信息
    TokenLocation getCurrentLocation() const;
    size_t getCurrentLine() const { return currentLine; }
    size_t getCurrentColumn() const { return currentColumn; }
    
    // 状态查询
    bool isAtEnd() const;
    bool hasError() const { return !errorMessage.empty(); }
    std::string getError() const { return errorMessage; }
    
    // 缩进管理
    int getCurrentIndentLevel() const { return currentIndentLevel; }
    
    // 重置
    void reset();
    
private:
    // 字符操作
    char peek(int offset = 0) const;
    char advance();
    bool match(char expected);
    bool matchSequence(const std::string& seq);
    void skipWhitespace();
    void skipToEndOfLine();
    
    // Token识别方法
    Token scanToken();
    Token scanIdentifier();
    Token scanString();
    Token scanUnquotedLiteral();
    Token scanNumber();
    Token scanComment();
    Token scanGeneratorComment();
    Token scanBlockKeyword();
    Token scanTypeKeyword();
    
    // 关键字识别
    TokenType checkKeyword(const std::string& text) const;
    bool isBlockKeyword(const std::string& text) const;
    bool isTypeKeyword(const std::string& text) const;
    bool isElementKeyword(const std::string& text) const;
    bool isOperationKeyword(const std::string& text) const;
    
    // 缩进处理
    void handleIndentation();
    int calculateIndentLevel(const std::string& line) const;
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isValidIdentifierStart(char c) const;
    bool isValidIdentifierChar(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    
    // 错误处理
    void setError(const std::string& message);
    Token makeErrorToken(const std::string& message);
    
    // Token创建
    Token makeToken(TokenType type);
    Token makeToken(TokenType type, const std::string& text);
    Token makeToken(TokenType type, const TokenValue& value, const std::string& text);
    
private:
    // 输入管理
    std::string input;
    size_t current;
    size_t start;
    std::string filename;
    
    // 位置跟踪
    size_t currentLine;
    size_t currentColumn;
    size_t startLine;
    size_t startColumn;
    
    // 缩进管理
    std::vector<int> indentStack;
    int currentIndentLevel;
    bool atLineStart;
    
    // 配置
    LexerConfig config;
    
    // 错误信息
    std::string errorMessage;
    
    // Token缓存（用于peek）
    mutable std::vector<Token> tokenBuffer;
    mutable size_t bufferIndex;
    
    // 关键字映射表
    static const std::unordered_map<std::string, TokenType> keywords;
    static const std::unordered_map<std::string, TokenType> blockKeywords;
    static const std::unordered_map<std::string, TokenType> typeKeywords;
    
    // 初始化静态数据
    static std::unordered_map<std::string, TokenType> initKeywords();
    static std::unordered_map<std::string, TokenType> initBlockKeywords();
    static std::unordered_map<std::string, TokenType> initTypeKeywords();
};

} // namespace chtl

#endif // CHTL_LEXER_H