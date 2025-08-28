#pragma once

#include <string>
#include <memory>

namespace CHTL {

// CHTL Token类型枚举
enum class CHTLTokenType {
    // 基础Token
    END_OF_FILE,
    IDENTIFIER,
    STRING_LITERAL,
    UNQUOTED_LITERAL,
    NUMBER,
    
    // 关键字
    KEYWORD_TEXT,
    KEYWORD_STYLE,
    KEYWORD_SCRIPT,
    KEYWORD_TEMPLATE,
    KEYWORD_CUSTOM,
    KEYWORD_ORIGIN,
    KEYWORD_IMPORT,
    KEYWORD_NAMESPACE,
    KEYWORD_CONFIGURATION,
    KEYWORD_INFO,
    KEYWORD_EXPORT,
    KEYWORD_USE,
    KEYWORD_EXCEPT,
    KEYWORD_INHERIT,
    KEYWORD_DELETE,
    KEYWORD_INSERT,
    KEYWORD_AFTER,
    KEYWORD_BEFORE,
    KEYWORD_REPLACE,
    KEYWORD_AT_TOP,
    KEYWORD_AT_BOTTOM,
    KEYWORD_FROM,
    KEYWORD_AS,
    KEYWORD_NAME,
    KEYWORD_ORIGIN_TYPE,
    
    // 模板类型
    TEMPLATE_STYLE,
    TEMPLATE_ELEMENT,
    TEMPLATE_VAR,
    
    // 自定义类型
    CUSTOM_STYLE,
    CUSTOM_ELEMENT,
    CUSTOM_VAR,
    
    // 原始嵌入类型
    ORIGIN_HTML,
    ORIGIN_STYLE,
    ORIGIN_JAVASCRIPT,
    ORIGIN_CUSTOM,  // 自定义原始嵌入类型
    
    // 导入类型
    IMPORT_HTML,
    IMPORT_STYLE,
    IMPORT_JAVASCRIPT,
    IMPORT_CHTL,
    IMPORT_CJMOD,
    IMPORT_CONFIG,
    
    // 配置类型
    CONFIGURATION_CONFIG,
    
    // 分隔符
    LEFT_BRACE,      // {
    RIGHT_BRACE,     // }
    LEFT_BRACKET,    // [
    RIGHT_BRACKET,   // ]
    LEFT_PAREN,      // (
    RIGHT_PAREN,     // )
    SEMICOLON,       // ;
    COLON,           // :
    EQUAL,           // =
    COMMA,           // ,
    DOT,             // .
    AT,              // @
    HASH,            // #
    AMPERSAND,       // &
    SLASH,           // /
    ASTERISK,        // *
    
    // 注释
    COMMENT_SINGLE,  // //
    COMMENT_MULTI,   // /**/
    COMMENT_GENERATOR, // --
    
    // 空白字符
    WHITESPACE,
    NEWLINE,
    
    // 特殊Token
    UNKNOWN
};

// CHTL Token类
class CHTLToken {
public:
    CHTLToken(CHTLTokenType type, const std::string& value, size_t line, size_t column);
    
    CHTLTokenType getType() const { return type_; }
    const std::string& getValue() const { return value_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    std::string toString() const;
    
private:
    CHTLTokenType type_;
    std::string value_;
    size_t line_;
    size_t column_;
};

// Token工厂函数
std::shared_ptr<CHTLToken> createToken(CHTLTokenType type, const std::string& value, size_t line, size_t column);

} // namespace CHTL