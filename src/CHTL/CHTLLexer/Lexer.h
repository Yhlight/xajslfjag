#pragma once
#include "Token.h"
#include "Type.h"
#include "GlobalMap.h"
#include "../CHTLState/State.h"
#include "../CHTLContext/Context.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 词法分析器内部状态（简化版）
 */
enum class LexerInternalState {
    NORMAL,                     // 正常状态
    IN_STRING_DOUBLE,           // 在双引号字符串中
    IN_STRING_SINGLE,           // 在单引号字符串中
    IN_LINE_COMMENT,            // 在行注释中
    IN_BLOCK_COMMENT,           // 在块注释中
    IN_GENERATOR_COMMENT,       // 在生成器注释中
    IN_BRACKET_EXPRESSION,      // 在方括号表达式中
    IN_AT_EXPRESSION,           // 在@表达式中
    IN_CSS_SELECTOR,            // 在CSS选择器中
    IN_INDEX_ACCESS             // 在索引访问中
};

/**
 * CHTL词法分析器
 * 严格按照CHTL语法文档实现，使用外部状态机和上下文
 */
class Lexer {
public:
    /**
     * 构造函数
     * @param source 源代码字符串
     * @param stateMachine 外部状态机（可选）
     * @param context 外部上下文（可选）
     * @param errorReporter 错误报告器（可选）
     */
    explicit Lexer(const std::string& source,
                   CHTLStateMachine* stateMachine = nullptr,
                   CHTLContext* context = nullptr,
                   ErrorReporter* errorReporter = nullptr);
    
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
     * 重置词法分析器
     * @param source 新的源代码
     */
    void reset(const std::string& source);
    
    /**
     * 设置状态机
     */
    void setStateMachine(CHTLStateMachine* stateMachine);
    
    /**
     * 设置上下文
     */
    void setContext(CHTLContext* context);
    
    /**
     * 设置错误报告器
     */
    void setErrorReporter(ErrorReporter* errorReporter);

private:
    // ========== 私有成员变量 ==========
    
    std::string m_source;               // 源代码
    size_t m_current;                   // 当前字符位置
    Position m_position;                // 当前位置信息
    LexerInternalState m_internalState; // 内部状态
    
    // 外部组件
    CHTLStateMachine* m_stateMachine;   // 外部状态机
    CHTLContext* m_context;             // 外部上下文
    ErrorReporter* m_errorReporter;     // 错误报告器
    
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
    void reportError(ErrorType type, const std::string& message);
    void reportError(ErrorType type, const std::string& message, const Position& pos);
    
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