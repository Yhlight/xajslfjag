#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// 词法分析器类
class Lexer {
private:
    std::string source;          // 源代码
    size_t position;             // 当前位置
    size_t line;                 // 当前行号
    size_t column;               // 当前列号
    size_t start_pos;            // 当前token起始位置
    std::vector<Token> tokens;   // 生成的token列表
    GlobalMap* global_map;       // 全局符号表引用
    
    // 字符检查辅助函数
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool match(char expected);
    bool matchSequence(const std::string& sequence);
    
    // Token生成辅助函数
    void skipWhitespace();
    void skipComment();
    Token makeToken(TokenType type, const std::string& value = "");
    Token makeErrorToken(const std::string& message);
    
    // 具体的词法分析函数
    Token scanString(char quote);
    Token scanUnquotedLiteral();
    Token scanNumber();
    Token scanIdentifier();
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    Token scanPrefix();
    Token scanTypeIdentifier();
    Token scanSelector();
    Token scanPath();
    
    // 上下文相关的扫描
    bool isInStyleBlock() const;
    bool isInScriptBlock() const;
    bool isInConfigBlock() const;
    
    // 关键字和标识符处理
    TokenType getIdentifierType(const std::string& text);
    bool isReservedKeyword(const std::string& text) const;
    
public:
    Lexer(const std::string& source, GlobalMap* globalMap = nullptr);
    ~Lexer() = default;
    
    // 主要接口
    std::vector<Token> tokenize();
    Token nextToken();
    void reset();
    void reset(const std::string& newSource);
    
    // 获取器
    const std::vector<Token>& getTokens() const;
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;
    
    // 调试支持
    void printTokens() const;
    
private:
    std::vector<std::string> errors;  // 错误信息列表
    void addError(const std::string& message);
};

} // namespace CHTL

#endif // CHTL_LEXER_H