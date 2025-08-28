#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "CHTLToken.h"
#include "CHTLGlobalMap.h"

namespace CHTL {

// CHTL词法分析器 - 负责将CHTL源代码分解为词法单元
class CHTLLexer {
public:
    explicit CHTLLexer(const std::string& input = "");
    
    // 设置输入源码
    void SetInput(const std::string& input);
    void SetInput(std::istream& stream);
    
    // 词法分析
    std::vector<CHTLTokenPtr> Tokenize();
    CHTLTokenPtr NextToken();
    
    // 状态查询
    bool IsAtEnd() const;
    size_t GetPosition() const;
    size_t GetLine() const;
    size_t GetColumn() const;
    
    // 错误处理
    bool HasErrors() const;
    const std::vector<std::string>& GetErrors() const;
    void ClearErrors();
    
    // 调试
    void EnableDebugMode(bool enable = true);
    void PrintTokens(const std::vector<CHTLTokenPtr>& tokens) const;
    
private:
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    std::vector<std::string> errors_;
    bool debugMode_;
    
    CHTLGlobalMap& globalMap_;
    
    // 字符处理
    char CurrentChar() const;
    char PeekChar(size_t offset = 1) const;
    void Advance();
    void SkipWhitespace();
    
    // 词法分析辅助方法
    CHTLTokenPtr ReadString(char delimiter);
    CHTLTokenPtr ReadUnquotedLiteral();
    CHTLTokenPtr ReadNumericLiteral();
    CHTLTokenPtr ReadIdentifier();
    CHTLTokenPtr ReadComment();
    CHTLTokenPtr ReadGeneratorComment();
    CHTLTokenPtr ReadSelector();
    CHTLTokenPtr ReadBracketExpression();
    CHTLTokenPtr ReadAtExpression();
    
    // 关键字和标识符处理
    CHTLTokenPtr ProcessIdentifierOrKeyword(const std::string& value);
    bool IsValidIdentifierStart(char c) const;
    bool IsValidIdentifierChar(char c) const;
    bool IsDigit(char c) const;
    bool IsWhitespace(char c) const;
    bool IsLineBreak(char c) const;
    
    // 多字符操作符
    CHTLTokenPtr ReadMultiCharOperator();
    bool MatchSequence(const std::string& sequence) const;
    
    // 错误处理
    void ReportError(const std::string& message);
    CHTLTokenPtr CreateErrorToken(const std::string& message);
    
    // 创建词法单元
    CHTLTokenPtr CreateToken(CHTLTokenType type, const std::string& value = "");
    CHTLTokenPtr CreateToken(CHTLTokenType type, char value);
};

} // namespace CHTL