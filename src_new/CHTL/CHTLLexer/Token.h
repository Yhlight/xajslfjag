#pragma once

#include <string>

namespace CHTL {
namespace Lexer {

/**
 * TokenType - CHTL Token类型枚举
 * 新目录结构下的完整Token定义
 */
enum class TokenType {
    // 基础Token
    IDENTIFIER,         // 标识符
    STRING_LITERAL,     // 字符串字面量
    NUMBER_LITERAL,     // 数字字面量
    BOOLEAN_LITERAL,    // 布尔字面量
    
    // 运算符
    ASSIGN,             // =
    COLON,              // :
    SEMICOLON,          // ;
    COMMA,              // ,
    DOT,                // .
    ARROW,              // ->
    
    // 括号
    LEFT_PAREN,         // (
    RIGHT_PAREN,        // )
    LEFT_BRACKET,       // [
    RIGHT_BRACKET,      // ]
    LEFT_BRACE,         // {
    RIGHT_BRACE,        // }
    
    // CHTL关键字
    NAMESPACE,          // [Namespace]
    IMPORT,             // [Import]
    CONFIGURATION,      // [Configuration]
    TEMPLATE,           // [Template]
    CUSTOM,             // [Custom]
    ORIGIN,             // [Origin]
    INFO,               // [Info]
    EXPORT,             // [Export]
    
    // CHTL类型
    VAR,                // @Var
    STYLE,              // @Style
    ELEMENT,            // @Element
    HTML,               // @Html
    JAVASCRIPT,         // @JavaScript
    CHTL_MODULE,        // @Chtl
    CJMOD,              // @CJmod
    CONFIG,             // @Config
    
    // CHTL操作
    INHERIT,            // inherit
    DELETE,             // delete
    INSERT,             // insert
    AFTER,              // after
    BEFORE,             // before
    REPLACE,            // replace
    AT_TOP,             // at top
    AT_BOTTOM,          // at bottom
    USE,                // use
    EXCEPT,             // except
    
    // HTML标签
    HTML_TAG_START,     // <tag>
    HTML_TAG_END,       // </tag>
    HTML_SELF_CLOSE,    // <tag />
    
    // 特殊标记
    TEXT_BLOCK_START,   // text {
    STYLE_BLOCK_START,  // style {
    SCRIPT_BLOCK_START, // script {
    
    // 索引访问
    INDEX_ACCESS,       // [0], [1], etc.
    
    // 注释
    COMMENT,            // -- 注释
    
    // 文件结束
    EOF_TOKEN,          // 文件结束
    
    // 错误
    INVALID,            // 无效Token
    
    // 无修饰字面量标记
    UNQUOTED_LITERAL,   // 无修饰字符串
    
    // 增强选择器（用于检测CHTL JS）
    ENHANCED_SELECTOR,  // {{selector}}
    
    // 官方模块前缀
    OFFICIAL_PREFIX     // chtl::
};

/**
 * Token - CHTL词法单元
 * 新目录结构下的Token实现
 */
class Token {
private:
    TokenType type_;
    std::string value_;
    std::string file_path_;
    size_t line_number_;
    size_t column_number_;
    size_t start_position_;
    size_t end_position_;
    
public:
    /**
     * 构造函数
     * @param type Token类型
     * @param value Token值
     * @param file_path 文件路径
     * @param line 行号
     * @param column 列号
     * @param start 起始位置
     * @param end 结束位置
     */
    Token(TokenType type = TokenType::INVALID,
          const std::string& value = "",
          const std::string& file_path = "",
          size_t line = 0,
          size_t column = 0,
          size_t start = 0,
          size_t end = 0);
    
    /**
     * 获取Token类型
     * @return Token类型
     */
    TokenType getType() const;
    
    /**
     * 获取Token值
     * @return Token值
     */
    const std::string& getValue() const;
    
    /**
     * 获取文件路径
     * @return 文件路径
     */
    const std::string& getFilePath() const;
    
    /**
     * 获取行号
     * @return 行号
     */
    size_t getLineNumber() const;
    
    /**
     * 获取列号
     * @return 列号
     */
    size_t getColumnNumber() const;
    
    /**
     * 获取起始位置
     * @return 起始位置
     */
    size_t getStartPosition() const;
    
    /**
     * 获取结束位置
     * @return 结束位置
     */
    size_t getEndPosition() const;
    
    /**
     * 是否为关键字
     * @return 是否为关键字
     */
    bool isKeyword() const;
    
    /**
     * 是否为操作符
     * @return 是否为操作符
     */
    bool isOperator() const;
    
    /**
     * 是否为字面量
     * @return 是否为字面量
     */
    bool isLiteral() const;
    
    /**
     * 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const;
    
    /**
     * Token类型转换为字符串
     * @param type Token类型
     * @return 字符串表示
     */
    static std::string typeToString(TokenType type);
};

} // namespace Lexer
} // namespace CHTL