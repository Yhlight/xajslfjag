#pragma once
#include <string>
#include <variant>

namespace CHTL_JS {

/**
 * CHTL JS Token类型枚举
 * 严格按照CHTL语法文档中的CHTL JS语法定义
 */
enum class TokenType {
    // 基础标识符和字面量
    IDENTIFIER,                   // 标识符
    UNQUOTED_LITERAL,            // 无修饰字面量 (无引号字符串)
    DOUBLE_QUOTED_STRING,        // 双引号字符串 ""
    SINGLE_QUOTED_STRING,        // 单引号字符串 ''
    NUMBER,                      // 数字

    // 注释
    LINE_COMMENT,                // // 注释
    BLOCK_COMMENT,               // /* */ 注释  
    GENERATOR_COMMENT,           // -- 生成器识别注释

    // 符号
    LEFT_BRACE,                  // {
    RIGHT_BRACE,                 // }
    LEFT_BRACKET,                // [
    RIGHT_BRACKET,               // ]
    LEFT_PAREN,                  // (
    RIGHT_PAREN,                 // )
    SEMICOLON,                   // ;
    COLON,                       // :
    EQUALS,                      // =
    COMMA,                       // ,
    DOT,                         // .
    ARROW,                       // -> (CHTL JS特有)
    AMPERSAND,                   // &

    // CHTL JS关键字
    SCRIPT,                      // script (局部script块)
    LISTEN,                      // listen (监听器)
    DELEGATE,                    // delegate (事件委托)
    ANIMATE,                     // animate (动画)
    VIR,                         // vir (虚对象)

    // CHTL JS增强选择器
    ENHANCED_SELECTOR,           // {{选择器}} 
    CLASS_SELECTOR,              // .className
    ID_SELECTOR,                 // #idName
    TAG_SELECTOR,                // 标签选择器
    
    // CHTL语法中允许的变量引用 (只在CHTL JS中可以使用)
    TEMPLATE_VAR,                // 模板变量引用 
    CUSTOM_VAR,                  // 自定义变量组引用

    // 特殊符号
    EOF_TOKEN,                   // 文件结束
    NEWLINE,                     // 换行符
    WHITESPACE,                  // 空白字符

    // 错误标记
    INVALID                      // 无效token
};

/**
 * Token值类型
 */
using TokenValue = std::variant<std::string, int, double>;

/**
 * 位置信息
 */
struct Position {
    size_t line;
    size_t column;
    size_t offset;
    
    Position(size_t l = 1, size_t c = 1, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

/**
 * CHTL JS Token类
 */
class Token {
public:
    TokenType type;
    TokenValue value;
    Position position;
    
    Token(TokenType t, const TokenValue& v, const Position& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const std::string& v, const Position& pos)
        : type(t), value(v), position(pos) {}
    
    Token(TokenType t, const Position& pos)
        : type(t), value(std::string{}), position(pos) {}
    
    // 获取字符串值
    std::string getString() const {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        }
        return "";
    }
    
    // 获取数字值
    double getNumber() const {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        return 0.0;
    }
    
    // 检查是否为特定类型
    bool isType(TokenType t) const {
        return type == t;
    }
    
    // 检查是否为CHTL JS关键字
    bool isCHTLJSKeyword() const {
        return type == TokenType::LISTEN || type == TokenType::DELEGATE || 
               type == TokenType::ANIMATE || type == TokenType::VIR;
    }
    
    // 检查是否为变量引用
    bool isVariableReference() const {
        return type == TokenType::TEMPLATE_VAR || type == TokenType::CUSTOM_VAR;
    }
    
    // 检查是否为选择器相关
    bool isSelector() const {
        return type == TokenType::ENHANCED_SELECTOR || 
               type == TokenType::CLASS_SELECTOR || 
               type == TokenType::ID_SELECTOR ||
               type == TokenType::TAG_SELECTOR;
    }
    
    // 转换为字符串表示
    std::string toString() const;
};

/**
 * Token类型到字符串的转换
 */
std::string tokenTypeToString(TokenType type);

/**
 * 检查字符串是否为CHTL JS关键字
 */
TokenType getKeywordType(const std::string& str);

/**
 * 检查字符串是否为HTML标签
 */
bool isHTMLTag(const std::string& str);

} // namespace CHTL_JS