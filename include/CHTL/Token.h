#ifndef CHTL_TOKEN_H
#define CHTL_TOKEN_H

#include "Core/Common.h"

namespace CHTL {

// CHTL Token类型
enum class TokenType {
    // 基本标记
    EOF_TOKEN,              // 文件结束
    UNKNOWN,                // 未知token
    
    // 字面量
    IDENTIFIER,             // 标识符
    STRING_LITERAL,         // 字符串字面量（双引号）
    SINGLE_STRING_LITERAL,  // 字符串字面量（单引号）
    UNQUOTED_LITERAL,       // 无修饰字面量
    NUMBER,                 // 数字
    
    // 运算符和标点
    LEFT_BRACE,             // {
    RIGHT_BRACE,            // }
    LEFT_BRACKET,           // [
    RIGHT_BRACKET,          // ]
    LEFT_PAREN,             // (
    RIGHT_PAREN,            // )
    SEMICOLON,              // ;
    COLON,                  // :
    EQUALS,                 // =
    COMMA,                  // ,
    DOT,                    // .
    AT,                     // @
    AMPERSAND,              // &
    EXCLAMATION,            // !
    
    // CHTL关键字
    TEXT,                   // text
    STYLE,                  // style
    SCRIPT,                 // script
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
    USE,                    // use
    HTML5,                  // html5
    
    // CHTL块标记
    TEMPLATE_MARKER,        // [Template]
    CUSTOM_MARKER,          // [Custom]
    ORIGIN_MARKER,          // [Origin]
    IMPORT_MARKER,          // [Import]
    NAMESPACE_MARKER,       // [Namespace]
    CONFIGURATION_MARKER,   // [Configuration]
    INFO_MARKER,            // [Info]
    EXPORT_MARKER,          // [Export]
    
    // @类型标记
    AT_STYLE,               // @Style
    AT_ELEMENT,             // @Element
    AT_VAR,                 // @Var
    AT_HTML,                // @Html
    AT_JAVASCRIPT,          // @JavaScript
    AT_CHTL,                // @Chtl
    AT_CJMOD,               // @CJmod
    AT_CONFIG,              // @Config
    
    // 注释
    LINE_COMMENT,           // // 注释
    BLOCK_COMMENT,          // /* 注释 */
    GENERATOR_COMMENT,      // -- 生成器注释
    
    // 特殊
    WHITESPACE,             // 空白字符
    NEWLINE,                // 换行符
};

// Token结构
struct Token {
    TokenType type;
    String value;
    SourceLocation location;
    
    Token() : type(TokenType::UNKNOWN) {}
    
    Token(TokenType t, const String& v, const SourceLocation& loc)
        : type(t), value(v), location(loc) {}
    
    // 判断是否是关键字
    bool IsKeyword() const;
    
    // 判断是否是块标记
    bool IsBlockMarker() const;
    
    // 判断是否是@类型标记
    bool IsAtTypeMarker() const;
    
    // 判断是否是字面量
    bool IsLiteral() const;
    
    // 判断是否是注释
    bool IsComment() const;
    
    // 获取Token类型名称
    String GetTypeName() const;
    
    // 判断两个Token是否相等（用于CE对等式）
    bool IsEquivalent(const Token& other) const;
};

// Token工具类
class TokenUtils {
public:
    // 将字符串转换为Token类型
    static TokenType StringToTokenType(const String& str);
    
    // 获取Token类型的字符串表示
    static String TokenTypeToString(TokenType type);
    
    // 判断字符串是否是CHTL关键字
    static bool IsKeyword(const String& str);
    
    // 判断字符串是否是块标记
    static bool IsBlockMarker(const String& str);
    
    // 判断字符串是否是@类型标记
    static bool IsAtTypeMarker(const String& str);
    
    // 获取所有CHTL关键字列表
    static Vector<String> GetKeywords();
    
    // 获取所有块标记列表
    static Vector<String> GetBlockMarkers();
    
    // 获取所有@类型标记列表
    static Vector<String> GetAtTypeMarkers();
    
    // 处理CE对等式（冒号和等号等价）
    static bool AreEquivalent(TokenType t1, TokenType t2);
};

// Token流
class TokenStream {
private:
    Vector<Token> tokens_;
    size_t current_ = 0;
    
public:
    // 添加Token
    void AddToken(const Token& token);
    void AddToken(TokenType type, const String& value, const SourceLocation& location);
    
    // 获取当前Token
    const Token& Current() const;
    
    // 获取下一个Token
    const Token& Next();
    
    // 查看下一个Token（不移动位置）
    const Token& Peek(size_t offset = 1) const;
    
    // 检查是否到达末尾
    bool IsAtEnd() const;
    
    // 重置位置
    void Reset() { current_ = 0; }
    
    // 获取当前位置
    size_t GetPosition() const { return current_; }
    
    // 设置位置
    void SetPosition(size_t pos) { current_ = pos; }
    
    // 获取所有Token
    const Vector<Token>& GetTokens() const { return tokens_; }
    
    // 清空Token流
    void Clear();
    
    // 获取Token数量
    size_t Size() const { return tokens_.size(); }
    
    // 匹配Token类型
    bool Match(TokenType type);
    bool Match(const Vector<TokenType>& types);
    
    // 消费Token
    Token Consume(TokenType type, const String& errorMessage);
    
    // 同步（用于错误恢复）
    void Synchronize();
};

} // namespace CHTL

#endif // CHTL_TOKEN_H