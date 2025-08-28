#pragma once

#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <functional>

namespace CHTL {

class CHTLLexer {
public:
    explicit CHTLLexer(std::shared_ptr<CHTLGlobalMap> globalMap = nullptr);
    ~CHTLLexer() = default;
    
    // 设置输入源
    void setInput(const std::string& input);
    void setInput(std::istream& input);
    
    // 设置文件名（用于错误报告）
    void setFileName(const std::string& fileName);
    
    // 词法分析
    CHTLTokenList tokenize();
    
    // 获取下一个Token
    CHTLTokenPtr nextToken();
    
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
    std::shared_ptr<CHTLGlobalMap> globalMap;
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
    CHTLTokenPtr readStringLiteral();
    CHTLTokenPtr readNumber();
    CHTLTokenPtr readIdentifierOrKeyword();
    CHTLTokenPtr readSingleLineComment();
    CHTLTokenPtr readMultiLineComment();
    CHTLTokenPtr readGeneratorComment();
    CHTLTokenPtr readSelectorReference();
    CHTLTokenPtr readUnquotedLiteral();
    
    // 读取操作符
    CHTLTokenPtr readOperator();
    
    // 读取标记（如[Template]、[Custom]等）
    CHTLTokenPtr readMarker();
    
    // 读取类型标记（如@Style、@Element等）
    CHTLTokenPtr readTypeMarker();
    
    // 字符分类
    bool isAlpha(char ch) const;
    bool isDigit(char ch) const;
    bool isAlphaNumeric(char ch) const;
    bool isWhitespace(char ch) const;
    bool isNewline(char ch) const;
    
    // 错误报告
    void reportError(const std::string& message);
    
    // 创建Token
    CHTLTokenPtr createToken(CHTLTokenType type, const std::string& text, size_t startLine, size_t startColumn, size_t startPosition);
    
    // 辅助方法
    std::string readUntil(char delimiter);
    std::string readWhile(std::function<bool(char)> predicate);
    bool match(const std::string& str);
    bool matchKeyword(const std::string& keyword);
};

} // namespace CHTL