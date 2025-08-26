#pragma once
#include "CHTLTokens.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// CHTL词法分析器
class CHTLLexer {
private:
    std::string source;           // 源代码
    size_t position;              // 当前位置
    size_t line;                  // 当前行号
    size_t column;                // 当前列号
    std::vector<std::string> errors; // 错误信息
    
    // 词法分析状态
    enum class LexerState {
        Normal,                   // 正常状态
        InString,                 // 字符串内部
        InComment,                // 注释内部
        InPrefix,                 // 前缀内部 [...]
        InTypeIdentifier,         // 类型标识符 @...
        InSelector                // 选择器 .class #id
    };
    
    LexerState currentState;
    
public:
    // 构造函数
    explicit CHTLLexer(const std::string& sourceCode);
    
    // 词法分析主函数
    std::vector<CHTLToken> tokenize();
    
    // 获取错误信息
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    // 重置词法分析器
    void reset(const std::string& newSource = "");
    
private:
    // 核心词法分析方法
    CHTLToken nextToken();
    
    // 字符检查方法
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    
    // 跳过空白字符和换行
    void skipWhitespace();
    void skipToNextLine();
    
    // 具体Token扫描方法
    CHTLToken scanStringLiteral(char quote);
    CHTLToken scanNumber();
    CHTLToken scanIdentifier();
    CHTLToken scanUnquotedLiteral();
    
    // CHTL特有的扫描方法
    CHTLToken scanPrefix();           // [Template], [Custom] 等
    CHTLToken scanTypeIdentifier();   // @Style, @Element 等
    CHTLToken scanSelector();         // .class, #id
    
    // 注释扫描方法
    CHTLToken scanLineComment();      // //
    CHTLToken scanBlockComment();     // /* */
    CHTLToken scanGeneratorComment(); // --
    
    // 多字符操作符扫描
    CHTLToken scanMultiCharOperator();
    
    // 工具方法
    CHTLToken makeToken(CHTLTokenType type, const std::string& value = "");
    CHTLToken makeErrorToken(const std::string& message);
    void addError(const std::string& message);
    
    // CHTL特有的识别方法
    bool isValidIdentifierStart(char c) const;
    bool isValidIdentifierChar(char c) const;
    bool isValidUnquotedChar(char c) const;
    
    // 前缀和类型标识符的完整扫描
    std::string scanCompletePrefix();
    std::string scanCompleteTypeIdentifier();
    
    // 处理CE对等式（冒号和等号等价）
    CHTLToken handleCEEquivalent(char c);
};

} // namespace CHTL