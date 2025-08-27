#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTLJS {

// CHTL JS词法分析器
class Lexer {
public:
    Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap);
    ~Lexer();
    
    // 获取下一个Token
    Token NextToken();
    
    // 查看下一个Token但不消费
    Token PeekToken();
    Token PeekToken(int offset);
    
    // 检查是否到达文件末尾
    bool IsEOF() const;
    
    // 获取当前位置信息
    int GetLine() const { return line; }
    int GetColumn() const { return column; }
    int GetPosition() const { return position; }
    
    // 错误处理
    void ReportError(const std::string& message);
    std::vector<std::string> GetErrors() const { return errors; }
    bool HasErrors() const { return !errors.empty(); }
    
    // 设置配置
    void SetDebugMode(bool debug) { debugMode = debug; }
    
    // 特殊处理
    bool IsInEnhancedSelector() const { return inEnhancedSelector; }
    void SetInEnhancedSelector(bool in) { inEnhancedSelector = in; }
    
private:
    std::string source;
    std::shared_ptr<GlobalMap> globalMap;
    
    int position;
    int line;
    int column;
    
    std::vector<Token> tokenBuffer;
    std::vector<std::string> errors;
    bool debugMode;
    
    // 状态标志
    bool inEnhancedSelector;  // 是否在{{}}内部
    
    // 辅助方法
    char CurrentChar() const;
    char PeekChar(int offset = 1) const;
    void Advance();
    void SkipWhitespace();
    void SkipNewline();
    
    // Token解析方法
    Token ReadIdentifier();
    Token ReadNumber();
    Token ReadStringLiteral(char quote);
    Token ReadUnquotedLiteral();
    Token ReadComment();
    Token ReadOperator();
    
    // 特殊处理
    Token ReadEnhancedSelector();
    Token ReadArrowOperator();
    Token ReadDoubleLeftBrace();
    
    // 判断方法
    bool IsValidIdentifierStart(char c) const;
    bool IsValidIdentifierChar(char c) const;
    bool IsValidUnquotedChar(char c) const;
    TokenType DetermineIdentifierType(const std::string& value);
};

} // namespace CHTLJS