#pragma once

#include "../../Util/Common.h"
#include "Token.h"
#include "GlobalMap.h"
#include <vector>
#include <queue>

namespace CHTL {
namespace JS {

// CHTL JS词法分析器状态
enum class CHTLJSLexerState {
    NORMAL,             // 正常状态
    IN_STRING,          // 字符串中
    IN_COMMENT_SINGLE,  // 单行注释中
    IN_COMMENT_MULTI,   // 多行注释中
    IN_SELECTOR,        // 选择器中 {{...}}
    IN_LISTEN_BLOCK,    // listen块中
    IN_DELEGATE_BLOCK,  // delegate块中
    IN_ANIMATE_BLOCK,   // animate块中
    IN_VIR_BLOCK,       // vir块中
    IN_MODULE_BLOCK,    // module块中
    IN_CJMOD_BLOCK,     // CJMOD扩展块中
    ERROR_STATE         // 错误状态
};

// CHTL JS词法分析器配置
struct CHTLJSLexerConfig {
    bool skipWhitespace = true;
    bool skipComments = false;
    bool trackPositions = true;
    bool enableCHTLJSExtensions = true;
    bool enableCJMODSupport = true;
    bool strictMode = false;
    bool debugMode = false;
    size_t maxTokens = 100000;
    StringVector customKeywords;
    StringVector ignorePatterns;
};

// CHTL JS词法分析器错误信息
struct CHTLJSLexerError {
    String message;
    Position position;
    String tokenValue;
    CHTLJSLexerState state;
    
    CHTLJSLexerError(const String& msg, const Position& pos, const String& token = "", 
                     CHTLJSLexerState st = CHTLJSLexerState::ERROR_STATE)
        : message(msg), position(pos), tokenValue(token), state(st) {}
};

// CHTL JS词法分析器主类
class CHTLJSLexer {
public:
    // 构造函数
    explicit CHTLJSLexer(const String& source, const CHTLJSLexerConfig& config = CHTLJSLexerConfig{});
    
    // 主要接口
    std::vector<CHTLJSToken> tokenize();
    CHTLJSToken nextToken();
    CHTLJSToken peekToken(size_t offset = 0);
    bool hasMoreTokens() const;
    
    // 状态查询
    Position getCurrentPosition() const;
    CHTLJSLexerState getCurrentState() const;
    bool isAtEnd() const;
    size_t getRemainingLength() const;
    
    // 错误处理
    const std::vector<CHTLJSLexerError>& getErrors() const;
    bool hasErrors() const;
    String getErrorSummary() const;
    void clearErrors();
    
    // 高级功能
    std::vector<CHTLJSToken> tokenizeWithContext();
    StringVector extractSelectors();
    StringVector extractEventBindings();
    StringVector extractModules();
    
    // 工具方法
    void reset();
    void setState(CHTLJSLexerState newState);
    String getCurrentLine() const;
    String getSourceSubstring(size_t start, size_t length) const;
    
private:
    // 成员变量
    String sourceCode;
    CHTLJSLexerConfig config;
    size_t currentPosition;
    size_t currentLine;
    size_t currentColumn;
    CHTLJSLexerState currentState;
    std::vector<CHTLJSLexerError> errors;
    std::queue<CHTLJSToken> tokenBuffer;
    
    // 状态栈（用于嵌套块）
    std::vector<CHTLJSLexerState> stateStack;
    
    // 核心词法分析方法
    CHTLJSToken scanNextToken();
    CHTLJSToken scanIdentifierOrKeyword();
    CHTLJSToken scanNumber();
    CHTLJSToken scanString();
    CHTLJSToken scanOperator();
    CHTLJSToken scanComment();
    
    // CHTL JS特殊语法扫描
    CHTLJSToken scanSelector();
    CHTLJSToken scanEventBinding();
    CHTLJSToken scanListenBlock();
    CHTLJSToken scanDelegateBlock();
    CHTLJSToken scanAnimateBlock();
    CHTLJSToken scanVirBlock();
    CHTLJSToken scanModuleBlock();
    
    // CJMOD扩展支持
    CHTLJSToken scanCJMODBlock();
    bool isCJMODKeyword(const String& word);
    
    // 辅助方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance(size_t count = 1);
    void skipWhitespace();
    void skipLine();
    bool isAtLineEnd() const;
    bool matchString(const String& str);
    bool matchPattern(const String& pattern);
    
    // 字符分类
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 状态管理
    void pushState(CHTLJSLexerState newState);
    void popState();
    void enterBlockState(CHTLJSLexerState blockState);
    void exitBlockState();
    
    // 错误处理
    void addError(const String& message, const String& token = "");
    void addWarning(const String& message, const String& token = "");
    CHTLJSToken createErrorToken(const String& message);
    
    // Token创建
    CHTLJSToken createToken(CHTLJSTokenType type, const String& value, size_t length = 0);
    CHTLJSToken createToken(CHTLJSTokenType type, char c);
    Position getCurrentTokenPosition() const;
    
    // 特殊语法解析
    String parseSelector();
    String parseEventBinding();
    String parseListenContent();
    String parseDelegateContent();
    String parseAnimateContent();
    String parseVirContent();
    String parseModuleContent();
    
    // 嵌套括号处理
    String parseBalancedBraces();
    String parseBalancedParens();
    String parseBalancedBrackets();
    int countNestingLevel(char open, char close);
    
    // 验证方法
    bool validateSelector(const String& selector);
    bool validateEventName(const String& eventName);
    bool validateCHTLJSFunction(const String& functionName);
    
    // 优化方法
    void optimizeTokens(std::vector<CHTLJSToken>& tokens);
    void removeDuplicateWhitespace(std::vector<CHTLJSToken>& tokens);
    void mergeStringLiterals(std::vector<CHTLJSToken>& tokens);
};

// CHTL JS词法分析器工厂类
class CHTLJSLexerFactory {
public:
    static std::unique_ptr<CHTLJSLexer> createLexer(const String& source, 
                                                    const CHTLJSLexerConfig& config = CHTLJSLexerConfig{});
    static std::unique_ptr<CHTLJSLexer> createStrictLexer(const String& source);
    static std::unique_ptr<CHTLJSLexer> createDebugLexer(const String& source);
    static std::unique_ptr<CHTLJSLexer> createCJMODLexer(const String& source);
    
    // 配置预设
    static CHTLJSLexerConfig getDefaultConfig();
    static CHTLJSLexerConfig getStrictConfig();
    static CHTLJSLexerConfig getDebugConfig();
    static CHTLJSLexerConfig getCJMODConfig();
};

} // namespace JS
} // namespace CHTL