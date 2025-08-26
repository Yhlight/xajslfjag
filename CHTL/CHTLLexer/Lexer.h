#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <stack>

namespace CHTL {

// 词法分析器状态
enum class LexerState {
    NORMAL,              // 正常状态
    IN_STRING,           // 在字符串内
    IN_COMMENT,          // 在注释内
    IN_MULTI_COMMENT,    // 在多行注释内
    IN_GENERATOR_COMMENT,// 在生成器注释内
    IN_UNQUOTED_LITERAL  // 在无引号字面量内
};

// 词法分析器
class Lexer {
private:
    std::string source;          // 源代码
    size_t position;             // 当前位置
    size_t line;                 // 当前行号
    size_t column;               // 当前列号
    size_t tokenStartPos;        // token开始位置
    size_t tokenStartLine;       // token开始行号
    size_t tokenStartColumn;     // token开始列号
    
    LexerState state;            // 当前状态
    std::stack<LexerState> stateStack;  // 状态栈
    
    std::shared_ptr<GlobalMap> globalMap;  // 全局符号表
    
    // 辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    bool isAtEnd() const;
    bool match(char expected);
    bool matchSequence(const std::string& sequence);
    
    // 字符判断
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isSymbol(char c) const;
    
    // Token扫描方法
    Token scanString(char quote);
    Token scanUnquotedLiteral();
    Token scanIdentifier();
    Token scanNumber();
    Token scanComment();
    Token scanMultiLineComment();
    Token scanGeneratorComment();
    Token scanOperator();
    Token scanSymbol();
    Token scanBracketKeyword();  // 扫描 [Template] 等关键字
    Token scanAtType();          // 扫描 @Style 等类型
    
    // 特殊处理
    Token handleSpecialSequences();  // 处理特殊序列如 "at top", "at bottom"
    bool isKeywordStart(const std::string& word) const;
    TokenType getKeywordType(const std::string& word) const;
    
    // 错误处理
    void reportError(const std::string& message);
    
public:
    Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap);
    ~Lexer() = default;
    
    // 主要接口
    std::vector<Token> tokenize();
    Token nextToken();
    
    // 状态管理
    void pushState(LexerState newState);
    void popState();
    LexerState getCurrentState() const { return state; }
    
    // 位置信息
    size_t getPosition() const { return position; }
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
    
    // 重置
    void reset();
    
    // 设置源代码
    void setSource(const std::string& newSource);
    
    // 调试
    void dumpTokens(const std::vector<Token>& tokens) const;
};

} // namespace CHTL

#endif // CHTL_LEXER_H