#pragma once
#include "Token.h"
#include "GlobalMap.h"
#include "../CHTLJSState/State.h"
#include "../CHTLJSContext/Context.h"
#include "../../Error/ErrorReport.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

// 前向声明
class CHTLJSStateMachine;
class CHTLJSContext;

/**
 * CHTL JS词法分析器内部状态
 */
enum class LexerInternalState {
    NORMAL,                     // 正常状态
    IN_STRING_DOUBLE,           // 在双引号字符串中
    IN_STRING_SINGLE,           // 在单引号字符串中
    IN_LINE_COMMENT,            // 在行注释中
    IN_BLOCK_COMMENT,           // 在块注释中
    IN_GENERATOR_COMMENT,       // 在生成器注释中
    IN_ENHANCED_SELECTOR,       // 在增强选择器中 {{...}}
    IN_EVENT_BINDING,           // 在事件绑定中
    IN_CHTLJS_FUNCTION          // 在CHTL JS函数中
};

/**
 * CHTL JS词法分析器
 * 专门用于分析CHTL JS语法
 */
class Lexer {
public:
    /**
     * 构造函数
     */
    explicit Lexer(const std::string& source,
                   CHTLJSStateMachine* stateMachine = nullptr,
                   CHTLJSContext* context = nullptr,
                   ErrorReporter* errorReporter = nullptr);
    
    /**
     * 析构函数
     */
    ~Lexer() = default;
    
    // 禁用拷贝和移动
    Lexer(const Lexer&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    
    /**
     * 分析所有token
     */
    TokenSequence tokenize();
    
    /**
     * 获取下一个token
     */
    Token nextToken();
    
    /**
     * 查看下一个token但不移动位置
     */
    Token peekToken();
    
    /**
     * 检查是否已到达源码末尾
     */
    bool isAtEnd() const;
    
    /**
     * 获取当前位置
     */
    Position getCurrentPosition() const;
    
    /**
     * 重置词法分析器
     */
    void reset(const std::string& source);

private:
    std::string m_source;               // 源代码
    size_t m_position;                  // 当前位置
    size_t m_line;                      // 当前行号
    size_t m_column;                    // 当前列号
    LexerInternalState m_state;         // 内部状态
    
    // 外部组件
    CHTLJSStateMachine* m_stateMachine;
    CHTLJSContext* m_context;
    ErrorReporter* m_errorReporter;
    
    // 词法分析方法
    Token scanToken();
    Token scanString(char quote);
    Token scanComment();
    Token scanEnhancedSelector();
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    
    // 辅助方法
    char currentChar() const;
    char nextChar();
    char peekChar() const;
    void skipWhitespace();
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    
    // 错误处理
    void reportError(const std::string& message);
};

} // namespace CHTLJS