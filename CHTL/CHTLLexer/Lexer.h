#pragma once
#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL词法分析器
 * 负责将CHTL源代码转换为Token序列
 */
class Lexer {
public:
    explicit Lexer(const std::string& source, std::shared_ptr<GlobalMap> globalMap = nullptr);
    ~Lexer() = default;

    // 获取下一个Token
    Token nextToken();
    
    // 预览下一个Token (不移动位置)
    Token peekToken();
    
    // 预览指定偏移的Token
    Token peekToken(size_t offset);
    
    // 检查是否到达文件末尾
    bool isEOF() const;
    
    // 获取当前位置
    Position getCurrentPosition() const;
    
    // 错误处理
    void reportError(const std::string& message);
    std::vector<std::string> getErrors() const;
    bool hasErrors() const;

private:
    const std::string& source_;
    size_t pos_;
    size_t line_;
    size_t column_;
    std::shared_ptr<GlobalMap> globalMap_;
    std::vector<std::string> errors_;
    
    // 字符处理
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipLine();
    
    // Token识别
    Token readString(char quote);
    Token readNumber();
    Token readIdentifier();
    Token readUnquotedLiteral();
    Token readComment();
    Token readGeneratorComment();
    
    // 符号处理
    Token readSymbol();
    Token readBracketExpression();
    Token readAtExpression();
    
    // 多字符操作符处理
    Token readMultiCharOperator();
    
    // CSS选择器处理
    Token readClassSelector();
    Token readIdSelector();
    Token readPseudoSelector();
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierChar(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isWhitespace(char c) const;
    bool isLineEnding(char c) const;
    
    // 关键字和HTML标签识别
    TokenType classifyIdentifier(const std::string& text) const;
    
    // 位置管理
    Position makePosition() const;
    void updatePosition(char c);
    
    // 预处理相关
    void handlePreprocessor();
    
    // 上下文相关词法分析
    bool isInStyleBlock() const;
    bool isInScriptBlock() const;
    bool isInTemplateBlock() const;
    
    // 状态标记
    struct LexerState {
        bool inStyleBlock = false;
        bool inScriptBlock = false;
        bool inTemplateBlock = false;
        bool inCustomBlock = false;
        bool inOriginBlock = false;
        bool inConfigBlock = false;
        bool inNamespaceBlock = false;
        size_t braceDepth = 0;
        size_t bracketDepth = 0;
    } state_;
};

} // namespace CHTL