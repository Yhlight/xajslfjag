#ifndef CHTL_LEXER_H
#define CHTL_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <istream>
#include <queue>
#include "CHTLToken.h"

namespace CHTL {

// 词法分析器模式
enum class LexerMode {
    NORMAL,          // 正常模式
    IN_STRING,       // 字符串内
    IN_COMMENT,      // 注释内
    IN_STYLE,        // style块内
    IN_SCRIPT,       // script块内
    IN_CHTL_JS,      // CHTL JS语法内
    IN_UNQUOTED      // 无引号字面量内
};

class CHTLLexer {
private:
    // 输入源
    std::string input;
    size_t position;
    size_t length;
    
    // 位置跟踪
    int line;
    int column;
    int token_start_line;
    int token_start_column;
    
    // 模式栈
    std::vector<LexerMode> mode_stack;
    LexerMode current_mode;
    
    // Token缓冲
    std::queue<Token> token_buffer;
    
    // 错误信息
    std::vector<std::string> errors;
    
    // 配置
    bool skip_whitespace;
    bool skip_comments;
    bool preserve_newlines;

private:
    // 字符操作
    char peek(int offset = 0) const;
    char advance();
    bool match(char expected);
    bool match(const std::string& expected);
    bool isAtEnd() const;
    void skipWhitespace();
    void updatePosition(char c);
    
    // 模式管理
    void pushMode(LexerMode mode);
    void popMode();
    
    // Token生成
    Token makeToken(TokenType type, const std::string& value = "");
    Token makeTokenFromCurrent(TokenType type);
    
    // 扫描方法
    Token scanToken();
    Token scanIdentifier();
    Token scanNumber();
    Token scanString(char quote);
    Token scanUnquotedLiteral();
    Token scanComment();
    Token scanGeneratorComment();
    Token scanOperator();
    Token scanTemplateVar();
    Token scanCustomVar();
    Token scanSelector();
    Token scanCHTLJS();
    Token scanImportDirective();
    
    // 辅助方法
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isWhitespace(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierPart(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isOperatorChar(char c) const;
    
    // 关键字识别
    TokenType checkKeyword(const std::string& text) const;
    TokenType checkHTMLTag(const std::string& text) const;
    TokenType checkImportType(const std::string& text) const;
    
    // 特殊语法识别
    bool isCHTLJSStart() const;
    bool isTemplateVarStart() const;
    bool isCustomVarStart() const;
    bool isSelectorStart() const;
    bool isImportStart() const;
    bool isConfigurationStart() const;
    bool isOriginStart() const;
    
    // 错误处理
    void addError(const std::string& message);
    Token errorToken(const std::string& message);

public:
    CHTLLexer();
    explicit CHTLLexer(const std::string& source);
    ~CHTLLexer() = default;
    
    // 初始化
    void setInput(const std::string& source);
    void setInput(std::istream& stream);
    void reset();
    
    // Token获取
    Token nextToken();
    Token peekToken(int lookahead = 0);
    std::vector<Token> scanAllTokens();
    
    // 配置
    void setSkipWhitespace(bool skip) { skip_whitespace = skip; }
    void setSkipComments(bool skip) { skip_comments = skip; }
    void setPreserveNewlines(bool preserve) { preserve_newlines = preserve; }
    
    // 状态查询
    int getCurrentLine() const { return line; }
    int getCurrentColumn() const { return column; }
    size_t getCurrentPosition() const { return position; }
    LexerMode getCurrentMode() const { return current_mode; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    std::vector<std::string> getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 调试
    void dumpTokens(const std::vector<Token>& tokens) const;
    std::string getModeString() const;
};

} // namespace CHTL

#endif // CHTL_LEXER_H