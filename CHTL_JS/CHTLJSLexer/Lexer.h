#pragma once
#include "Token.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL_JS {

/**
 * CHTL JS词法分析器
 * 负责将CHTL JS源代码转换为Token序列
 * 支持CHTL JS特有的增强选择器、箭头操作符等
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
    
    // CHTL JS特有的Token识别
    Token readEnhancedSelector();      // {{选择器}}
    Token readArrow();                 // ->
    Token readClassSelector();         // .className
    Token readIdSelector();            // #idName
    
    // 符号处理
    Token readSymbol();
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierChar(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isWhitespace(char c) const;
    bool isLineEnding(char c) const;
    
    // 关键字识别
    TokenType classifyIdentifier(const std::string& text) const;
    
    // 位置管理
    Position makePosition() const;
    void updatePosition(char c);
    
    // 上下文相关词法分析
    bool isInLocalScriptBlock() const;
    bool isInListenBlock() const;
    bool isInDelegateBlock() const;
    bool isInAnimateBlock() const;
    
    // 状态标记
    struct LexerState {
        bool inLocalScriptBlock = false;
        bool inListenBlock = false;
        bool inDelegateBlock = false;
        bool inAnimateBlock = false;
        bool inVirBlock = false;
        size_t braceDepth = 0;
        size_t bracketDepth = 0;
        size_t parenDepth = 0;
    } state_;
};

} // namespace CHTL_JS