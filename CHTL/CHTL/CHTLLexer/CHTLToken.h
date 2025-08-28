#pragma once
#include <string>
#include <vector>
#include <memory>

namespace CHTL {
    namespace Core {
        
        /**
         * CHTL词法单元类型枚举
         * 严格按照CHTL语法文档定义的所有语法元素
         */
        enum class CHTLTokenType {
            // 基础词法单元
            IDENTIFIER,           // 标识符
            STRING_LITERAL,       // 字符串字面量 ("", '')
            UNQUOTED_LITERAL,     // 无修饰字面量
            NUMBER,               // 数字
            
            // 分隔符
            COLON,                // :
            SEMICOLON,            // ;
            COMMA,                // ,
            DOT,                  // .
            EQUALS,               // = (CE对等式，等价于:)
            
            // 括号
            LEFT_BRACE,           // {
            RIGHT_BRACE,          // }
            LEFT_BRACKET,         // [
            RIGHT_BRACKET,        // ]
            LEFT_PAREN,           // (
            RIGHT_PAREN,          // )
            
            // 注释
            LINE_COMMENT,         // //
            BLOCK_COMMENT,        // /* */
            GENERATOR_COMMENT,    // --
            
            // 关键字
            KW_TEXT,              // text
            KW_STYLE,             // style
            KW_SCRIPT,            // script
            KW_INHERIT,           // inherit
            KW_DELETE,            // delete
            KW_INSERT,            // insert
            KW_AFTER,             // after
            KW_BEFORE,            // before
            KW_REPLACE,           // replace
            KW_AT_TOP,            // at top
            KW_AT_BOTTOM,         // at bottom
            KW_FROM,              // from
            KW_AS,                // as
            KW_EXCEPT,            // except
            KW_USE,               // use
            KW_HTML5,             // html5
            
            // 块类型标识符
            TEMPLATE,             // [Template]
            CUSTOM,               // [Custom]
            ORIGIN,               // [Origin]
            IMPORT,               // [Import]
            NAMESPACE,            // [Namespace]
            CONFIGURATION,        // [Configuration]
            
            // 类型标识符
            TYPE_STYLE,           // @Style
            TYPE_ELEMENT,         // @Element
            TYPE_VAR,             // @Var
            TYPE_HTML,            // @Html
            TYPE_JAVASCRIPT,      // @JavaScript
            TYPE_CHTL,            // @Chtl
            TYPE_CJMOD,           // @CJmod
            TYPE_CONFIG,          // @Config
            
            // CHTL JS 特殊语法
            SELECTOR_START,       // {{
            SELECTOR_END,         // }}
            REFERENCE,            // &
            ARROW,                // ->
            BIND_EVENT,           // &->
            
            // HTML标签和属性
            HTML_TAG,             // HTML元素标签
            ATTRIBUTE_NAME,       // 属性名
            
            // 特殊字符
            NEWLINE,              // 换行符
            WHITESPACE,           // 空白字符
            EOF_TOKEN,            // 文件结束
            UNKNOWN               // 未知字符
        };
        
        /**
         * CHTL词法单元位置信息
         */
        struct CHTLTokenPosition {
            size_t line;          // 行号（从1开始）
            size_t column;        // 列号（从1开始）
            size_t offset;        // 在文件中的偏移量
            
            CHTLTokenPosition(size_t l = 1, size_t c = 1, size_t o = 0) 
                : line(l), column(c), offset(o) {}
        };
        
        /**
         * CHTL词法单元
         * 表示从源代码中解析出的一个语法元素
         */
        class CHTLToken {
        private:
            CHTLTokenType type_;         // 词法单元类型
            std::string value_;          // 词法单元值
            CHTLTokenPosition position_; // 位置信息
            
        public:
            CHTLToken(CHTLTokenType type, const std::string& value, 
                     const CHTLTokenPosition& position);
            
            // 访问器
            CHTLTokenType GetType() const { return type_; }
            const std::string& GetValue() const { return value_; }
            const CHTLTokenPosition& GetPosition() const { return position_; }
            
            // 类型判断方法
            bool IsKeyword() const;
            bool IsTypeIdentifier() const;
            bool IsBlockIdentifier() const;
            bool IsLiteral() const;
            bool IsOperator() const;
            bool IsComment() const;
            bool IsHTMLElement() const;
            
            // 字符串表示
            std::string ToString() const;
            
            // 比较操作
            bool operator==(const CHTLToken& other) const;
            bool operator!=(const CHTLToken& other) const;
        };
        
        // 词法单元类型到字符串的转换
        std::string TokenTypeToString(CHTLTokenType type);
        
        // 关键字映射
        bool IsKeywordString(const std::string& str);
        CHTLTokenType GetKeywordType(const std::string& str);
        
    } // namespace Core
} // namespace CHTL