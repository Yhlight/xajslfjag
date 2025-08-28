#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <ostream>

namespace CHTL {

// Token类型枚举 - 严格根据CHTL语法文档定义
enum class TokenType {
    // 文件结束标记
    EOF_TOKEN,
    
    // 字面量
    STRING_LITERAL,          // 双引号字符串 ""
    SINGLE_QUOTE_STRING,     // 单引号字符串 ''
    UNQUOTED_LITERAL,        // 无修饰字面量
    NUMBER,                  // 数字
    
    // 标识符
    IDENTIFIER,              // 标识符 (元素名、属性名等)
    
    // 关键字 - 基础
    TEXT,                    // text
    STYLE,                   // style (局部样式块)
    SCRIPT,                  // script (局部脚本块)
    USE,                     // use
    INHERIT,                 // inherit
    DELETE_KW,               // delete
    INSERT,                  // insert
    AFTER,                   // after
    BEFORE,                  // before
    REPLACE,                 // replace
    AT,                      // at
    TOP,                     // top
    BOTTOM,                  // bottom
    FROM,                    // from
    AS,                      // as
    EXCEPT,                  // except
    
    // 方括号关键字
    TEMPLATE,                // [Template]
    CUSTOM,                  // [Custom]
    ORIGIN,                  // [Origin]
    IMPORT,                  // [Import]
    NAMESPACE,               // [Namespace]
    CONFIGURATION,           // [Configuration]
    
    // @前缀类型
    AT_STYLE,                // @Style
    AT_ELEMENT,              // @Element
    AT_VAR,                  // @Var
    AT_HTML,                 // @Html
    AT_JAVASCRIPT,           // @JavaScript
    AT_CHTL,                 // @Chtl
    AT_CJMOD,                // @CJmod
    AT_CONFIG,               // @Config
    AT_CUSTOM_TYPE,          // @自定义类型 (如@Vue)
    
    // 注释
    SINGLE_LINE_COMMENT,     // //
    MULTI_LINE_COMMENT,      // /* */
    GENERATOR_COMMENT,       // -- (生成器识别的注释)
    
    // 操作符和分隔符
    LEFT_BRACE,              // {
    RIGHT_BRACE,             // }
    LEFT_BRACKET,            // [
    RIGHT_BRACKET,           // ]
    LEFT_PAREN,              // (
    RIGHT_PAREN,             // )
    SEMICOLON,               // ;
    COLON,                   // :
    EQUALS,                  // =
    COMMA,                   // ,
    DOT,                     // .
    SLASH,                   // /
    STAR,                    // *
    AMPERSAND,               // &
    HASH,                    // #
    
    // 特殊标记
    WHITESPACE,              // 空白字符
    NEWLINE,                 // 换行符
    UNKNOWN                  // 未知字符
};

// Token类 - 表示一个词法单元
class Token {
private:
    TokenType type_;
    std::string lexeme_;     // 原始文本
    std::string value_;      // 处理后的值（如去除引号的字符串）
    int line_;               // 行号
    int column_;             // 列号
    
public:
    // 构造函数
    Token();
    Token(TokenType type, const std::string& lexeme, int line, int column);
    Token(TokenType type, const std::string& lexeme, const std::string& value, int line, int column);
    
    // 析构函数
    ~Token() = default;
    
    // Getter方法
    TokenType getType() const { return type_; }
    const std::string& getLexeme() const { return lexeme_; }
    const std::string& getValue() const { return value_; }
    int getLine() const { return line_; }
    int getColumn() const { return column_; }
    
    // 类型判断辅助方法
    bool isLiteral() const;
    bool isKeyword() const;
    bool isOperator() const;
    bool isComment() const;
    bool isAtType() const;
    bool isBracketKeyword() const;
    
    // 获取类型名称
    static std::string getTypeName(TokenType type);
    std::string getTypeName() const { return getTypeName(type_); }
    
    // 输出Token信息
    friend std::ostream& operator<<(std::ostream& os, const Token& token);
};

} // namespace CHTL

#endif // CHTL_TOKEN_H