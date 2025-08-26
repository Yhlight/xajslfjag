#pragma once
#include "CHTLJSTokens.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

namespace CHTLJS {

// CHTL JS词法分析器状态
enum class CHTLJSLexerState {
    Normal,                      // 正常状态
    InStringLiteral,             // 字符串字面量中
    InTemplateLiteral,           // 模板字面量中
    InTemplateExpression,        // 模板表达式中
    InRegexLiteral,              // 正则表达式中
    InLineComment,               // 行注释中
    InBlockComment,              // 块注释中
    InJSDocComment,              // JSDoc注释中
    InNumber,                    // 数字中
    InIdentifier,                // 标识符中
    InOperator,                  // 操作符中
    InSelector,                  // 选择器语法中
    InAnimation,                 // 动画语法中
    InVirtualObject,             // 虚拟对象语法中
    InEventDelegation            // 事件委托语法中
};

// CHTL JS词法分析器
class CHTLJSLexer {
private:
    std::string source;          // 源代码
    size_t position;             // 当前位置
    size_t line;                 // 当前行号
    size_t column;               // 当前列号
    size_t start;                // 当前Token开始位置
    std::vector<std::string> errors; // 错误信息
    
    CHTLJSLexerState currentState; // 当前状态
    std::vector<CHTLJSLexerState> stateStack; // 状态栈
    
    // 模板字面量嵌套计数
    int templateNestingLevel;
    
    // 正则表达式标志
    bool canBeRegex;
    
public:
    explicit CHTLJSLexer(const std::string& sourceCode);
    
    // 主要方法
    std::vector<CHTLJSToken> tokenize();
    
    // 错误处理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    
    // 重置
    void reset(const std::string& newSource = "");
    
private:
    // === 核心扫描方法 ===
    CHTLJSToken nextToken();
    CHTLJSToken scanToken();
    
    // === 字面量扫描 ===
    CHTLJSToken scanStringLiteral(char quote);
    CHTLJSToken scanTemplateLiteral();
    CHTLJSToken scanTemplateExpression();
    CHTLJSToken scanNumber();
    CHTLJSToken scanRegexLiteral();
    CHTLJSToken scanIdentifier();
    
    // === 操作符扫描 ===
    CHTLJSToken scanOperator();
    CHTLJSToken scanSingleCharOperator(char c);
    CHTLJSToken scanMultiCharOperator();
    
    // === CHTL JS特殊语法扫描 ===
    CHTLJSToken scanCHTLJSKeyword();
    CHTLJSToken scanSelectorSyntax();
    CHTLJSToken scanAnimationSyntax();
    CHTLJSToken scanVirtualObjectSyntax();
    CHTLJSToken scanEventDelegationSyntax();
    
    // === 注释扫描 ===
    CHTLJSToken scanLineComment();
    CHTLJSToken scanBlockComment();
    CHTLJSToken scanJSDocComment();
    
    // === 字符操作 ===
    char peek(size_t offset = 0) const;
    char advance();
    bool isAtEnd() const;
    bool match(char expected);
    bool matchString(const std::string& str);
    
