#pragma once
#include <string>
#include <vector>

namespace CHTL {

// CHTL Token类型 - 严格按照语法文档定义
enum class CHTLTokenType {
    // === 基础字面量 ===
    STRING_LITERAL,           // "string" 'string'
    UNQUOTED_LITERAL,         // CSS风格的无引号字符串
    NUMBER,                   // 123 123.45
    IDENTIFIER,               // 标识符
    
    // === CHTL核心关键字 ===
    TEXT,                     // text
    STYLE,                    // style (局部样式块)
    SCRIPT,                   // script (局部脚本块)
    USE,                      // use声明
    
    // === CHTL继承和修改关键字 ===
    INHERIT,                  // inherit
    DELETE,                   // delete
    INSERT,                   // insert
    AFTER,                    // after
    BEFORE,                   // before
    REPLACE,                  // replace
    AT_TOP,                   // at top
    AT_BOTTOM,                // at bottom
    
    // === CHTL导入关键字 ===
    FROM,                     // from
    AS,                       // as
    EXCEPT,                   // except
    
    // === CHTL声明前缀 ===
    TEMPLATE,                 // [Template]
    CUSTOM,                   // [Custom]
    ORIGIN,                   // [Origin]
    IMPORT,                   // [Import]
    NAMESPACE,                // [Namespace]
    CONFIGURATION,            // [Configuration]
    INFO,                     // [Info] (CMOD)
    EXPORT,                   // [Export] (CMOD)
    
    // === CHTL配置块类型 ===
    NAME,                     // [Name]
    ORIGINTYPE,               // [OriginType]
    
    // === CHTL类型标识符 ===
    AT_STYLE,                 // @Style
    AT_ELEMENT,               // @Element
    AT_VAR,                   // @Var
    AT_HTML,                  // @Html
    AT_JAVASCRIPT,            // @JavaScript
    AT_CHTL,                  // @Chtl
    AT_CJMOD,                 // @CJmod
    AT_CONFIG,                // @Config
    
    // === CHTL结构符号 ===
    LBRACE, RBRACE,           // { }
    LPAREN, RPAREN,           // ( )
    LBRACKET, RBRACKET,       // [ ]
    LT, GT,                   // < > (HTML标签)
    SEMICOLON,                // ;
    COLON,                    // : (CE对等式)
    EQUAL,                    // = (CE对等式)
    COMMA,                    // ,
    DOT,                      // .
    SLASH,                    // /
    STAR,                     // *
    AMPERSAND,                // &
    HASH,                     // #
    
    // === CHTL选择器 ===
    CLASS_SELECTOR,           // .className
    ID_SELECTOR,              // #idName
    
    // === CHTL注释 (3种类型) ===
    LINE_COMMENT,             // // (被生成器忽略)
    BLOCK_COMMENT,            // /* */ (被生成器忽略)
    GENERATOR_COMMENT,        // -- (被生成器处理)
    
    // === 特殊Token ===
    NEWLINE,                  // 换行
    WHITESPACE,               // 空白字符
    EOF_TOKEN,                // 文件结束
    INVALID                   // 错误Token
};

// CHTL Token结构
struct CHTLToken {
    CHTLTokenType type;
    std::string value;
    size_t line;
    size_t column;
    size_t position;          // 在源码中的位置
    
    CHTLToken(CHTLTokenType t = CHTLTokenType::INVALID, 
              const std::string& v = "", 
              size_t l = 1, 
              size_t c = 1, 
              size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // 获取Token类型的字符串表示
    std::string getTypeName() const;
    
    // 判断是否为有效Token
    bool isValid() const { return type != CHTLTokenType::INVALID; }
    
    // 判断是否为关键字
    bool isKeyword() const;
    
    // 判断是否为前缀类型
    bool isPrefix() const;
    
    // 判断是否为类型标识符
    bool isTypeIdentifier() const;
};

// Token工具函数
class CHTLTokenUtils {
public:
    // 将字符串转换为Token类型
    static CHTLTokenType stringToTokenType(const std::string& str);
    
    // 判断字符串是否为CHTL关键字
    static bool isKeyword(const std::string& str);
    
    // 判断字符串是否为CHTL前缀
    static bool isPrefix(const std::string& str);
    
    // 判断字符串是否为类型标识符
    static bool isTypeIdentifier(const std::string& str);
    
    // 获取所有CHTL关键字列表
    static std::vector<std::string> getAllKeywords();
};

} // namespace CHTL