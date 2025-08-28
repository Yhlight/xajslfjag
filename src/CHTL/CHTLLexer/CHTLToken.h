#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * CHTL语法Token枚举
 * 严格根据CHTL语法文档定义，不私自扩展
 */
enum class CHTLTokenType {
    // 基础Token
    UNKNOWN,                    // 未知Token
    END_OF_FILE,               // 文件结束
    WHITESPACE,                // 空白字符
    NEWLINE,                   // 换行符
    
    // 注释Token
    SINGLE_LINE_COMMENT,       // // 单行注释
    MULTI_LINE_COMMENT,        // /**/ 多行注释
    GENERATOR_COMMENT,         // -- 生成器注释
    
    // 文本和字面量
    TEXT_LITERAL,              // 无修饰字面量
    DOUBLE_QUOTE_STRING,       // 双引号字符串
    SINGLE_QUOTE_STRING,       // 单引号字符串
    
    // 标点符号
    LEFT_BRACE,                // {
    RIGHT_BRACE,               // }
    LEFT_BRACKET,              // [
    RIGHT_BRACKET,             // ]
    LEFT_PAREN,                // (
    RIGHT_PAREN,               // )
    SEMICOLON,                 // ;
    COLON,                     // : (与=等价)
    EQUALS,                    // = (与:等价)
    COMMA,                     // ,
    DOT,                       // .
    SLASH,                     // /
    ASTERISK,                  // *
    
    // 关键字Token
    KEYWORD_TEMPLATE,          // [Template]
    KEYWORD_CUSTOM,            // [Custom]
    KEYWORD_ORIGIN,            // [Origin]
    KEYWORD_IMPORT,            // [Import]
    KEYWORD_NAMESPACE,         // [Namespace]
    KEYWORD_CONFIGURATION,     // [Configuration]
    KEYWORD_INFO,              // [Info]
    KEYWORD_EXPORT,            // [Export]
    
    // 类型标识符
    TYPE_STYLE,                // @Style
    TYPE_ELEMENT,              // @Element
    TYPE_VAR,                  // @Var
    TYPE_HTML,                 // @Html
    TYPE_JAVASCRIPT,           // @JavaScript
    TYPE_CHTL,                 // @Chtl
    TYPE_CJMOD,                // @CJmod
    TYPE_CONFIG,               // @Config
    
    // 操作符关键字
    KEYWORD_INHERIT,           // inherit
    KEYWORD_DELETE,            // delete
    KEYWORD_INSERT,            // insert
    KEYWORD_AFTER,             // after
    KEYWORD_BEFORE,            // before
    KEYWORD_REPLACE,           // replace
    KEYWORD_AT_TOP,            // at top
    KEYWORD_AT_BOTTOM,         // at bottom
    KEYWORD_FROM,              // from
    KEYWORD_AS,                // as
    KEYWORD_EXCEPT,            // except
    KEYWORD_TEXT,              // text
    KEYWORD_STYLE,             // style (局部样式块)
    KEYWORD_SCRIPT,            // script (局部脚本块)
    KEYWORD_USE,               // use
    
    // 特殊符号
    AMPERSAND,                 // & (引用选择器)
    DOLLAR_SIGN,               // $ (模板变量)
    HASH_SIGN,                 // # (ID选择器)
    PERIOD,                    // . (类选择器)
    
    // 标识符
    IDENTIFIER,                // 标识符
    NUMBER,                    // 数字
    
    // 选择器相关
    CSS_SELECTOR,              // CSS选择器
    PSEUDO_CLASS,              // 伪类选择器
    PSEUDO_ELEMENT,            // 伪元素选择器
    
    // 配置组相关
    CONFIG_KEY,                // 配置键
    CONFIG_VALUE,              // 配置值
    
    // 导入相关
    IMPORT_PATH,               // 导入路径
    MODULE_NAME,               // 模块名称
    
    // 命名空间相关
    NAMESPACE_NAME,            // 命名空间名称
    NAMESPACE_SEPARATOR,       // 命名空间分隔符
    
    // 约束相关
    CONSTRAINT_OPERATOR,       // 约束操作符
    
    // 模板相关
    TEMPLATE_NAME,             // 模板名称
    TEMPLATE_PARAMETER,        // 模板参数
    
    // 自定义相关
    CUSTOM_NAME,               // 自定义名称
    CUSTOM_PARAMETER,          // 自定义参数
    
    // 原始嵌入相关
    ORIGIN_TYPE,               // 原始嵌入类型
    ORIGIN_NAME,               // 原始嵌入名称
    
    // 样式组相关
    STYLE_GROUP_NAME,          // 样式组名称
    STYLE_PROPERTY,            // 样式属性
    STYLE_VALUE,               // 样式值
    
    // 元素相关
    ELEMENT_NAME,              // 元素名称
    ELEMENT_ATTRIBUTE,         // 元素属性
    ELEMENT_ATTRIBUTE_VALUE    // 元素属性值
};

/**
 * CHTL Token结构体
 */
struct CHTLToken {
    CHTLTokenType type;        // Token类型
    std::string value;         // Token值
    size_t line;               // 行号
    size_t column;             // 列号
    size_t position;           // 位置
    
    CHTLToken(CHTLTokenType t, const std::string& v, size_t l, size_t c, size_t p)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    CHTLToken() : type(CHTLTokenType::UNKNOWN), line(0), column(0), position(0) {}
};

/**
 * Token类型到字符串的转换
 */
class CHTLTokenUtils {
public:
    static std::string tokenTypeToString(CHTLTokenType type);
    static bool isKeyword(CHTLTokenType type);
    static bool isOperator(CHTLTokenType type);
    static bool isPunctuation(CHTLTokenType type);
    static bool isLiteral(CHTLTokenType type);
};

} // namespace CHTL