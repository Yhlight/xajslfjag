#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

// CHTL词法分析器
class Lexer {
public:
    Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap);
    ~Lexer();
    
    // 获取下一个Token
    Token NextToken();
    
    // 查看下一个Token但不消费
    Token PeekToken();
    Token PeekToken(int offset);  // 向前查看多个Token
    
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
    
private:
    std::string source;
    std::shared_ptr<GlobalMap> globalMap;
    
    int position;
    int line;
    int column;
    
    std::vector<Token> tokenBuffer;  // Token缓冲区，用于PeekToken
    std::vector<std::string> errors;
    bool debugMode;
    
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
    Token ReadGeneratorComment();
    Token ReadPrefixKeyword();
    Token ReadTypeIdentifier();
    
    // 特殊处理
    bool IsStartOfPrefixKeyword() const;
    bool IsStartOfTypeIdentifier() const;
    bool IsValidUnquotedChar(char c) const;
    TokenType DetermineIdentifierType(const std::string& value);
    
    // 处理"at top"和"at bottom"
    Token HandleAtKeyword();
};

} // namespace CHTL