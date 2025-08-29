#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include <string>
#include <memory>
#include <vector>
#include <queue>
#include "Token.h"
#include "../CHTLJSContext/Context.h"

namespace CHTLJS {

// 词法分析器配置
struct LexerConfig {
    bool skipWhitespace = true;         // 是否跳过空白字符
    bool skipComments = false;          // 是否跳过注释
    bool enableUnquotedLiterals = true; // 是否启用无修饰字面量
};

// CHTL JS词法分析器
class Lexer {
public:
    Lexer(const std::string& source, std::shared_ptr<CompileContext> context,
          const LexerConfig& config = LexerConfig());
    ~Lexer() = default;
    
    // 获取下一个Token
    std::shared_ptr<Token> nextToken();
    
    // 预览下一个Token（不消费）
    std::shared_ptr<Token> peekToken();
    
    // 预览指定数量的Token
    std::vector<std::shared_ptr<Token>> peekTokens(size_t count);
    
    // 检查是否到达文件末尾
    // 声明，在cpp文件中定义
    bool isAtEnd() const;
    
    // 获取当前位置
    size_t getCurrentLine() const { return line_; }
    size_t getCurrentColumn() const { return column_; }
    
    // 重置词法分析器
    void reset();
    
    // 获取所有Token（用于调试）
    std::vector<std::shared_ptr<Token>> tokenizeAll();

private:
    std::string source_;
    std::shared_ptr<CompileContext> context_;
    LexerConfig config_;
    
    // 位置追踪
    size_t current_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    size_t tokenStart_ = 0;
    size_t tokenStartLine_ = 1;
    size_t tokenStartColumn_ = 1;
    
    // Token缓冲区（用于peek功能）
    std::queue<std::shared_ptr<Token>> tokenBuffer_;
    
    // 内部方法
    std::shared_ptr<Token> scanToken();
    
    // 字符操作
    char advance();
    bool match(char expected);
    bool match(const std::string& expected);
    char peek(size_t offset = 0) const;
    bool isAtEnd(size_t offset) const;
    
    // 跳过空白和注释
    void skipWhitespace();
    void skipSingleLineComment();
    void skipMultiLineComment();
    
    // 扫描不同类型的Token
    std::shared_ptr<Token> scanString(char quote);
    std::shared_ptr<Token> scanNumber();
    std::shared_ptr<Token> scanIdentifier();
    std::shared_ptr<Token> scanUnquotedLiteral();
    std::shared_ptr<Token> scanEnhancedSelector();  // 扫描{{}}选择器
    std::shared_ptr<Token> scanSelector();          // 扫描选择器内容
    
    // 辅助方法
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isSelectorChar(char c) const;
    
    // 创建Token
    std::shared_ptr<Token> makeToken(TokenType type) const;
    std::shared_ptr<Token> makeToken(TokenType type, const std::string& lexeme) const;
    std::shared_ptr<Token> makeToken(TokenType type, const TokenValue& value) const;
    
    // 错误处理
    std::shared_ptr<Token> errorToken(const std::string& message) const;
    
    // 更新位置信息
    void updatePosition(char c);
    
    // 特殊处理
    bool checkArrowOperator();  // 检查 -> 和 &->
    TokenType determineSelectorType(const std::string& selector);  // 判断选择器类型
};

// 词法分析异常
class LexerException : public std::exception {
public:
    explicit LexerException(const std::string& message) : msg_(message) {}
    const char* what() const noexcept override { return msg_.c_str(); }
    
private:
    std::string msg_;
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H