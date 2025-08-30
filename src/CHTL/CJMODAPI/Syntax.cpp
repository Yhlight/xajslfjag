#include "Syntax.h"
#include <regex>
#include <sstream>
#include <stack>

namespace CHTL {
namespace CJMOD {

Arg Syntax::analyze(const std::string& pattern) {
    Arg args;
    std::vector<std::string> tokens = parsePattern(pattern);
    
    size_t position = 0;
    for (const auto& token : tokens) {
        args.addItem(ArgItem(token, position++));
    }
    
    return args;
}

bool Syntax::isObject(const std::string& code) {
    // 简单的对象检测：以{开始，以}结束
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '{' && trimmed.back() == '}' && validateBrackets(trimmed);
}

bool Syntax::isFunction(const std::string& code) {
    // 检测function关键字或箭头函数
    std::regex functionRegex(R"(^\s*(function\s+\w*\s*\(|const\s+\w+\s*=\s*\(|\w+\s*=>\s*))");
    return std::regex_search(code, functionRegex);
}

bool Syntax::isArray(const std::string& code) {
    // 简单的数组检测：以[开始，以]结束
    std::string trimmed = code;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '[' && trimmed.back() == ']' && validateBrackets(trimmed);
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // CHTL JS函数格式：functionName {param1: value1, param2: value2}
    std::regex cjtlFuncRegex(R"(^\s*\w+\s*\{[^}]*\}\s*;?\s*$)");
    return std::regex_match(code, cjtlFuncRegex);
}

bool Syntax::matches(const std::string& code, const std::string& pattern) {
    Arg patternArgs = analyze(pattern);
    // TODO: 实现完整的模式匹配
    return true;
}

std::vector<std::string> Syntax::parsePattern(const std::string& pattern) {
    std::vector<std::string> tokens;
    std::istringstream stream(pattern);
    std::string token;
    
    while (stream >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Syntax::extractCodeFragment(const std::string& code, size_t start, size_t end) {
    if (start >= code.length() || end > code.length() || start >= end) {
        return "";
    }
    return code.substr(start, end - start);
}

bool Syntax::validateBrackets(const std::string& code) {
    std::stack<char> brackets;
    
    for (char c : code) {
        if (c == '(' || c == '[' || c == '{') {
            brackets.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (brackets.empty()) return false;
            
            char open = brackets.top();
            brackets.pop();
            
            if ((c == ')' && open != '(') ||
                (c == ']' && open != '[') ||
                (c == '}' && open != '{')) {
                return false;
            }
        }
    }
    
    return brackets.empty();
}

} // namespace CJMOD
} // namespace CHTL