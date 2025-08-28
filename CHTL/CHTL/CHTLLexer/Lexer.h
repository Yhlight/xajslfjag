#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <istream>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

// CHTL词法分析器 - 负责将源代码转换为Token流
class Lexer {
private:
    // 输入源
    std::string source_;         // 源代码字符串
    size_t current_;             // 当前字符位置
    size_t line_;                // 当前行号
    size_t column_;              // 当前列号
    
    // Token缓冲区
    std::vector<Token> tokens_;  // 已解析的Token列表
    
    // 错误信息
    std::vector<std::string> errors_;
    
    // 辅助方法 - 字符操作
    bool isAtEnd() const;
    char peek() const;
    char peekNext() const;
    char advance();
    bool match(char expected);
    bool matchSequence(const std::string& sequence);
    
    // 辅助方法 - 字符判断
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    
    // 扫描方法
    void scanToken();
    void scanString(char quote);
    void scanUnquotedLiteral();
    void scanNumber();
    void scanIdentifier();
    void scanComment();
    void scanMultiLineComment();
    void scanGeneratorComment();
    void scanBracketKeyword();
    void scanAtType();
    void scanOperator();
    
    // 特殊处理方法
    void handleColonEquals();    // 处理CE对等式 (: 和 = 等价)
    void handleDoubleSlash();    // 处理 // 注释或操作符
    void handleSlashStar();      // 处理 /* */ 注释
    
    // 错误处理
    void addError(const std::string& message);
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& lexeme) const;
    Token makeToken(TokenType type, const std::string& lexeme, const std::string& value) const;
    
    // 位置记录
    size_t startLine_;
    size_t startColumn_;
    size_t startPos_;
    
    void markStart();
    std::string getCurrentLexeme() const;
    
public:
    // 构造函数
    Lexer();
    explicit Lexer(const std::string& source);
    
    // 析构函数
    ~Lexer() = default;
    
    // 主要接口
    void setSource(const std::string& source);
    void setSource(std::istream& input);
    
    // 扫描所有Token
    std::vector<Token> scanTokens();
    
    // 获取下一个Token（用于流式处理）
    Token nextToken();
    
    // 获取错误信息
    const std::vector<std::string>& getErrors() const { return errors_; }
    bool hasErrors() const { return !errors_.empty(); }
    
    // 重置词法分析器
    void reset();
    
    // 调试方法
    void printTokens() const;
};

} // namespace CHTL

#endif // CHTL_LEXER_H