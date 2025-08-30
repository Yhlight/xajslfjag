#include "Syntax.h"
#include <regex>
#include <algorithm>

namespace CJMOD {

Syntax::Syntax() {
    initializePatterns();
}

void Syntax::initializePatterns() {
    // 初始化基础语法模式
    patterns["function"] = std::regex(R"(\w+\s*\([^)]*\)\s*\{)");
    patterns["variable"] = std::regex(R"(\w+\s*=\s*.+)");
    patterns["string"] = std::regex(R"(["'].*?["'])");
    patterns["number"] = std::regex(R"(\d+(?:\.\d+)?)");
    patterns["keyword"] = std::regex(R"(\b(?:if|else|for|while|return|const|let|var)\b)");
}

std::vector<AtomArg> Syntax::analyze(const std::string& input) {
    std::vector<AtomArg> atoms;
    
    // 简单的语法分析
    std::string trimmed = input;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.empty()) {
        return atoms;
    }
    
    // 检查是否为函数
    if (isFunction(trimmed)) {
        atoms.emplace_back(AtomArg::Type::Function, trimmed);
    }
    // 检查是否为变量
    else if (isVariable(trimmed)) {
        atoms.emplace_back(AtomArg::Type::Variable, trimmed);
    }
    // 检查是否为字符串
    else if (isString(trimmed)) {
        atoms.emplace_back(AtomArg::Type::String, trimmed);
    }
    // 检查是否为数字
    else if (isNumber(trimmed)) {
        atoms.emplace_back(AtomArg::Type::Number, trimmed);
    }
    // 检查是否为关键字
    else if (isKeyword(trimmed)) {
        atoms.emplace_back(AtomArg::Type::Keyword, trimmed);
    }
    // 默认为标识符
    else {
        atoms.emplace_back(AtomArg::Type::Identifier, trimmed);
    }
    
    return atoms;
}

bool Syntax::isFunction(const std::string& input) const {
    return std::regex_search(input, patterns.at("function"));
}

bool Syntax::isVariable(const std::string& input) const {
    return std::regex_search(input, patterns.at("variable"));
}

bool Syntax::isString(const std::string& input) const {
    return std::regex_search(input, patterns.at("string"));
}

bool Syntax::isNumber(const std::string& input) const {
    return std::regex_search(input, patterns.at("number"));
}

bool Syntax::isKeyword(const std::string& input) const {
    return std::regex_search(input, patterns.at("keyword"));
}

bool Syntax::isCHTLJSFunction(const std::string& input) const {
    // 检查CHTL JS函数语法
    return input.find("listen") != std::string::npos ||
           input.find("animate") != std::string::npos ||
           input.find("delegate") != std::string::npos ||
           input.find("vir") != std::string::npos;
}

bool Syntax::isArray(const std::string& input) const {
    std::string trimmed = input;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']';
}

std::string Syntax::extractFunctionName(const std::string& input) const {
    std::smatch match;
    std::regex funcNameRegex(R"((\w+)\s*\()");
    
    if (std::regex_search(input, match, funcNameRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::vector<std::string> Syntax::extractFunctionArgs(const std::string& input) const {
    std::vector<std::string> args;
    
    size_t start = input.find('(');
    size_t end = input.find(')', start);
    
    if (start != std::string::npos && end != std::string::npos) {
        std::string argsStr = input.substr(start + 1, end - start - 1);
        
        // 简单的参数分割
        std::stringstream ss(argsStr);
        std::string arg;
        while (std::getline(ss, arg, ',')) {
            arg.erase(0, arg.find_first_not_of(" \t"));
            arg.erase(arg.find_last_not_of(" \t") + 1);
            if (!arg.empty()) {
                args.push_back(arg);
            }
        }
    }
    
    return args;
}

}  // namespace CJMOD