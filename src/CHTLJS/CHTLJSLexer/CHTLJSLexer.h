#pragma once

#include "CHTLJSToken.h"
#include "CHTLJSGlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <functional>

namespace CHTLJS {

class CHTLJSLexer {
public:
    explicit CHTLJSLexer(std::shared_ptr<CHTLJSGlobalMap> globalMap = nullptr);
    ~CHTLJSLexer() = default;
    
    // 设置输入源
    void setInput(const std::string& input);
    void setInput(std::istream& input);
    
    // 设置文件名（用于错误报告）
    void setFileName(const std::string& fileName);
    
    // 词法分析
    CHTLJSTokenList tokenize();
    
    // 获取下一个Token
    CHTLJSTokenPtr nextToken();
    
    // 重置状态
    void reset();
    
    // 错误处理
    struct LexerError {
        std::string message;
        size_t line;
        size_t column;
        size_t position;
    };
    
    const std::vector<LexerError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    // 获取当前位置信息
    size_t getCurrentLine() const { return currentLine; }
    size_t getCurrentColumn() const { return currentColumn; }
    size_t getCurrentPosition() const { return currentPosition; }

private:
    std::shared_ptr<CHTLJSGlobalMap> globalMap;
    std::string input;
    std::string fileName;
    size_t currentPosition;
    size_t currentLine;
    size_t currentColumn;
    std::vector<LexerError> errors;
    
    // 当前字符操作
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    bool isAtEnd() const;
    
    // 跳过空白符
    void skipWhitespace();
    
    // 读取各种Token
    CHTLJSTokenPtr readStringLiteral();
    CHTLJSTokenPtr readTemplateLiteral();
    CHTLJSTokenPtr readNumber();
    CHTLJSTokenPtr readIdentifierOrKeyword();
    CHTLJSTokenPtr readSingleLineComment();
    CHTLJSTokenPtr readMultiLineComment();
    CHTLJSTokenPtr readGeneratorComment();
    CHTLJSTokenPtr readSelectorReference();
    CHTLJSTokenPtr readUnquotedLiteral();
    CHTLJSTokenPtr readRegexLiteral();
    
    // 读取操作符
    CHTLJSTokenPtr readOperator();
    
    // 读取CHTL JS特殊语法
    CHTLJSTokenPtr readModuleBlock();
    CHTLJSTokenPtr readListenBlock();
    CHTLJSTokenPtr readAnimateBlock();
    CHTLJSTokenPtr readDelegateBlock();
    
    // 字符分类
    bool isAlpha(char ch) const;
    bool isDigit(char ch) const;
    bool isAlphaNumeric(char ch) const;
    bool isWhitespace(char ch) const;
    bool isNewline(char ch) const;
    bool isHexDigit(char ch) const;
    
    // 错误报告
    void reportError(const std::string& message);
    
    // 创建Token
    CHTLJSTokenPtr createToken(CHTLJSTokenType type, const std::string& text, size_t startLine, size_t startColumn, size_t startPosition);
    
    // 辅助方法
    std::string readUntil(char delimiter);
    std::string readWhile(std::function<bool(char)> predicate);
    bool match(const std::string& str);
    bool matchKeyword(const std::string& keyword);
    
    // 处理无修饰字面量的特殊情况
    bool shouldTreatAsUnquotedLiteral(const std::string& text) const;
};

} // namespace CHTLJS