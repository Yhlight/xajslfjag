#pragma once

#include "CHTLToken.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
namespace Token {

// 词法分析器状态
enum class LexerState {
    NORMAL,              // 正常状态
    IN_STRING,           // 在字符串中
    IN_SINGLE_STRING,    // 在单引号字符串中
    IN_COMMENT,          // 在注释中
    IN_MULTI_COMMENT,    // 在多行注释中
    IN_GENERATOR_COMMENT,// 在生成器注释中
    IN_KEYWORD_BLOCK,    // 在关键字块中
    IN_TYPE_IDENTIFIER,  // 在类型标识符中
    IN_UNQUOTED_LITERAL, // 在无修饰字面量中
    IN_NUMBER,           // 在数字中
    ERROR_STATE          // 错误状态
};

// CHTL词法分析器
class CHTLLexer {
public:
    CHTLLexer();
    explicit CHTLLexer(const std::string& source);
    ~CHTLLexer();
    
    // 设置源代码
    void setSource(const std::string& source);
    
    // 获取下一个Token
    CHTLToken nextToken();
    
    // 查看下一个Token但不消费
    CHTLToken peekToken();
    
    // 检查是否到达文件结尾
    bool isAtEnd() const;
    
    // 获取当前位置信息
    size_t getCurrentLine() const { return current_line; }
    size_t getCurrentColumn() const { return current_column; }
    size_t getCurrentPosition() const { return current_pos; }
    
    // 错误处理
    bool hasErrors() const { return !errors.empty(); }
    std::vector<std::string> getErrors() const { return errors; }
    void clearErrors() { errors.clear(); }
    
    // 重置词法分析器
    void reset();
    
    // 设置调试模式
    void setDebugMode(bool debug) { debug_mode = debug; }
    
    // 获取所有Token（用于调试）
    std::vector<CHTLToken> tokenizeAll();

private:
    std::string source;
    size_t current_pos;
    size_t current_line;
    size_t current_column;
    LexerState state;
    std::vector<std::string> errors;
    bool debug_mode;
    
    // 预读缓存
    bool has_peeked;
    CHTLToken peeked_token;
    
    // 内部辅助方法
    char peek(size_t offset = 0) const;
    char advance();
    void skipWhitespace();
    void skipToEndOfLine();
    
    // Token识别方法
    CHTLToken makeToken(TokenType type, const std::string& value = "");
    CHTLToken makeErrorToken(const std::string& message);
    
    // 字符串处理
    CHTLToken scanString(char quote);
    CHTLToken scanUnquotedLiteral();
    CHTLToken scanNumber();
    CHTLToken scanIdentifier();
    
    // 注释处理
    CHTLToken scanSingleComment();
    CHTLToken scanMultiComment();
    CHTLToken scanGeneratorComment();
    
    // 关键字块处理
    CHTLToken scanKeywordBlock();
    CHTLToken scanTypeIdentifier();
    
    // 符号处理
    CHTLToken scanSymbol();
    CHTLToken scanArrow();  // ->
    
    // 关键字识别
    TokenType identifyKeyword(const std::string& text) const;
    TokenType identifyTypeIdentifier(const std::string& text) const;
    TokenType identifyKeywordBlock(const std::string& text) const;
    
    // 字符判断
    bool isAtEnd(size_t pos) const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isUnquotedLiteralChar(char c) const;
    bool isValidIdentifierChar(char c) const;
    
    // 错误处理
    void addError(const std::string& message);
    
    // 位置更新
    void updatePosition(char c);
    
    // 调试输出
    void debugLog(const std::string& message) const;
};

} // namespace Token
} // namespace CHTL