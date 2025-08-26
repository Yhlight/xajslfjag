#ifndef CHTL_CORE_COMMON_H
#define CHTL_CORE_COMMON_H

// 标准库头文件
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <variant>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <chrono>
#include <stdexcept>

// UTF-8支持
#ifdef _WIN32
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#endif

namespace CHTL {

// 版本信息
constexpr const char* VERSION = "1.0.0";
constexpr int VERSION_MAJOR = 1;
constexpr int VERSION_MINOR = 0;
constexpr int VERSION_PATCH = 0;

// 平台定义
#ifdef CHTL_PLATFORM_WINDOWS
    constexpr const char* PLATFORM = "Windows";
    constexpr const char* PATH_SEPARATOR = "\\";
    constexpr const char PATH_SEPARATOR_CHAR = '\\';
#else
    constexpr const char* PLATFORM = "Linux";
    constexpr const char* PATH_SEPARATOR = "/";
    constexpr const char PATH_SEPARATOR_CHAR = '/';
#endif

// UTF-8字符串类型别名
using String = std::string;
using StringView = std::string_view;
using WString = std::wstring;
using Path = std::filesystem::path;

// 容器类型别名
template<typename T>
using Vector = std::vector<T>;

template<typename K, typename V>
using HashMap = std::unordered_map<K, V>;

template<typename T>
using HashSet = std::unordered_set<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using Optional = std::optional<T>;

template<typename... Types>
using Variant = std::variant<Types...>;

// 位置信息
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    
    Position() = default;
    Position(size_t l, size_t c, size_t o = 0) 
        : line(l), column(c), offset(o) {}
};

// 范围信息
struct Range {
    Position start;
    Position end;
    
    Range() = default;
    Range(const Position& s, const Position& e) 
        : start(s), end(e) {}
};

// 源码位置信息
struct SourceLocation {
    String filename;
    Range range;
    
    SourceLocation() = default;
    SourceLocation(const String& file, const Range& r) 
        : filename(file), range(r) {}
};

// 初始化UTF-8支持
inline void InitializeUTF8Support() {
#ifdef _WIN32
    // Windows控制台UTF-8支持
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    // 设置C运行时UTF-8
    _setmode(_fileno(stdout), _O_U8TEXT);
    _setmode(_fileno(stdin), _O_U8TEXT);
    _setmode(_fileno(stderr), _O_U8TEXT);
#endif
    // 设置C++ locale
    std::locale::global(std::locale("en_US.UTF-8"));
}

// 错误处理基类
class CHTLException : public std::runtime_error {
protected:
    SourceLocation location_;
    
public:
    explicit CHTLException(const String& message) 
        : std::runtime_error(message) {}
    
    CHTLException(const String& message, const SourceLocation& loc)
        : std::runtime_error(message), location_(loc) {}
    
    const SourceLocation& GetLocation() const { return location_; }
};

// 编译错误
class CompileError : public CHTLException {
public:
    using CHTLException::CHTLException;
};

// 语法错误
class SyntaxError : public CompileError {
public:
    using CompileError::CompileError;
};

// 语义错误
class SemanticError : public CompileError {
public:
    using CompileError::CompileError;
};

// 导入错误
class ImportError : public CompileError {
public:
    using CompileError::CompileError;
};

// 结果类型模板
template<typename T, typename E = String>
class Result {
private:
    Variant<T, E> value_;
    
public:
    Result(T&& value) : value_(std::move(value)) {}
    Result(const T& value) : value_(value) {}
    Result(E&& error) : value_(std::move(error)) {}
    Result(const E& error) : value_(error) {}
    
    bool IsOk() const { return std::holds_alternative<T>(value_); }
    bool IsError() const { return std::holds_alternative<E>(value_); }
    
    T& Value() { return std::get<T>(value_); }
    const T& Value() const { return std::get<T>(value_); }
    
    E& Error() { return std::get<E>(value_); }
    const E& Error() const { return std::get<E>(value_); }
    
    T ValueOr(T&& default_value) const {
        return IsOk() ? Value() : std::move(default_value);
    }
};

// 创建成功结果
template<typename T>
Result<T> Ok(T&& value) {
    return Result<T>(std::forward<T>(value));
}

// 创建错误结果
template<typename T, typename E = String>
Result<T, E> Err(E&& error) {
    return Result<T, E>(std::forward<E>(error));
}

} // namespace CHTL

#endif // CHTL_CORE_COMMON_H