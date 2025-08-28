/**
 * CHTL词法分析器Token定义
 * 
 * 严格根据CHTL语法文档定义所有Token类型
 * 包括关键字、操作符、标识符、字面量等
 * 
 * @author CHTL Team
 * @version 1.0.0
 * @license MIT
 */

#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include <string>
#include <memory>

namespace CHTL {

/**
 * Token类型枚举
 * 严格按照CHTL语法文档定义
 */
enum class TokenType {
    // 基础Token类型
    UNKNOWN,        // 未知Token
    END_OF_FILE,   // 文件结束
    WHITESPACE,    // 空白字符
    NEWLINE,       // 换行符
    
    // 注释Token
    SINGLE_LINE_COMMENT,    // // 单行注释
    MULTI_LINE_COMMENT,     // /**/ 多行注释
    GENERATOR_COMMENT,      // -- 生成器注释
    
    // 字面量Token
    STRING_LITERAL,         // 字符串字面量（双引号、单引号）
    UNQUOTED_LITERAL,      // 无修饰字面量（无引号）
    NUMBER_LITERAL,        // 数字字面量
    
    // 分隔符和操作符
    LEFT_BRACE,            // {
    RIGHT_BRACE,           // }
    LEFT_BRACKET,          // [
    RIGHT_BRACKET,         // ]
    LEFT_PAREN,            // (
    RIGHT_PAREN,           // )
    SEMICOLON,             // ;
    COLON,                 // : (与=等价)
    EQUALS,                // = (与:等价)
    COMMA,                 // ,
    DOT,                   // .
    SLASH,                 // /
    BACKSLASH,             // \
    
    // 关键字Token - 模板系统
    TEMPLATE,              // [Template]
    TEMPLATE_STYLE,        // @Style
    TEMPLATE_ELEMENT,      // @Element
    TEMPLATE_VAR,          // @Var
    
    // 关键字Token - 自定义系统
    CUSTOM,                // [Custom]
    CUSTOM_STYLE,          // @Style (在[Custom]中)
    CUSTOM_ELEMENT,        // @Element (在[Custom]中)
    CUSTOM_VAR,            // @Var (在[Custom]中)
    
    // 关键字Token - 原始嵌入
    ORIGIN,                // [Origin]
    ORIGIN_HTML,           // @Html
    ORIGIN_STYLE,          // @Style (在[Origin]中)
    ORIGIN_JAVASCRIPT,     // @JavaScript
    
    // 关键字Token - 导入系统
    IMPORT,                 // [Import]
    IMPORT_HTML,            // @Html (在[Import]中)
    IMPORT_STYLE,           // @Style (在[Import]中)
    IMPORT_JAVASCRIPT,      // @JavaScript (在[Import]中)
    IMPORT_CHTL,            // @Chtl
    IMPORT_CJMOD,           // @CJmod
    IMPORT_CONFIG,          // @Config (在[Import]中)
    
    // 关键字Token - 配置系统
    CONFIGURATION,          // [Configuration]
    CONFIGURATION_CONFIG,   // @Config (在[Configuration]中)
    
    // 关键字Token - 命名空间
    NAMESPACE,              // [Namespace]
    
    // 关键字Token - 其他
    TEXT,                   // text
    STYLE,                  // style (局部样式块)
    SCRIPT,                 // script (局部脚本块)
    
    // 关键字Token - 操作符
    INHERIT,                // inherit
    DELETE,                 // delete
    INSERT,                 // insert
    AFTER,                  // after
    BEFORE,                 // before
    REPLACE,                // replace
    AT_TOP,                 // at top
    AT_BOTTOM,              // at bottom
    FROM,                   // from
    AS,                     // as
    EXCEPT,                 // except
    
    // 关键字Token - 配置组内部
    NAME,                   // [Name]
    ORIGIN_TYPE,            // [OriginType]
    
    // 关键字Token - 使用配置
    USE,                    // use
    HTML5,                  // html5
    
    // 标识符
    IDENTIFIER,             // 标识符（变量名、函数名等）
    
