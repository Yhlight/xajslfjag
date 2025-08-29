#pragma once

#include "../Util/Common.h"
#include <functional>
#include <regex>

namespace CJMOD {

// 前置声明
class Arg;
class AtomArg;

// Syntax类 - 语法分析类
class Syntax {
public:
    // 核心静态方法
    static Arg analyze(const CHTL::String& pattern);
    static bool isObject(const CHTL::String& content);
    static bool isFunction(const CHTL::String& content);
    static bool isArray(const CHTL::String& content);
    static bool isCHTLJSFunction(const CHTL::String& content);
    
private:
    // 辅助方法
    static CHTL::StringVector tokenizePattern(const CHTL::String& pattern);
    static bool isValidJSObject(const CHTL::String& content);
    static bool isValidJSFunction(const CHTL::String& content);
    static bool isValidJSArray(const CHTL::String& content);
    static bool isValidCHTLJSFunction(const CHTL::String& content);
    
    // 模式匹配
    static std::regex getObjectPattern();
    static std::regex getFunctionPattern();
    static std::regex getArrayPattern();
    static std::regex getCHTLJSFunctionPattern();
};

// AtomArg类 - 原子参数
class AtomArg {
public:
    enum class PlaceholderType {
        NORMAL,        // $
        OPTIONAL,      // $?
        REQUIRED,      // $!
        UNORDERED,     // $_
        VARIADIC       // ...
    };
    
    CHTL::String placeholder;
    CHTL::String value;
    PlaceholderType type;
    std::function<CHTL::String(const CHTL::String&)> bindFunction;
    bool isBound;
    bool isFilled;
    
    explicit AtomArg(const CHTL::String& ph = "", PlaceholderType t = PlaceholderType::NORMAL);
    
    // 绑定和填充
    void bind(std::function<CHTL::String(const CHTL::String&)> func);
    void fillValue(const CHTL::String& val);
    void fillValue(int val);
    void fillValue(double val);
    void fillValue(bool val);
    
    // 状态检查
    bool hasValue() const { return isFilled; }
    bool hasBinder() const { return isBound; }
    CHTL::String getValue() const;
    
    // 类型转换
    CHTL::String toString() const;
    static PlaceholderType parsePlaceholderType(const CHTL::String& placeholder);
    static CHTL::String placeholderTypeToString(PlaceholderType type);
};

// Arg类 - 参数列表类
class Arg {
public:
    std::vector<AtomArg> atoms;
    CHTL::String transformPattern;
    bool isTransformed;
    
    Arg() : isTransformed(false) {}
    explicit Arg(const CHTL::StringVector& placeholders);
    explicit Arg(const std::vector<AtomArg>& atomList);
    
    // 访问操作
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    AtomArg& at(size_t index);
    const AtomArg& at(size_t index) const;
    size_t size() const { return atoms.size(); }
    bool empty() const { return atoms.empty(); }
    
    // 绑定操作
    void bind(const CHTL::String& placeholder, std::function<CHTL::String(const CHTL::String&)> func);
    void bindAll(std::function<CHTL::String(const CHTL::String&)> func);
    void bindByIndex(size_t index, std::function<CHTL::String(const CHTL::String&)> func);
    
    // 填充操作
    void fillValue(const Arg& other);
    void fillValue(const CHTL::StringVector& values);
    void fillValueByPlaceholder(const CHTL::String& placeholder, const CHTL::String& value);
    void fillValueByIndex(size_t index, const CHTL::String& value);
    
    // 转换操作
    void transform(const CHTL::String& pattern);
    CHTL::String getTransformedResult() const;
    
    // 验证和状态
    bool isComplete() const;
    bool hasUnboundPlaceholders() const;
    bool hasUnfilledRequired() const;
    CHTL::StringVector getUnboundPlaceholders() const;
    CHTL::StringVector getUnfilledRequired() const;
    
    // 输出和调试
    void print() const;
    CHTL::String toString() const;
    CHTL::String toDebugString() const;
    
    // 查找和操作
    AtomArg* findByPlaceholder(const CHTL::String& placeholder);
    const AtomArg* findByPlaceholder(const CHTL::String& placeholder) const;
    bool hasPlaceholder(const CHTL::String& placeholder) const;
    void removePlaceholder(const CHTL::String& placeholder);
    
    // 迭代器支持
    std::vector<AtomArg>::iterator begin() { return atoms.begin(); }
    std::vector<AtomArg>::iterator end() { return atoms.end(); }
    std::vector<AtomArg>::const_iterator begin() const { return atoms.begin(); }
    std::vector<AtomArg>::const_iterator end() const { return atoms.end(); }
    
    // CJMOD特殊功能
    void clear() { atoms.clear(); isTransformed = false; transformPattern.clear(); }
    void addAtomArg(const AtomArg& atom) { atoms.push_back(atom); }
    CHTL::String getSourceCode() const;  // 获取源代码用于扫描
    void addError(const CHTL::String& error);  // 添加错误信息
    
private:
    // 辅助方法
    void expandVariadicArgs(const CHTL::StringVector& values);
    CHTL::String applyTransformPattern() const;
    void validatePlaceholders() const;
};

} // namespace CJMOD