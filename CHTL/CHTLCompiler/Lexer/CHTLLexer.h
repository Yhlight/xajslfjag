#pragma once
#include "CHTLTokens.h"
#include <string>
#include <vector>

namespace CHTL {

// CHTL词法分析器状态
enum class CHTLLexerState {
    Normal,                  // 正常状态
    InStringLiteral,         // 字符串字面量中
    InNumber,                // 数字中
    InIdentifier,            // 标识符中
    InBracketKeyword,        // 方括号关键字中 [Template], [Custom] 等
    InTypeIdentifier,        // 类型标识符中 @Style, @Element 等
    InSelector,              // 选择器中 .class, #id
    InLineComment,           // 行注释中 //
    InBlockComment,          // 块注释中 /* */
    InGeneratorComment,      // 生成器注释中 --
    InEnhancedSelector,      // 增强选择器中 {{selector}}
    InOriginContent,         // 原始嵌入内容中（直接跳过词法分析）
};

// CHTL词法分析器
class CHTLLexer {
private:
    // 源码信息
    std::string source;
    size_t position;
    size_t line;
    size_t column;
    size_t start;            // 当前Token开始位置
    
    // 错误信息
    std::vector<std::string> errors;
    
    // 状态管理
    CHTLLexerState currentState;
    bool inOriginBlock;      // 是否在原始嵌入块中
    size_t originBraceCount; // 原始嵌入块中的花括号计数
    bool expectingOriginContent; // 是否期待原始嵌入内容（在[Origin]类型标识符后）
    CHTLTokenType lastTokenType; // 最后解析的Token类型（用于Origin检测）
    
public:
    explicit CHTLLexer(const std::string& sourceCode);
    
    // 主要接口
    std::vector<CHTLToken> tokenize();
    void reset(const std::string& newSource = "");
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
private:
    // === 核心扫描方法 ===
    CHTLToken nextToken();
    CHTLToken scanToken();
    
    // === 特定Token扫描 ===
    CHTLToken scanStringLiteral(char quote);      // 扫描字符串 "..." '...'
    CHTLToken scanUnquotedLiteral();              // 扫描无引号字面量
    CHTLToken scanNumber();                       // 扫描数字
    CHTLToken scanIdentifier();                   // 扫描标识符和关键字
    CHTLToken scanBracketKeyword();               // 扫描方括号关键字 [Template]
    CHTLToken scanTypeIdentifier();               // 扫描类型标识符 @Style
    CHTLToken scanSelector();                     // 扫描CSS选择器 .class #id
    CHTLToken scanEnhancedSelector();             // 扫描增强选择器 {{selector}}
    
    // === 注释扫描 ===
    CHTLToken scanLineComment();                  // 扫描行注释 //
    CHTLToken scanBlockComment();                 // 扫描块注释 /* */
    CHTLToken scanGeneratorComment();             // 扫描生成器注释 --
    
    // === 原始嵌入扫描 ===
    CHTLToken scanOriginContent();                // 扫描原始嵌入内容（直接跳过）
    CHTLToken scanCompleteOriginBlock(const std::string& prefix); // 扫描完整的原始嵌入块
    
    // === 操作符扫描 ===
    CHTLToken scanArrow();                        // 扫描 -> 操作符
    CHTLToken scanCEEquivalent(char c);           // 扫描CE对等式 : =
    
    // === 字符操作 ===
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    
    // === 字符分类 ===
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isValidIdentifierStart(char c) const;
    bool isValidIdentifierChar(char c) const;
    bool isValidUnquotedChar(char c) const;
    
    // === Token创建 ===
    CHTLToken makeToken(CHTLTokenType type, const std::string& value = "");
    CHTLToken makeErrorToken(const std::string& message);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    
    // === 工具方法 ===
    void skipWhitespace();
    void skipToEndOfLine();
    std::string getCurrentLexeme() const;
    bool checkKeyword(const std::string& keyword) const;
    bool checkBracketKeyword(const std::string& keyword) const;
    
    // === 特殊处理 ===
    CHTLToken handleAtTop();                      // 处理 "at top"
    CHTLToken handleAtBottom();                   // 处理 "at bottom"
    bool isMultiWordKeyword() const;              // 检查多词关键字
    bool isInOriginBlock() const;                 // 检查是否在原始嵌入块中
    void enterOriginBlock();                      // 进入原始嵌入块状态
    void exitOriginBlock();                       // 退出原始嵌入块状态
    bool lastTokenWasOrigin() const;              // 检查最后的Token是否表示将要开始Origin块
};

} // namespace CHTL