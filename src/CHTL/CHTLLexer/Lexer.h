#pragma once

#include "../../Util/Common.h"
#include "Token.h"
#include "GlobalMap.h"

namespace CHTL {

// 词法分析器状态
enum class LexerState {
    NORMAL,               // 正常状态
    IN_STRING,            // 字符串内部
    IN_COMMENT_LINE,      // 行注释内部
    IN_COMMENT_BLOCK,     // 块注释内部
    IN_STYLE_BLOCK,       // style块内部
    IN_SCRIPT_BLOCK,      // script块内部
    IN_TEMPLATE_BLOCK,    // 模板块内部
    IN_CUSTOM_BLOCK,      // 自定义块内部
    IN_ORIGIN_BLOCK,      // 原始嵌入块内部
    IN_IMPORT_STATEMENT,  // import语句内部
    IN_CONFIG_BLOCK,      // 配置块内部
    IN_NAMESPACE_BLOCK,   // 命名空间块内部
    IN_SELECTOR,          // CSS选择器内部
    IN_CHTLJS_SELECTOR,   // CHTL JS选择器内部 {{...}}
    IN_BRACKET_EXPR,      // 方括号表达式内部
    IN_ATTRIBUTE_VALUE,   // 属性值内部
    IN_PSEUDO_CLASS,      // 伪类内部
    IN_PSEUDO_ELEMENT     // 伪元素内部
};

// 词法分析器配置
struct LexerConfig {
    bool skipWhitespace = false;
    bool skipComments = false;
    bool trackPositions = true;
    bool enableCHTLJS = true;
    bool enableUnquotedLiterals = true;
    bool enableAutoSemicolon = true;
};

// 词法分析器
class Lexer {
public:
    explicit Lexer(const String& source, const LexerConfig& config = LexerConfig{});
    ~Lexer() = default;
    
    // 基础功能
    Token nextToken();
    Token peekToken(size_t lookahead = 1);
    bool hasMoreTokens() const;
    void reset();
    
    // 状态管理
    LexerState getState() const { return state; }
    void setState(LexerState newState) { state = newState; }
    Position getCurrentPosition() const { return position; }
    
    // 配置
    void setConfig(const LexerConfig& config) { this->config = config; }
    const LexerConfig& getConfig() const { return config; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    StringVector getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 工具方法
    String getSourceSubstring(size_t start, size_t length) const;
    String getCurrentLine() const;
    void skipToNextLine();
    void skipToEndOfBlock();
    
private:
    String source;
    size_t current;
    Position position;
    LexerState state;
    LexerConfig config;
    StringVector errors;
    std::stack<LexerState> stateStack;
    
    // 内部解析方法
    Token scanToken();
    Token scanIdentifier();
    Token scanString(char quote);
    Token scanNumber();
    Token scanUnquotedLiteral();
    Token scanComment();
    Token scanOperator();
    Token scanSelector();
    Token scanCHTLJSSelector();
    Token scanBracketExpression();
    Token scanPseudoClass();
    Token scanPseudoElement();
    
    // 工具方法
    char advance();
    char peek(size_t offset = 0) const;
    bool isAtEnd() const;
    bool match(char expected);
    bool match(const String& expected);
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    
    // 字符检查
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    // 状态管理
    void pushState(LexerState newState);
    void popState();
    
    // 错误报告
    void reportError(const String& message);
    void reportUnexpectedCharacter(char c);
    void reportUnterminatedString();
    void reportUnterminatedComment();
    
    // 特殊语法处理
    Token handleTemplateKeyword();
    Token handleCustomKeyword();
    Token handleOriginKeyword();
    Token handleImportKeyword();
    Token handleConfigKeyword();
    Token handleNamespaceKeyword();
    
    // HTML和CSS特殊处理
    Token handleStyleBlock();
    Token handleScriptBlock();
    Token handleHTMLElement();
    
    // CHTL JS特殊处理
    Token handleCHTLJSFunction();
    Token handleEventBinding();
    Token handleVirtualObject();
    
    // 位置更新
    void updatePosition(char c);
    Position makePosition() const;
};

// 词法分析器工厂
class LexerFactory {
public:
    static std::unique_ptr<Lexer> createCHTLLexer(const String& source);
    static std::unique_ptr<Lexer> createCHTLJSLexer(const String& source);
    static std::unique_ptr<Lexer> createStyleLexer(const String& source);
    static std::unique_ptr<Lexer> createScriptLexer(const String& source);
};

} // namespace CHTL