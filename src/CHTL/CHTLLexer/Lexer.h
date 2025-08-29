#pragma once
#include "Token.h"
#include "Type.h"
#include "GlobalMap.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

/**
 * 词法分析器状态
 */
enum class LexerState {
    NORMAL,                     // 正常状态
    IN_STRING_DOUBLE,           // 在双引号字符串中
    IN_STRING_SINGLE,           // 在单引号字符串中
    IN_LINE_COMMENT,            // 在行注释中
    IN_BLOCK_COMMENT,           // 在块注释中
    IN_GENERATOR_COMMENT,       // 在生成器注释中
    IN_BRACKET_EXPRESSION,      // 在方括号表达式中
    IN_AT_EXPRESSION,           // 在@表达式中
    IN_CSS_SELECTOR,            // 在CSS选择器中
    IN_INDEX_ACCESS,            // 在索引访问中
    ERROR_STATE                 // 错误状态
};

/**
 * 词法分析错误类型
 */
enum class LexerError {
    NONE,                       // 无错误
    UNTERMINATED_STRING,        // 未终止的字符串
    UNTERMINATED_COMMENT,       // 未终止的注释
    INVALID_CHARACTER,          // 无效字符
    INVALID_ESCAPE_SEQUENCE,    // 无效转义序列
    MALFORMED_TOKEN,            // 格式错误的token
    UNEXPECTED_EOF,             // 意外的文件结束
    INVALID_SELECTOR,           // 无效的选择器
    INVALID_AT_EXPRESSION       // 无效的@表达式
};

/**
 * 词法分析错误信息
 */
struct LexerErrorInfo {
    LexerError type;
    std::string message;
    Position position;
    
    LexerErrorInfo(LexerError t = LexerError::NONE, 
                   const std::string& msg = "", 
                   const Position& pos = Position())
        : type(t), message(msg), position(pos) {}
};

/**
 * CHTL词法分析器
 * 严格按照CHTL语法文档实现
 */
class Lexer {
public:
    /**
     * 构造函数
     * @param source 源代码字符串
     */
    explicit Lexer(const std::string& source);
    
    /**
     * 析构函数
     */
    ~Lexer() = default;
    
    // 禁用拷贝和移动
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(Lexer&&) = delete;
    
    /**
     * 分析所有token
     * @return token序列
     */
    TokenSequence tokenize();
    
    /**
     * 获取下一个token
     * @return 下一个token
     */
    Token nextToken();
    
    /**
     * 查看下一个token但不移动位置
     * @return 下一个token
     */
    Token peekToken();
    
    /**
     * 检查是否已到达源码末尾
     * @return true如果已到达末尾
     */
    bool isAtEnd() const;
    
    /**
     * 获取当前位置
     * @return 当前位置信息
     */
    Position getCurrentPosition() const;
    
    /**
     * 获取错误信息
     * @return 错误信息列表
     */
    const std::vector<LexerErrorInfo>& getErrors() const;
    
    /**
     * 检查是否有错误
     * @return true如果有错误
     */
    bool hasErrors() const;
    
    /**
     * 重置词法分析器
     * @param source 新的源代码
     */
    void reset(const std::string& source);
    
    /**
     * 设置错误处理回调
     * @param callback 错误处理函数
     */
    void setErrorCallback(std::function<void(const LexerErrorInfo&)> callback);

private:
    // ========== 私有成员变量 ==========
    
    std::string m_source;               // 源代码
    size_t m_current;                   // 当前字符位置
    Position m_position;                // 当前位置信息
    LexerState m_state;                 // 当前状态
    std::vector<LexerErrorInfo> m_errors; // 错误列表
    std::function<void(const LexerErrorInfo&)> m_errorCallback; // 错误回调
    
    // ========== 私有辅助方法 ==========
    
    /**
     * 获取当前字符
     */
    char currentChar() const;
    
    /**
     * 获取下一个字符但不移动位置
     */
    char peekChar(size_t offset = 1) const;
    
    /**
     * 移动到下一个字符
     */
    void advance();
    
    /**
     * 跳过空白字符
     */
    void skipWhitespace();
    
    /**
     * 匹配指定字符
     */
    bool match(char expected);
    
    /**
     * 匹配字符串
     */
    bool match(const std::string& expected);
    
    /**
     * 检查字符类型
     */
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    bool isIdentifierStart(char c) const;
    bool isIdentifierChar(char c) const;
    bool isWhitespace(char c) const;
    bool isNewline(char c) const;
    
    /**
     * 创建token
     */
    Token makeToken(TokenType type, const std::string& value);
    Token makeToken(TokenType type, const std::string& value, const Position& startPos);
    
    /**
     * 报告错误
     */
    void reportError(LexerError type, const std::string& message);
    void reportError(LexerError type, const std::string& message, const Position& pos);
    
    // ========== Token扫描方法 ==========
    
    /**
     * 扫描字符串
     */
    Token scanString(char quote);
    
    /**
     * 扫描无修饰字面量
     */
    Token scanUnquotedLiteral();
    
    /**
     * 扫描标识符
     */
    Token scanIdentifier();
    
    /**
     * 扫描数字
     */
    Token scanNumber();
    
    /**
     * 扫描注释
     */
    Token scanLineComment();
    Token scanBlockComment();
    Token scanGeneratorComment();
    
    /**
     * 扫描方括号表达式
     */
    Token scanBracketExpression();
    
    /**
     * 扫描@表达式
     */
    Token scanAtExpression();
    
    /**
     * 扫描CSS选择器
     */
    Token scanCssSelector(char prefix);
    
    /**
     * 扫描索引访问
     */
    Token scanIndexAccess();
    
    // ========== Token类型判断方法 ==========
    
    /**
     * 判断标识符的token类型
     */
    TokenType getIdentifierType(const std::string& identifier);
    
    /**
     * 判断是否为关键字
     */
    bool isKeyword(const std::string& word);
    
    /**
     * 判断是否为HTML元素
     */
    bool isHtmlElement(const std::string& element);
    
    /**
     * 验证选择器格式
     */
    bool isValidSelector(const std::string& selector, char prefix);
    
    /**
     * 验证@表达式格式
     */
    bool isValidAtExpression(const std::string& expression);
    
    /**
     * 处理转义字符
     */
    std::string processEscapeSequences(const std::string& str);
    
    /**
     * 更新位置信息
     */
    void updatePosition(char c);
};

} // namespace CHTL