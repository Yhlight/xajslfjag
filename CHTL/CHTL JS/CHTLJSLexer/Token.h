#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

/**
 * @brief CHTL JS Token类型枚举
 * 定义CHTL JS语言中所有可能的Token类型
 */
enum class TokenType {
    // 基础类型
    IDENTIFIER,         // 标识符
    STRING,            // 字符串字面量 
    NUMBER,            // 数字字面量
    LITERAL,           // 无修饰字面量

    // 分隔符
    LEFT_BRACE,        // {
    RIGHT_BRACE,       // }
    LEFT_BRACKET,      // [
    RIGHT_BRACKET,     // ]
    LEFT_PAREN,        // (
    RIGHT_PAREN,       // )
    SEMICOLON,         // ;
    COLON,             // :
    EQUAL,             // =
    COMMA,             // ,
    DOT,               // .
    
    // CHTL JS特有操作符
    ARROW,             // ->
    BIND_OPERATOR,     // &->
    SELECTOR_START,    // {{
    SELECTOR_END,      // }}

    // 注释
    SINGLE_COMMENT,    // //
    MULTI_COMMENT,     // /* */

    // CHTL JS关键字
    SCRIPT,            // script
    FUNCTION,          // function
    CONST,             // const
    LET,               // let
    VAR,               // var
    IF,                // if
    ELSE,              // else
    FOR,               // for
    WHILE,             // while
    RETURN,            // return
    TRUE,              // true
    FALSE,             // false
    NULL_TOKEN,        // null
    UNDEFINED,         // undefined

    // CHTL JS增强语法
    LISTEN,            // listen
    DELEGATE,          // delegate
    ANIMATE,           // animate
    VIR,               // vir (虚对象)
    WHEN,              // when (动画关键字)
    BEGIN,             // begin (动画关键字)
    END,               // end (动画关键字)
    AT,                // at (动画关键字)
    TARGET,            // target
    DURATION,          // duration
    EASING,            // easing
    LOOP,              // loop
    DIRECTION,         // direction
    DELAY,             // delay
    CALLBACK,          // callback

    // 特殊
    NEWLINE,           // 换行符
    WHITESPACE,        // 空白字符
    EOF_TOKEN,         // 文件结束
    UNKNOWN            // 未知token
};

/**
 * @brief 位置信息结构体
 */
struct Position {
    int line;
    int column;
    int offset;

    Position(int l = 1, int c = 1, int o = 0) : line(l), column(c), offset(o) {}
    
    bool operator==(const Position& other) const {
        return line == other.line && column == other.column && offset == other.offset;
    }
};

/**
 * @brief CHTL JS Token类
 * 表示CHTL JS源代码中的一个词法单元
 */
class Token {
private:
    TokenType type_;
    std::string value_;
    Position start_pos_;
    Position end_pos_;

public:
    /**
     * @brief 构造函数
     * @param type Token类型
     * @param value Token值
     * @param start_pos 起始位置
     * @param end_pos 结束位置
     */
    Token(TokenType type, const std::string& value, 
          const Position& start_pos, const Position& end_pos)
        : type_(type), value_(value), start_pos_(start_pos), end_pos_(end_pos) {}

    /**
     * @brief 默认构造函数
     */
    Token() : type_(TokenType::UNKNOWN), value_(""), start_pos_(), end_pos_() {}

    // Getters
    TokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    const Position& getStartPos() const { return start_pos_; }
    const Position& getEndPos() const { return end_pos_; }

    // Setters
    void setType(TokenType type) { type_ = type; }
    void setValue(const std::string& value) { value_ = value; }
    void setStartPos(const Position& pos) { start_pos_ = pos; }
    void setEndPos(const Position& pos) { end_pos_ = pos; }

    /**
     * @brief 判断是否为关键字类型
     */
    bool isKeyword() const {
        return type_ >= TokenType::SCRIPT && type_ <= TokenType::CALLBACK;
    }

    /**
     * @brief 判断是否为分隔符
     */
    bool isDelimiter() const {
        return type_ >= TokenType::LEFT_BRACE && type_ <= TokenType::SELECTOR_END;
    }

    /**
     * @brief 判断是否为注释
     */
    bool isComment() const {
        return type_ >= TokenType::SINGLE_COMMENT && type_ <= TokenType::MULTI_COMMENT;
    }

    /**
     * @brief 判断是否为操作符
     */
    bool isOperator() const {
        return type_ == TokenType::ARROW || type_ == TokenType::BIND_OPERATOR;
    }

    /**
     * @brief 判断是否为CHTL JS增强语法关键字
     */
    bool isEnhancedKeyword() const {
        return type_ >= TokenType::LISTEN && type_ <= TokenType::CALLBACK;
    }

    /**
     * @brief 获取Token类型的字符串表示
     */
    std::string getTypeString() const;

    /**
     * @brief 转换为字符串表示
     */
    std::string toString() const;

    /**
     * @brief 判断两个Token是否相等
     */
    bool operator==(const Token& other) const {
        return type_ == other.type_ && value_ == other.value_ &&
               start_pos_ == other.start_pos_ && end_pos_ == other.end_pos_;
    }

    /**
     * @brief 判断两个Token是否不相等
     */
    bool operator!=(const Token& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Token智能指针类型定义
 */
using TokenPtr = std::shared_ptr<Token>;
using TokenList = std::vector<TokenPtr>;

} // namespace CHTLJS