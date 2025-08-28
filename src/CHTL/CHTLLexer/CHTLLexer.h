#pragma once

#include "CHTLToken.h"
#include "CHTLGlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <fstream>

namespace CHTL {

/**
 * CHTL词法分析器
 * 严格根据CHTL语法文档实现，不私自扩展
 */
class CHTLLexer {
private:
    // 输入流
    std::istream* inputStream;
    std::ifstream fileStream;
    bool isFileInput;
    
    // 当前扫描位置
    size_t currentLine;
    size_t currentColumn;
    size_t currentPosition;
    
    // 当前字符
    char currentChar;
    char nextChar;
    
    // 全局状态表
    std::shared_ptr<CHTLGlobalMap> globalMap;
    
    // Token缓冲区
    std::vector<CHTLToken> tokenBuffer;
    size_t currentTokenIndex;
    
    // 扫描状态
    bool isInString;
    bool isInComment;
    bool isInGeneratorComment;
    bool isInBracket;
    bool isInParen;
    bool isInBrace;
    
    // 字符串类型
    char stringDelimiter;
    
    // 注释类型
    bool isMultiLineComment;
    int commentNestLevel;

public:
    CHTLLexer();
    explicit CHTLLexer(const std::string& filename);
    explicit CHTLLexer(std::istream& stream);
    ~CHTLLexer();
    
    // 初始化方法
    void initialize(const std::string& filename);
    void initialize(std::istream& stream);
    void setGlobalMap(std::shared_ptr<CHTLGlobalMap> map);
    
    // 主要扫描方法
    CHTLToken getNextToken();
    std::vector<CHTLToken> getAllTokens();
    void reset();
    
    // 状态查询
    bool hasMoreTokens() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    size_t getCurrentPosition() const;
    
    // 调试方法
    void printTokens() const;
    void printCurrentState() const;

private:
    // 字符读取方法
    void advance();
    char peek();
    char peekNext();
    bool isEOF() const;
    
    // 空白字符处理
    void skipWhitespace();
    void skipNewline();
    
    // 注释处理
    CHTLToken handleSingleLineComment();
    CHTLToken handleMultiLineComment();
    CHTLToken handleGeneratorComment();
    
    // 字符串处理
    CHTLToken handleString();
    CHTLToken handleTextLiteral();
    
    // 数字处理
    CHTLToken handleNumber();
    
    // 标识符和关键字处理
    CHTLToken handleIdentifier();
    CHTLToken handleKeyword(const std::string& text);
    CHTLToken handleTypeIdentifier(const std::string& text);
    
    // 标点符号处理
    CHTLToken handlePunctuation();
    CHTLToken handleOperator();
    
    // 特殊符号处理
    CHTLToken handleSpecialSymbol();
    
    // 选择器处理
    CHTLToken handleCSSSelector();
    CHTLToken handlePseudoClass();
    CHTLToken handlePseudoElement();
    
    // 配置相关处理
    CHTLToken handleConfigKey();
    CHTLToken handleConfigValue();
    
    // 导入相关处理
    CHTLToken handleImportPath();
    CHTLToken handleModuleName();
    
    // 命名空间相关处理
    CHTLToken handleNamespaceName();
    
    // 模板相关处理
    CHTLToken handleTemplateName();
    CHTLToken handleTemplateParameter();
    
    // 自定义相关处理
    CHTLToken handleCustomName();
    CHTLToken handleCustomParameter();
    
    // 原始嵌入相关处理
    CHTLToken handleOriginType();
    CHTLToken handleOriginName();
    
    // 样式组相关处理
    CHTLToken handleStyleGroupName();
    CHTLToken handleStyleProperty();
    CHTLToken handleStyleValue();
    
    // 元素相关处理
    CHTLToken handleElementName();
    CHTLToken handleElementAttribute();
    CHTLToken handleElementAttributeValue();
    
    // 辅助方法
    bool isLetter(char c) const;
    bool isDigit(char c) const;
    bool isHexDigit(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isPunctuation(char c) const;
    bool isOperator(char c) const;
    bool isSpecialSymbol(char c) const;
    
    // 关键字识别
    bool isKeyword(const std::string& text) const;
    bool isTypeIdentifier(const std::string& text) const;
    bool isOperatorKeyword(const std::string& text) const;
    
    // 错误处理
    void reportError(const std::string& message);
    CHTLToken createErrorToken(const std::string& message);
    
    // 位置管理
    void updatePosition();
    void resetPosition();
    
    // 缓冲区管理
    void addToBuffer(const CHTLToken& token);
    CHTLToken getFromBuffer();
    void clearBuffer();
    bool hasBufferedTokens() const;
};

} // namespace CHTL