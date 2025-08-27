#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>

namespace CHTL {

/**
 * 基础类型定义
 * 严格按照CHTL语法文档定义
 */

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    std::string filename;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o, const std::string& f = "")
        : line(l), column(c), offset(o), filename(f) {}
};

// 源码范围
struct SourceRange {
    Position start;
    Position end;
    
    SourceRange() = default;
    SourceRange(const Position& s, const Position& e) : start(s), end(e) {}
};

// 字面量类型 (支持无修饰字符串)
enum class LiteralType {
    UNQUOTED,       // 无修饰字面量: HelloWorld, red, 100px
    DOUBLE_QUOTED,  // 双引号字符串: "Hello World"
    SINGLE_QUOTED   // 单引号字符串: 'Hello World'
};

// 字面量值
struct Literal {
    std::string value;
    LiteralType type;
    SourceRange range;
    
    Literal() : type(LiteralType::UNQUOTED) {}
    Literal(const std::string& v, LiteralType t) : value(v), type(t) {}
};

// 属性值 (支持CE对等式)
struct AttributeValue {
    Literal value;
    bool uses_colon = true;  // true for ':', false for '='
    SourceRange range;
};

// 注释类型
enum class CommentType {
    LINE_COMMENT,        // // 注释
    BLOCK_COMMENT,       // /* */ 注释  
    GENERATOR_COMMENT    // -- 生成器识别的注释
};

// 注释
struct Comment {
    std::string content;
    CommentType type;
    SourceRange range;
};

// 错误信息
struct CompileError {
    std::string message;
    SourceRange range;
    std::string suggestion;
    
    CompileError(const std::string& msg, const SourceRange& r, const std::string& suggest = "")
        : message(msg), range(r), suggestion(suggest) {}
};

// 警告信息
struct CompileWarning {
    std::string message;
    SourceRange range;
    std::string suggestion;
    
    CompileWarning(const std::string& msg, const SourceRange& r, const std::string& suggest = "")
        : message(msg), range(r), suggestion(suggest) {}
};

// 编译结果
struct CompileResult {
    bool success = false;
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<CompileError> errors;
    std::vector<CompileWarning> warnings;
};

} // namespace CHTL