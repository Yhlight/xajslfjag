#pragma once

#include <string>
#include <memory>
#include "Location.h"

namespace CHTL {

// Token基类
class Token {
public:
    enum class Type {
        // 通用Token类型
        UNKNOWN,
        END_OF_FILE,
        
        // 字面量
        IDENTIFIER,
        STRING_LITERAL,
        NUMBER_LITERAL,
        UNQUOTED_LITERAL,  // 无修饰字面量
        
        // 符号
        LEFT_BRACE,         // {
        RIGHT_BRACE,        // }
        LEFT_PAREN,         // (
        RIGHT_PAREN,        // )
        LEFT_BRACKET,       // [
        RIGHT_BRACKET,      // ]
        SEMICOLON,          // ;
        COLON,              // :
        EQUALS,             // =
        COMMA,              // ,
        DOT,                // .
        ARROW,              // ->
        AMPERSAND,          // &
        DOLLAR,             // $
        AT,                 // @
        HASH,               // #
        
        // 注释
        COMMENT_SINGLE,     // //
        COMMENT_MULTI,      // /* */
        COMMENT_GENERATOR,  // --
        
        // 关键字基础定义（具体关键字在子类中定义）
        KEYWORD_START = 1000,
        KEYWORD_END = 2000,
        
        // CHTL特有Token起始
        CHTL_TOKEN_START = 2000,
        
        // CHTL JS特有Token起始
        CHTLJS_TOKEN_START = 3000
    };
    
    Token(Type type, const std::string& lexeme, const Location& location)
        : type_(type), lexeme_(lexeme), location_(location) {}
    
    virtual ~Token() = default;
    
    Type GetType() const { return type_; }
    const std::string& GetLexeme() const { return lexeme_; }
    const Location& GetLocation() const { return location_; }
    
    bool IsKeyword() const {
        return type_ >= Type::KEYWORD_START && type_ < Type::KEYWORD_END;
    }
    
    bool IsCHTLToken() const {
        return type_ >= Type::CHTL_TOKEN_START && type_ < Type::CHTLJS_TOKEN_START;
    }
    
    bool IsCHTLJSToken() const {
        return type_ >= Type::CHTLJS_TOKEN_START;
    }
    
    virtual std::string ToString() const;
    
protected:
    Type type_;
    std::string lexeme_;
    Location location_;
};

using TokenPtr = std::shared_ptr<Token>;

} // namespace CHTL