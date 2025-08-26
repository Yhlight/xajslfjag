#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <unordered_map>

namespace CHTL {

enum class TokenType {
    // 结束标记
    END_OF_FILE,
    
    // 字面量
    STRING_LITERAL,          // "字符串" 或 '字符串'
    UNQUOTED_LITERAL,        // 无引号字面量
    NUMBER_LITERAL,          // 数字字面量
    
    // 标识符
    IDENTIFIER,              // 标识符
    
    // 关键字
    TEXT,                    // text
    STYLE,                   // style
    SCRIPT,                  // script
    IMPORT,                  // [Import]
    ORIGIN,                  // [Origin]
    CONFIGURATION,           // [Configuration]
    NAMESPACE,               // namespace
    FROM,                    // from
    AS,                      // as
    DELETE,                  // delete
    
    // 模板和变量
    TEMPLATE_VAR,            // ${var}
    CUSTOM_VAR,              // $var
    
    // 选择器和引用
    SELECTOR_REF,            // &
    CLASS_SELECTOR,          // .className
    ID_SELECTOR,             // #idName
    PSEUDO_CLASS,            // :hover 等
    PSEUDO_ELEMENT,          // ::before 等
    
    // CHTL JS 特殊语法
    DOUBLE_BRACE_OPEN,       // {{
    DOUBLE_BRACE_CLOSE,      // }}
    ARROW,                   // ->
    FAT_ARROW,               // =>
    
    // 操作符
    COLON,                   // :
    SEMICOLON,               // ;
    COMMA,                   // ,
    DOT,                     // .
    EQUALS,                  // =
    PLUS,                    // +
    MINUS,                   // -
    ASTERISK,                // *
    SLASH,                   // /
    PERCENT,                 // %
    EXCLAMATION,             // !
    QUESTION,                // ?
    AT,                      // @
    HASH,                    // #
    DOLLAR,                  // $
    AMPERSAND,               // &
    PIPE,                    // |
    TILDE,                   // ~
    CARET,                   // ^
    
    // 括号
    LEFT_BRACE,              // {
    RIGHT_BRACE,             // }
    LEFT_PAREN,              // (
    RIGHT_PAREN,             // )
    LEFT_BRACKET,            // [
    RIGHT_BRACKET,           // ]
    
    // 比较操作符
    LESS_THAN,               // <
    GREATER_THAN,            // >
    LESS_EQUAL,              // <=
    GREATER_EQUAL,           // >=
    EQUAL_EQUAL,             // ==
    NOT_EQUAL,               // !=
    
    // 逻辑操作符
    AND_AND,                 // &&
    OR_OR,                   // ||
    
    // 注释
    SINGLE_LINE_COMMENT,     // // 注释
    MULTI_LINE_COMMENT,      // /* 注释 */
    GENERATOR_COMMENT,       // -- 注释
    
    // 空白
    WHITESPACE,              // 空白字符
    NEWLINE,                 // 换行符
    
    // HTML元素标签（常用）
    HTML,
    HEAD,
    BODY,
    DIV,
    SPAN,
    P,
    A,
    IMG,
    UL,
    OL,
    LI,
    TABLE,
    TR,
    TD,
    TH,
    FORM,
    INPUT,
    BUTTON,
    TEXTAREA,
    SELECT,
    OPTION,
    H1, H2, H3, H4, H5, H6,
    BR,
    HR,
    META,
    LINK,
    TITLE,
    NAV,
    HEADER,
    FOOTER,
    MAIN,
    SECTION,
    ARTICLE,
    ASIDE,
    
    // 导入类型
    AT_CHTL,                 // @Chtl
    AT_CJMOD,                // @CJmod
    AT_HTML,                 // @Html
    AT_STYLE,                // @Style
    AT_JAVASCRIPT,           // @JavaScript
    
    // 特殊标记
    WILDCARD,                // * (用于导入)
    DOUBLE_COLON,            // ::
    
    // 错误标记
    UNKNOWN
};

class Token {
public:
    TokenType type;
    std::string value;
    int line;
    int column;
    int position;
    
    Token() : type(TokenType::UNKNOWN), line(0), column(0), position(0) {}
    
    Token(TokenType t, const std::string& v, int l, int c, int p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    bool isLiteral() const {
        return type == TokenType::STRING_LITERAL || 
               type == TokenType::UNQUOTED_LITERAL ||
               type == TokenType::NUMBER_LITERAL;
    }
    
    bool isOperator() const {
        return type >= TokenType::COLON && type <= TokenType::NOT_EQUAL;
    }
    
    bool isKeyword() const {
        return type >= TokenType::TEXT && type <= TokenType::DELETE;
    }
    
    bool isHTMLTag() const {
        return type >= TokenType::HTML && type <= TokenType::ASIDE;
    }
    
    bool isComment() const {
        return type == TokenType::SINGLE_LINE_COMMENT ||
               type == TokenType::MULTI_LINE_COMMENT ||
               type == TokenType::GENERATOR_COMMENT;
    }
    
    bool isWhitespace() const {
        return type == TokenType::WHITESPACE || type == TokenType::NEWLINE;
    }
};

// 关键字映射
extern std::unordered_map<std::string, TokenType> keywordMap;

// 初始化关键字映射
void initializeKeywordMap();

// Token类型转字符串
std::string tokenTypeToString(TokenType type);

} // namespace CHTL

#endif // CHTL_TOKEN_H