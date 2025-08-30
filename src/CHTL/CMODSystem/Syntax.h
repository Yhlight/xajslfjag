#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

namespace CJMOD {

// 原子参数类型
struct AtomArg {
    enum class Type {
        Function,
        Variable,
        String,
        Number,
        Keyword,
        Identifier,
        Array,
        Object
    };
    
    Type type;
    std::string content;
    
    AtomArg(Type t, const std::string& c) : type(t), content(c) {}
};

// 语法分析器
class Syntax {
private:
    std::unordered_map<std::string, std::regex> patterns;
    
public:
    Syntax();
    
    // 分析输入并返回原子参数
    std::vector<AtomArg> analyze(const std::string& input);
    
    // 语法检查方法
    bool isFunction(const std::string& input) const;
    bool isVariable(const std::string& input) const;
    bool isString(const std::string& input) const;
    bool isNumber(const std::string& input) const;
    bool isKeyword(const std::string& input) const;
    bool isCHTLJSFunction(const std::string& input) const;
    bool isArray(const std::string& input) const;
    
    // 提取方法
    std::string extractFunctionName(const std::string& input) const;
    std::vector<std::string> extractFunctionArgs(const std::string& input) const;
    
private:
    void initializePatterns();
};

}  // namespace CJMOD