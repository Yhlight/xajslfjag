#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <unordered_map>

namespace CHTL {

    // Token类型枚举 - 仅CHTL部分
    enum class TokenType {
        // 基础符号
        LEFT_BRACE,         // {
        RIGHT_BRACE,        // }
        LEFT_BRACKET,       // [
        RIGHT_BRACKET,      // ]
        LEFT_PAREN,         // (
        RIGHT_PAREN,        // )
        SEMICOLON,          // ;
        COLON,              // :
        EQUAL,              // =
        COMMA,              // ,
        DOT,                // .
        AMPERSAND,          // &
        HASH,               // #
        AT,                 // @
        
        // 字面量
        STRING,             // "string" 或 'string'
        UNQUOTED_STRING,    // 无引号字符串
        NUMBER,             // 数字
        
        // 注释
        LINE_COMMENT,       // //
        BLOCK_COMMENT,      // /* */
        GENERATOR_COMMENT,  // --
        
        // 关键字 - 核心语法
        TEXT,               // text
        STYLE,              // style
        HTML5,              // html5
        
        // 关键字 - 模板系统
        TEMPLATE,           // [Template]
        CUSTOM,             // [Custom]
        ORIGIN,             // [Origin]
        IMPORT,             // [Import]
        CONFIGURATION,      // [Configuration]
        NAMESPACE,          // [Namespace]
        INFO,               // [Info]
        EXPORT,             // [Export]
        NAME,               // [Name]
        ORIGIN_TYPE,        // [OriginType]
        
        // 关键字 - 模板类型
        STYLE_TYPE,         // @Style
        ELEMENT_TYPE,       // @Element  
        VAR_TYPE,           // @Var
        HTML_TYPE,          // @Html
        JAVASCRIPT_TYPE,    // @JavaScript
        CONFIG_TYPE,        // @Config
        CHTL_TYPE,          // @Chtl
        CJMOD_TYPE,         // @CJmod
        
        // 关键字 - 操作符
        INHERIT,            // inherit
        DELETE,             // delete
        INSERT,             // insert
        AFTER,              // after
        BEFORE,             // before
        REPLACE,            // replace
        AT_TOP,             // at top
        AT_BOTTOM,          // at bottom
        FROM,               // from
        AS,                 // as
        EXCEPT,             // except
        USE,                // use
        
        // HTML标签 (常用的)
        HTML,               // html
        HEAD,               // head
        BODY,               // body
        DIV,                // div
        SPAN,               // span
        P,                  // p
        A,                  // a
        IMG,                // img
        INPUT,              // input
        BUTTON,             // button
        FORM,               // form
        TABLE,              // table
        TR,                 // tr
        TD,                 // td
        TH,                 // th
        UL,                 // ul
        OL,                 // ol
        LI,                 // li
        H1, H2, H3, H4, H5, H6, // h1-h6
        
        // 特殊标识符
        IDENTIFIER,         // 标识符
        
        // 结束符
        EOF_TOKEN,          // 文件结束
        NEWLINE,            // 换行符
        WHITESPACE,         // 空白符
        
        // 错误
        UNKNOWN             // 未知token
    };

    // Token结构体
    struct Token {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;
        size_t position;
        
        Token(TokenType t = TokenType::UNKNOWN, 
              const std::string& v = "", 
              size_t l = 0, 
              size_t c = 0, 
              size_t p = 0)
            : type(t), value(v), line(l), column(c), position(p) {}
        
        // 判断是否为关键字
        bool isKeyword() const;
        
        // 判断是否为HTML标签
        bool isHtmlTag() const;
        
        // 判断是否为模板类型
        bool isTemplateType() const;
        
        // 判断是否为字面量
        bool isLiteral() const;
        
        // 判断是否为注释
        bool isComment() const;
        
        // 转换为字符串表示
        std::string toString() const;
    };

    // 关键字映射表
    class KeywordMap {
    private:
        static std::unordered_map<std::string, TokenType> keywords;
        static std::unordered_map<std::string, TokenType> htmlTags;
        static bool initialized;
        
        static void initialize();
        
    public:
        static TokenType getKeywordType(const std::string& word);
        static TokenType getHtmlTagType(const std::string& tag);
        static bool isKeyword(const std::string& word);
        static bool isHtmlTag(const std::string& tag);
    };

    // Token类型转字符串
    std::string tokenTypeToString(TokenType type);
}

#endif // CHTL_TOKEN_H