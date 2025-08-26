#ifndef CHTLJS_SYNTAX_H
#define CHTLJS_SYNTAX_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <variant>
#include <unordered_map>

namespace chtl {
namespace js {

// 前向声明
class CHTLUnifiedScanner;

// Arg参数类型
class Arg {
public:
    Arg(const std::string& name = "", const std::string& value = "")
        : name(name), value(value), isPlaceholder(false) {}
    
    // 绑定函数
    void bind(std::function<std::string(const std::string&)> func) {
        bindFunction = func;
    }
    
    // 重载调用操作符
    void operator()(const std::string& input) {
        if (bindFunction) {
            transformedValue = bindFunction(input);
        } else {
            transformedValue = input;
        }
    }
    
    // 获取值
    std::string getValue() const { return value; }
    std::string getTransformedValue() const { return transformedValue; }
    std::string getName() const { return name; }
    bool getIsPlaceholder() const { return isPlaceholder; }
    
    // 设置为占位符
    void setAsPlaceholder() { isPlaceholder = true; }
    
    // 匹配并执行绑定函数
    void match(const std::string& input) {
        operator()(input);
    }
    
private:
    std::string name;
    std::string value;
    std::string transformedValue;
    bool isPlaceholder;
    std::function<std::string(const std::string&)> bindFunction;
};

// Syntax语法对象
class Syntax {
public:
    Syntax() : placeholderCount(0) {}
    
    // 参数列表
    std::vector<Arg> args;
    
    // 绑定函数到参数
    template<typename T>
    void bind(const std::string& name, std::function<std::string(T)> func) {
        // 查找参数
        for (auto& arg : args) {
            if (arg.getName() == name || (arg.getIsPlaceholder() && name == "$")) {
                // 如果是占位符，按顺序绑定
                if (arg.getIsPlaceholder() && name == "$") {
                    if (placeholderBindCount[&arg] == 0) {
                        arg.bind([func](const std::string& input) {
                            return func(input);
                        });
                        placeholderBindCount[&arg]++;
                        return;
                    }
                } else {
                    arg.bind([func](const std::string& input) {
                        return func(input);
                    });
                    return;
                }
            }
        }
    }
    
    // 通过索引绑定
    void bind(size_t index, std::function<std::string(const std::string&)> func) {
        if (index < args.size()) {
            args[index].bind(func);
        }
    }
    
    // 匹配参数
    void match(const std::string& name, const std::string& value) {
        for (auto& arg : args) {
            if (arg.getName() == name || (arg.getIsPlaceholder() && name == "$")) {
                arg.match(value);
                return;
            }
        }
    }
    
    // 通过索引匹配
    void match(size_t index, const std::string& value) {
        if (index < args.size()) {
            args[index].match(value);
        }
    }
    
    // 转换参数为JavaScript代码
    void transform(const std::string& name, const std::string& jsCode) {
        for (auto& arg : args) {
            if (arg.getName() == name) {
                arg(jsCode);  // 直接设置转换后的值
                return;
            }
        }
    }
    
    // 通过索引转换
    void transform(size_t index, const std::string& jsCode) {
        if (index < args.size()) {
            args[index](jsCode);
        }
    }
    
    // 获取结果
    std::string result() const {
        std::string result;
        for (const auto& arg : args) {
            result += arg.getTransformedValue();
        }
        return result;
    }
    
    // 生成代码
    std::string generateCode() const {
        return result();
    }
    
private:
    size_t placeholderCount;
    std::unordered_map<Arg*, int> placeholderBindCount;
};

// 语法分析函数
Syntax syntaxAnalys(const std::string& syntaxString, const std::string& ignoreChars = "");

// 辅助函数
bool isObject(const std::string& str);
bool isFunction(const std::string& str);
std::string slice(const std::string& str, size_t start, size_t end = std::string::npos);
std::string peekKeyword(int offset);

// 扫描器接口（用于CJMOD）
class CJMODScannerInterface {
public:
    CJMODScannerInterface(CHTLUnifiedScanner* scanner);
    
    // 扫描关键字
    void scanKeyword(const std::string& keyword, std::function<void()> callback);
    void scanKeyword(const Arg& arg, std::function<void()> callback);
    
    // 政策改变
    void policyChangeBegin(const std::string& delimiter, const std::string& policy);
    std::string policyChangeEnd(const std::string& delimiter, const std::string& policy);
    
    // 获取关键字
    std::string peekKeyword(int offset) const;
    
private:
    CHTLUnifiedScanner* scanner;
    std::vector<std::string> keywordHistory;
    size_t currentIndex;
};

// CJmod模块接口
class CJMODModule {
public:
    virtual ~CJMODModule() = default;
    
    // 模块信息
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // 注册语法
    virtual void registerSyntax(CJMODScannerInterface& scanner) = 0;
    
    // 生成代码
    virtual std::string generateCode(const Syntax& syntax) = 0;
};

// CJmod模块管理器
class CJMODManager {
public:
    static CJMODManager& getInstance();
    
    // 注册模块
    void registerModule(std::unique_ptr<CJMODModule> module);
    
    // 获取模块
    CJMODModule* getModule(const std::string& name) const;
    
    // 获取所有模块
    std::vector<std::string> getModuleNames() const;
    
private:
    CJMODManager() = default;
    std::unordered_map<std::string, std::unique_ptr<CJMODModule>> modules;
};

} // namespace js
} // namespace chtl

#endif // CHTLJS_SYNTAX_H