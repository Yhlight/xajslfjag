#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

// CHTL Token类型枚举 - 严格基于CHTL语法文档
enum class CHTLTokenType {
    // === 基础字面量 ===
    STRING_LITERAL,           // "string" 'string'
    UNQUOTED_LITERAL,         // CSS风格的无引号字符串
    NUMBER,                   // 123 123.45
    IDENTIFIER,               // 标识符
    
    // === CHTL核心关键字 ===
    TEXT,                     // text (文本节点)
    STYLE,                    // style (局部样式块)
    SCRIPT,                   // script (局部脚本块)
    USE,                      // use (使用配置组)
    
    // === CHTL模板和自定义前缀 ===
    LBRACKET_TEMPLATE,        // [Template]
    LBRACKET_CUSTOM,          // [Custom]
    LBRACKET_ORIGIN,          // [Origin]
    LBRACKET_IMPORT,          // [Import]
    LBRACKET_NAMESPACE,       // [Namespace]
    LBRACKET_CONFIGURATION,   // [Configuration]
    LBRACKET_INFO,            // [Info] (CMOD)
    LBRACKET_EXPORT,          // [Export] (CMOD)
    LBRACKET_NAME,            // [Name] (配置子块)
    LBRACKET_ORIGINTYPE,      // [OriginType] (配置子块)
    
    // === CHTL类型标识符 ===
    AT_STYLE,                 // @Style
    AT_ELEMENT,               // @Element
    AT_VAR,                   // @Var
    AT_HTML,                  // @Html
    AT_JAVASCRIPT,            // @JavaScript
    AT_CHTL,                  // @Chtl
    AT_CJMOD,                 // @CJmod
    AT_CONFIG,                // @Config
    
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
    
    // === CHTL结构符号 ===
    LBRACE, RBRACE,           // { }
    LPAREN, RPAREN,           // ( )
    LBRACKET, RBRACKET,       // [ ]
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
    
    // === CHTL JS相关Token ===
    ENHANCED_SELECTOR,        // {{selector}} (CHTL JS增强选择器)
    VIR,                      // vir (虚对象关键字)
    LISTEN,                   // listen (监听器)
    DELEGATE,                 // delegate (事件委托)
    ANIMATE,                  // animate (动画)
    ARROW,                    // -> (CHTL JS链式访问)
    
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
    size_t position;
    
    CHTLToken(CHTLTokenType t = CHTLTokenType::INVALID, 
              const std::string& v = "", 
              size_t l = 0, 
              size_t c = 0, 
              size_t p = 0)
        : type(t), value(v), line(l), column(c), position(p) {}
    
    // Token工具方法
    bool isValid() const { return type != CHTLTokenType::INVALID; }
    std::string getTypeName() const;
    bool isKeyword() const;
    bool isBracketKeyword() const;
    bool isTypeIdentifier() const;
    bool isComment() const;
    bool isStructuralSymbol() const;
    bool isSelector() const;
};

// CHTL Token工具类
class CHTLTokenUtils {
public:
    // Token类型转换
    static CHTLTokenType stringToTokenType(const std::string& str);
    static std::string tokenTypeToString(CHTLTokenType type);
    
    // Token分类检查
    static bool isKeyword(const std::string& str);
    static bool isBracketKeyword(const std::string& str);
    static bool isTypeIdentifier(const std::string& str);
    static bool isStructuralSymbol(const std::string& str);
    static bool isComment(CHTLTokenType type);
    static bool isSelector(CHTLTokenType type);
    
    // 获取映射表
    static const std::unordered_map<std::string, CHTLTokenType>& getKeywordMap();
    static const std::unordered_map<std::string, CHTLTokenType>& getBracketKeywordMap();
    static const std::unordered_map<std::string, CHTLTokenType>& getTypeIdentifierMap();
    static const std::unordered_set<std::string>& getCHTLKeywords();
};

} // namespace CHTL