    // 特殊Token
    AMPERSAND,              // & (上下文推导)
    AT_SYMBOL,              // @ (模板前缀)
    HASH_SYMBOL,            // # (ID选择器)
    PERIOD,                 // . (类选择器)
    
    // 选择器相关
    CLASS_SELECTOR,         // .className
    ID_SELECTOR,            // #idName
    PSEUDO_CLASS,           // :pseudo-class
    PSEUDO_ELEMENT,         // ::pseudo-element
    
    // 模板使用Token
    TEMPLATE_REFERENCE,     // 模板引用（如@Style DefaultText）
    CUSTOM_REFERENCE,       // 自定义引用
    VARIABLE_REFERENCE,     // 变量引用
    
    // 继承和特例化Token
    INHERIT_REFERENCE,      // 继承引用
    DELETE_REFERENCE,       // 删除引用
    INSERT_REFERENCE,       // 插入引用
    
    // 索引访问Token
    INDEX_ACCESS,           // [index]
    
    // 通配符Token
    WILDCARD,               // * (通配符)
    WILDCARD_EXTENSION,     // *.扩展名
    
    // 路径分隔符
    PATH_SEPARATOR,         // / 或 . (路径分隔)
    
    // 模块前缀
    MODULE_PREFIX,          // chtl::
    
    // 配置值Token
    CONFIG_VALUE,           // 配置值
    CONFIG_ASSIGNMENT,      // 配置赋值
    CONFIG_OPTION_COUNT,    // 选项数量限制
};

/**
 * Token类
 * 表示词法分析器识别的一个Token
 */
class Token {
public:
    Token(TokenType type, const std::string& value, size_t line, size_t column);
    Token(const Token& other);
    Token& operator=(const Token& other);
    ~Token() = default;
    
    // 获取Token信息
    TokenType getType() const { return m_type; }
    const std::string& getValue() const { return m_value; }
    size_t getLine() const { return m_line; }
    size_t getColumn() const { return m_column; }
    
    // 设置Token信息
    void setType(TokenType type) { m_type = type; }
    void setValue(const std::string& value) { m_value = value; }
    void setPosition(size_t line, size_t column) { m_line = line; m_column = column; }
    
    // 判断Token类型
    bool isKeyword() const;
    bool isOperator() const;
    bool isLiteral() const;
    bool isIdentifier() const;
    bool isComment() const;
    
    // 获取Token类型名称（用于调试）
    std::string getTypeName() const;
    
    // 获取Token的字符串表示
    std::string toString() const;
    
private:
    TokenType m_type;           // Token类型
    std::string m_value;        // Token值
    size_t m_line;              // 行号（从1开始）
    size_t m_column;            // 列号（从1开始）
};

/**
 * Token工厂类
 * 用于创建各种类型的Token
 */
class TokenFactory {
public:
    // 创建基础Token
    static std::shared_ptr<Token> createToken(TokenType type, const std::string& value, size_t line, size_t column);
    
    // 创建关键字Token
    static std::shared_ptr<Token> createKeyword(const std::string& keyword, size_t line, size_t column);
    
    // 创建标识符Token
    static std::shared_ptr<Token> createIdentifier(const std::string& name, size_t line, size_t column);
    
    // 创建字面量Token
    static std::shared_ptr<Token> createStringLiteral(const std::string& value, size_t line, size_t column);
    static std::shared_ptr<Token> createUnquotedLiteral(const std::string& value, size_t line, size_t column);
    static std::shared_ptr<Token> createNumberLiteral(const std::string& value, size_t line, size_t column);
    
    // 创建操作符Token
    static std::shared_ptr<Token> createOperator(const std::string& op, size_t line, size_t column);
    
    // 创建分隔符Token
    static std::shared_ptr<Token> createDelimiter(const std::string& delimiter, size_t line, size_t column);
    
    // 创建注释Token
    static std::shared_ptr<Token> createComment(const std::string& comment, TokenType type, size_t line, size_t column);
    
    // 创建特殊Token
    static std::shared_ptr<Token> createSpecialToken(TokenType type, const std::string& value, size_t line, size_t column);
};

} // namespace CHTL

#endif // CHTL_TOKEN_H