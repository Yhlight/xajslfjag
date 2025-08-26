#pragma once

#include <string>

namespace CHTL {
namespace CHTLJS {
namespace Lexer {

/**
 * CHTLJSTokenType - CHTL JS Token类型枚举
 * 专门为CHTL JS语法定义的Token类型
 */
enum class CHTLJSTokenType {
    // 基础Token
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量
    NUMBER_LITERAL,     // 数字字面量
    BOOLEAN_LITERAL,    // 布尔字面量
    
    // 运算符
    ASSIGN,             // =
    COLON,              // :
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    ARROW,              // ->
    
    // 括号
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    
    // CHTL JS特有Token
    ENHANCED_SELECTOR,  // {{selector}}
    VIR_KEYWORD,        // vir
    LISTEN_KEYWORD,     // listen
    DELEGATE_KEYWORD,   // delegate
    ANIMATE_KEYWORD,    // animate
    
    // CHTL JS函数块（新语法）
    FUNCTION_BLOCK_START,   // listen {, delegate {, animate {
    FUNCTION_BLOCK_END,     // }
    
    // 动画关键字
    TARGET_KEYWORD,     // target
    DURATION_KEYWORD,   // duration
    EASING_KEYWORD,     // easing
    BEGIN_KEYWORD,      // begin
    WHEN_KEYWORD,       // when
    END_KEYWORD,        // end
    LOOP_KEYWORD,       // loop
    DIRECTION_KEYWORD,  // direction
    DELAY_KEYWORD,      // delay
    CALLBACK_KEYWORD,   // callback
    AT_KEYWORD,         // at
    
    // 事件相关
    EVENT_NAME,         // click, mouseenter, etc.
    
    // 注释
    COMMENT,            // 注释
    
    // 文件结束
    EOF_TOKEN,          // 文件结束
    
    // 错误
    INVALID,            // 无效Token
    
    // 无修饰字面量（CHTL JS支持）
    UNQUOTED_LITERAL    // 无修饰字符串
};

/**
 * CHTLJSToken - CHTL JS词法单元
 * CHTL JS专用的Token实现
 */
class CHTLJSToken {
private:
    CHTLJSTokenType type_;
    std::string value_;
    std::string file_path_;
    size_t line_number_;
    size_t column_number_;
    size_t start_position_;
    size_t end_position_;
    
public:
    /**
     * 构造函数
     * @param type Token类型
     * @param value Token值
     * @param file_path 文件路径
     * @param line 行号
     * @param column 列号
     * @param start 起始位置
     * @param end 结束位置
     */
    CHTLJSToken(CHTLJSTokenType type = CHTLJSTokenType::INVALID,
                const std::string& value = "",
                const std::string& file_path = "",
                size_t line = 0,
                size_t column = 0,
                size_t start = 0,
                size_t end = 0);
    
    /**
     * 获取Token类型
     * @return Token类型
     */
    CHTLJSTokenType getType() const;
    
    /**
     * 获取Token值
     * @return Token值
     */
    const std::string& getValue() const;
    
    /**
     * 获取文件路径
     * @return 文件路径
     */
    const std::string& getFilePath() const;
    
    /**
     * 获取行号
     * @return 行号
     */
    size_t getLineNumber() const;
    
    /**
     * 获取列号
     * @return 列号
     */
    size_t getColumnNumber() const;
    
    /**
     * 获取起始位置
     * @return 起始位置
     */
    size_t getStartPosition() const;
    
    /**
     * 获取结束位置
     * @return 结束位置
     */
    size_t getEndPosition() const;
    
    /**
     * 是否为CHTL JS关键字
     * @return 是否为关键字
     */
    bool isCHTLJSKeyword() const;
    
    /**
     * 是否为增强选择器
     * @return 是否为增强选择器
     */
    bool isEnhancedSelector() const;
    
    /**
     * 是否为CHTL JS函数
     * @return 是否为CHTL JS函数
     */
    bool isCHTLJSFunction() const;
    
    /**
     * 是否为动画关键字
     * @return 是否为动画关键字
     */
    bool isAnimationKeyword() const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const;
    
    /**
     * Token类型转换为字符串
     * @param type Token类型
     * @return 字符串表示
     */
    static std::string typeToString(CHTLJSTokenType type);
    
    /**
     * 字符串转换为Token类型
     * @param type_str 类型字符串
     * @return Token类型
     */
    static CHTLJSTokenType stringToType(const std::string& type_str);
    
    /**
     * 检查是否为有效的JS事件名
     * @param event_name 事件名
     * @return 是否为有效事件名
     */
    static bool isValidJSEvent(const std::string& event_name);
    
    /**
     * 获取所有支持的JS事件列表
     * @return 事件名列表
     */
    static std::vector<std::string> getSupportedJSEvents();
};

} // namespace Lexer
} // namespace CHTLJS
} // namespace CHTL