#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {
namespace Token {

// CHTL Token类型定义，严格按照CHTL语法文档
enum class TokenType {
    // 基础Token
    IDENTIFIER,          // 标识符
    STRING_LITERAL,      // 字符串字面量(双引号)
    SINGLE_STRING_LITERAL, // 单引号字符串
    UNQUOTED_LITERAL,    // 无修饰字面量
    NUMBER_LITERAL,      // 数字字面量
    
    // 符号
    LEFT_BRACE,          // {
    RIGHT_BRACE,         // }
    LEFT_BRACKET,        // [
    RIGHT_BRACKET,       // ]
    LEFT_PAREN,          // (
    RIGHT_PAREN,         // )
    SEMICOLON,           // ;
    COLON,               // :
    EQUALS,              // =
    COMMA,               // ,
    DOT,                 // .
    HASHTAG,             // #
    
    // 注释
    SINGLE_COMMENT,      // //
    MULTI_COMMENT,       // /* */
    GENERATOR_COMMENT,   // --
    
    // 关键字块标识符
    TEMPLATE,            // [Template]
    CUSTOM,              // [Custom]
    ORIGIN,              // [Origin]
    IMPORT,              // [Import]
    CONFIGURATION,       // [Configuration]
    NAMESPACE,           // [Namespace]
    INFO,                // [Info]
    EXPORT,              // [Export]
    
    // 类型标识符
    STYLE_TYPE,          // @Style
    ELEMENT_TYPE,        // @Element
    VAR_TYPE,            // @Var
    HTML_TYPE,           // @Html
    JAVASCRIPT_TYPE,     // @JavaScript
    CONFIG_TYPE,         // @Config
    CHTL_TYPE,           // @Chtl
    CJMOD_TYPE,          // @CJmod
    
    // CHTL关键字
    TEXT,                // text
    STYLE,               // style (局部样式块)
    SCRIPT,              // script (局部脚本块)
    INHERIT,             // inherit
    DELETE,              // delete
    INSERT,              // insert
    AFTER,               // after
    BEFORE,              // before
    REPLACE,             // replace
    AT_TOP,              // at top
    AT_BOTTOM,           // at bottom
    FROM,                // from
    AS,                  // as
    EXCEPT,              // except
    USE,                 // use
    VIR,                 // vir (CHTL JS虚对象)
    
    // HTML5声明
    HTML5,               // html5
    
    // HTML元素标签
    HTML_TAG,            // 所有HTML标签
    
    // 特殊符号
    AMPERSAND,           // & (上下文推导)
    ARROW,               // -> (CHTL JS语法)
    
    // 结束符
    EOF_TOKEN,           // 文件结束
    NEWLINE,             // 换行
    WHITESPACE,          // 空白字符
    
    // 错误类型
    UNKNOWN,             // 未知Token
    ERROR                // 错误Token
};

// Token结构体
struct CHTLToken {
    TokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;
    
    CHTLToken() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    
    CHTLToken(TokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // 便于调试的字符串表示
    std::string toString() const;
    
    // 判断是否是关键字块
    bool isKeywordBlock() const;
    
    // 判断是否是类型标识符
    bool isTypeIdentifier() const;
    
    // 判断是否是HTML标签
    bool isHTMLTag() const;
};

// Token工具类
class TokenUtils {
public:
    // 获取所有HTML标签名
    static const std::vector<std::string>& getHTMLTags();
    
    // 判断是否是HTML标签
    static bool isHTMLTag(const std::string& tag);
    
    // 获取Token类型名称
    static std::string getTokenTypeName(TokenType type);
    
    // 判断字符是否是标识符字符
    static bool isIdentifierChar(char c);
    
    // 判断字符是否是数字
    static bool isDigit(char c);
    
    // 判断字符是否是空白字符
    static bool isWhitespace(char c);
    
    // CE对等式：判断是否是等价符号(:和=)
    static bool isEquivalentSymbol(char c);
    
private:
    static std::vector<std::string> htmlTags;
    static std::unordered_map<TokenType, std::string> tokenTypeNames;
    static void initializeHTMLTags();
    static void initializeTokenTypeNames();
};

} // namespace Token
} // namespace CHTL