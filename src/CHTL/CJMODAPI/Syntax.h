#ifndef CHTL_CJMOD_SYNTAX_H
#define CHTL_CJMOD_SYNTAX_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "Arg.h"

namespace CHTL {
namespace CJMOD {

// 语法分析类
class Syntax {
public:
    // 分析语法，返回参数列表
    static Arg analyze(const std::string& pattern);
    
    // JavaScript类型检查
    static bool isObject(const std::string& code);
    static bool isFunction(const std::string& code);
    static bool isArray(const std::string& code);
    static bool isCHTLJSFunction(const std::string& code);
    
    // 语法模式匹配
    static bool matches(const std::string& code, const std::string& pattern);
    
private:
    // 解析语法模式
    static std::vector<std::string> parsePattern(const std::string& pattern);
    
    // 提取代码片段
    static std::string extractCodeFragment(const std::string& code, size_t start, size_t end);
    
    // 验证括号匹配
    static bool validateBrackets(const std::string& code);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_SYNTAX_H