    // === 字符分类 ===
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isDigit(char c) const;
    bool isHexDigit(char c) const;
    bool isBinaryDigit(char c) const;
    bool isOctalDigit(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    
    // === 状态管理 ===
    void pushState(CHTLJSLexerState state);
    void popState();
    CHTLJSLexerState getCurrentState() const;
    
    // === 特殊检查 ===
    bool isOperatorStart(char c) const;
    bool canStartRegex() const;
    bool isKeywordOrIdentifier(const std::string& text) const;
    bool isCHTLJSSpecialSyntax() const;
    
    // === 转义处理 ===
    std::string processEscapeSequences(const std::string& str);
    char processEscapeSequence();
    std::string processUnicodeEscape();
    
    // === 数字处理 ===
    CHTLJSToken scanDecimalNumber();
    CHTLJSToken scanHexNumber();
    CHTLJSToken scanBinaryNumber();
    CHTLJSToken scanOctalNumber();
    CHTLJSToken scanBigIntNumber();
    bool isValidNumberSuffix(char c) const;
    
    // === 模板字面量处理 ===
    void enterTemplateExpression();
    void exitTemplateExpression();
    bool isInTemplateExpression() const;
    
    // === 正则表达式处理 ===
    std::string scanRegexPattern();
    std::string scanRegexFlags();
    bool isValidRegexFlag(char c) const;
    
    // === CHTL JS增强语法检测 ===
    bool isSelectorContext() const;
    bool isAnimationContext() const;
    bool isVirtualObjectContext() const;
    bool isEventDelegationContext() const;
    bool isLifecycleContext() const;
    
    // === 上下文相关词法分析 ===
    CHTLJSToken handleContextualKeyword(const std::string& identifier);
    CHTLJSToken resolveCHTLJSKeyword(const std::string& keyword);
    
    // === 工具方法 ===
    CHTLJSToken makeToken(CHTLJSTokenType type);
    CHTLJSToken makeToken(CHTLJSTokenType type, const std::string& value);
    CHTLJSToken makeErrorToken(const std::string& message);
    std::string getCurrentLexeme() const;
    void skipWhitespace();
    void skipToEndOfLine();
    void addError(const std::string& message);
    
    // === 前瞻方法 ===
    bool checkAhead(const std::string& pattern, size_t offset = 1) const;
    std::string peekAhead(size_t length, size_t offset = 0) const;
    
    // === 验证方法 ===
    bool isValidStringEscape(char c) const;
    bool isValidTemplatePart(const std::string& part) const;
    bool isValidIdentifierName(const std::string& name) const;
    bool isValidNumberFormat(const std::string& number) const;
    
    // === CHTL JS特性检测 ===
    bool isEnhancedSelectorOperator(char c) const;
    bool isAnimationKeywordStart() const;
    bool isVirtualObjectKeywordStart() const;
    bool isEventKeywordStart() const;
    bool isLifecycleKeywordStart() const;
    
    // === 操作符优先级处理 ===
    int getOperatorPrecedence(const std::string& op) const;
    bool isRightAssociative(const std::string& op) const;
    
    // === 错误恢复 ===
    void synchronize();
    void recoverFromError();
    
    // === 静态辅助方法 ===
    static bool isReservedWord(const std::string& word);
    static bool isStrictModeReservedWord(const std::string& word);
    static bool isFutureReservedWord(const std::string& word);
    static const std::unordered_set<std::string>& getReservedWords();
    static const std::unordered_set<std::string>& getStrictModeReservedWords();
    static const std::unordered_set<std::string>& getFutureReservedWords();
    
    // === CHTL JS关键字处理 ===
    static const std::unordered_set<std::string>& getCHTLJSSelectorKeywords();
    static const std::unordered_set<std::string>& getCHTLJSAnimationKeywords();
    static const std::unordered_set<std::string>& getCHTLJSVirtualObjectKeywords();
    static const std::unordered_set<std::string>& getCHTLJSEventKeywords();
    static const std::unordered_set<std::string>& getCHTLJSLifecycleKeywords();
    
    // === 调试方法 ===
    std::string getStateString() const;
    std::string getPositionInfo() const;
    void dumpCurrentState() const;
};

// CHTL JS词法分析器配置
struct CHTLJSLexerConfig {
    bool enableStrictMode = true;        // 启用严格模式
    bool enableCHTLJSExtensions = true;  // 启用CHTL JS扩展
    bool allowReservedWords = false;     // 允许保留字作为标识符
    bool generateComments = true;        // 生成注释Token
    bool generateWhitespace = false;     // 生成空白Token
    bool enableErrorRecovery = true;     // 启用错误恢复
    bool validateIdentifiers = true;     // 验证标识符
    bool validateNumbers = true;         // 验证数字格式
    bool validateStrings = true;         // 验证字符串格式
    std::string sourceFilename;          // 源文件名
};

// 带配置的词法分析器工厂
class CHTLJSLexerFactory {
public:
    static std::unique_ptr<CHTLJSLexer> createLexer(
        const std::string& source,
        const CHTLJSLexerConfig& config = CHTLJSLexerConfig()
    );
    
    static CHTLJSLexerConfig getDefaultConfig();
    static CHTLJSLexerConfig getStrictConfig();
    static CHTLJSLexerConfig getCHTLJSConfig();
};

} // namespace CHTLJS