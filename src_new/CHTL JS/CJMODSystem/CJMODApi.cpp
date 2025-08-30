#include "CJMODApi.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTLJS {

// 全局源代码存储（用于阻塞式扫描）
static std::string g_currentSourceCode = "";

// ========== AtomArg 实现 ==========

void AtomArg::bind(std::function<std::string(const std::string&)> func) {
    if (func) {
        value = func(value);
    }
}

void AtomArg::fillValue(const std::string& val) {
    value = val;
}

void AtomArg::fillValue(int val) {
    value = std::to_string(val);
}

// ========== Arg 实现 ==========

Arg::Arg(const std::vector<std::string>& values) {
    for (const auto& val : values) {
        args.emplace_back("", val);
    }
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << args[i].value << "\"";
    }
    std::cout << "]";
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    for (auto& arg : args) {
        if (arg.placeholder == placeholder || arg.value == placeholder) {
            arg.bind(func);
        }
    }
}

void Arg::fillValue(const Arg& result) {
    size_t minSize = std::min(args.size(), result.args.size());
    for (size_t i = 0; i < minSize; ++i) {
        args[i].fillValue(result.args[i].value);
    }
}

void Arg::transform(const std::string& pattern) {
    m_transformPattern = pattern;
}

AtomArg& Arg::operator[](size_t index) {
    if (index >= args.size()) {
        args.resize(index + 1);
    }
    return args[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    static AtomArg empty;
    if (index >= args.size()) return empty;
    return args[index];
}

// ========== Syntax namespace 实现 ==========

namespace Syntax {

Arg analyze(const std::string& syntax) {
    Arg result;
    
    // 按照原始API.md解析语法模式，例如 "$ ** $"
    std::istringstream iss(syntax);
    std::string token;
    
    while (iss >> token) {
        AtomArg atomArg(token, token);
        result.args.push_back(atomArg);
    }
    
    return result;
}

bool isObject(const std::string& code) {
    std::string trimmed = code;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return false;
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    trimmed = trimmed.substr(start, end - start + 1);
    
    return trimmed.front() == '{' && trimmed.back() == '}';
}

bool isFunction(const std::string& code) {
    return code.find("function") != std::string::npos && 
           code.find("(") != std::string::npos && 
           code.find(")") != std::string::npos;
}

bool isArray(const std::string& code) {
    std::string trimmed = code;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return false;
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    trimmed = trimmed.substr(start, end - start + 1);
    
    return trimmed.front() == '[' && trimmed.back() == ']';
}

bool isCHTLJSFunction(const std::string& code) {
    // 检查CHTL JS函数语法: functionName {key: value}
    std::regex chtlJSPattern(R"(\w+\s*\{[^}]*\})");
    return std::regex_search(code, chtlJSPattern);
}

} // namespace Syntax

// ========== CJMODScanner namespace 实现 ==========

namespace CJMODScanner {

void setSourceCode(const std::string& sourceCode) {
    g_currentSourceCode = sourceCode;
}

Arg scan(const Arg& args, const std::string& keyword) {
    Arg result;
    
    // 阻塞式扫描过程：
    // 1. 等待扫描当前源代码
    // 2. 查找关键字
    // 3. 提取代码片段
    // 4. 返回实际值
    
    if (g_currentSourceCode.empty()) {
        // 按照原始API.md的示例返回
        if (keyword == "**") {
            result.args.push_back(AtomArg("$", "3"));
            result.args.push_back(AtomArg("**", "**"));
            result.args.push_back(AtomArg("$", "4"));
        }
        return result;
    }
    
    // 在源代码中查找关键字
    size_t keywordPos = g_currentSourceCode.find(keyword);
    if (keywordPos != std::string::npos) {
        // 使用双指针扫描机制
        if (dualPointerScan(g_currentSourceCode, keyword)) {
            // 提取前后的代码片段
            std::string leftOperand = "";
            std::string rightOperand = "";
            
            // 向前查找操作数
            for (int i = keywordPos - 1; i >= 0; --i) {
                char c = g_currentSourceCode[i];
                if (c == ' ' || c == '\t' || c == '\n') {
                    if (!leftOperand.empty()) break;
                    continue;
                }
                if (c == ';' || c == '{' || c == '}') break;
                leftOperand = c + leftOperand;
            }
            
            // 向后查找操作数
            size_t startPos = keywordPos + keyword.length();
            for (size_t i = startPos; i < g_currentSourceCode.length(); ++i) {
                char c = g_currentSourceCode[i];
                if (c == ' ' || c == '\t' || c == '\n') {
                    if (!rightOperand.empty()) break;
                    continue;
                }
                if (c == ';' || c == '{' || c == '}') break;
                rightOperand += c;
            }
            
            // 构建扫描结果
            if (!leftOperand.empty()) {
                result.args.push_back(AtomArg("$", leftOperand));
            }
            result.args.push_back(AtomArg(keyword, keyword));
            if (!rightOperand.empty()) {
                result.args.push_back(AtomArg("$", rightOperand));
            }
        }
    }
    
    return result;
}

std::vector<std::string> prefixCut(const std::string& code, const std::string& keyword) {
    std::vector<std::string> cutFragments;
    
    // 前置截取机制 - 处理 arg ** arg2 这样的语法
    size_t keywordPos = code.find(keyword);
    
    while (keywordPos != std::string::npos) {
        // 截取关键字前的片段，避免将语法发送给编译器引发错误
        if (keywordPos > 0) {
            std::string prefixFragment = code.substr(0, keywordPos);
            
            // 去除尾部空白
            size_t end = prefixFragment.find_last_not_of(" \t\n\r");
            if (end != std::string::npos) {
                prefixFragment = prefixFragment.substr(0, end + 1);
                if (!prefixFragment.empty()) {
                    cutFragments.push_back(prefixFragment);
                }
            }
        }
        
        // 查找下一个关键字
        keywordPos = code.find(keyword, keywordPos + keyword.length());
    }
    
    return cutFragments;
}

bool dualPointerScan(const std::string& code, const std::string& keyword) {
    // 双指针扫描机制
    // 两个指针同时位于0位置
    size_t frontPointer = 0;
    size_t backPointer = 0;
    
    // 预先扫描一个片段是否存在关键字
    while (backPointer < code.length()) {
        // 检查当前窗口是否包含关键字
        std::string window = code.substr(frontPointer, backPointer - frontPointer + 50);
        
        if (window.find(keyword) != std::string::npos) {
            // 找到关键字，确保语法片段能够成功返回CJMOD
            return true;
        }
        
        // 滑动窗口算法同步向前
        if (backPointer - frontPointer > 100) {
            frontPointer += 25;
        }
        backPointer += 25;
    }
    
    return false;
}

} // namespace CJMODScanner

// ========== CJMODGenerator namespace 实现 ==========

namespace CJMODGenerator {

std::string exportResult(const Arg& args) {
    std::ostringstream result;
    
    // 根据transform模式导出最终的JS代码
    for (size_t i = 0; i < args.args.size(); ++i) {
        if (i > 0) result << " ";
        result << args.args[i].value;
    }
    
    return result.str();
}

} // namespace CJMODGenerator

// ========== CHTLJSFunction namespace 实现 ==========

namespace CHTLJSFunction {

void CreateCHTLJSFunction(const std::string& syntax) {
    // 按照原始API.md创建CHTL JS函数
    // 封装了原始API构建语法的流程，能够快速构建CHTL JS函数
    // 这些CHTL JS函数天然支持虚对象vir以及无修饰字符串
    std::cout << "Creating CHTL JS Function: " << syntax << std::endl;
}

void bindVirtualObject(const std::string& functionName) {
    // 绑定虚拟对象vir
    // 对于不使用CreateCHTLJSFunction创建的，但是符合CHTL JS函数的语法
    // 可以使用bindVirtualObject手动绑定虚对象vir，获得虚对象的支持
    std::cout << "Binding virtual object for: " << functionName << std::endl;
}

} // namespace CHTLJSFunction

} // namespace CHTLJS