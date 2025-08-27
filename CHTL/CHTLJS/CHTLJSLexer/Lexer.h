#ifndef CHTLJS_LEXER_H
#define CHTLJS_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTLJS {

// CHTL JS 词法分析器
class Lexer {
private:
    std::string source;          // 源代码
    size_t position;             // 当前位置
    int line;                    // 当前行号
    int column;                  // 当前列号
    std::vector<Token> tokens;   // Token列表
    GlobalMap* globalMap;        // 全局映射表
    bool inCHTLSelector;         // 是否在CHTL选择器内部
    
    // 辅助方法
    char getCurrentChar() const;
    char peekChar(int offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipNewline();
    
    // Token扫描方法
    Token scanIdentifierOrKeyword();
    Token scanString();
    Token scanNumber();
    Token scanOperator();
    Token scanComment();
    Token scanCHTLSelector();  // 扫描{{}}选择器
    
    // 辅助判断方法
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isOperatorStart(char c) const;
    
    // 创建Token
    Token makeToken(TokenType type, const std::string& value, int startColumn);
    
    // 扫描多字符操作符
    Token scanMultiCharOperator(char first);

public:
    Lexer(const std::string& src);
    ~Lexer();
    
    // 执行词法分析
    std::vector<Token> tokenize();
    
    // 获取下一个Token
    Token getNextToken();
    
    // 重置词法分析器
    void reset();
    
    // 设置源代码
    void setSource(const std::string& src);
    
    // 获取所有Token（不包括空白和注释）
    std::vector<Token> getFilteredTokens();
    
    // 获取错误信息
    std::string getErrorContext() const;
    
    // 设置是否在CHTL选择器内
    void setInCHTLSelector(bool inSelector) { inCHTLSelector = inSelector; }
};

} // namespace CHTLJS

#endif // CHTLJS_LEXER_H