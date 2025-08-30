#include "JSCompiler.h"
#include "../Error/ErrorReporter.h"
#include <iostream>
#include <sstream>

namespace CHTL {

JSCompiler::JSCompiler() : m_debugMode(false) {}

JSCompiler::~JSCompiler() = default;

std::string JSCompiler::compile(const std::string& jsCode) {
    try {
        if (m_debugMode) {
            std::cout << "⚡ 开始JavaScript编译，代码长度: " << jsCode.length() << std::endl;
        }
        
        // 基本JavaScript处理
        std::string processedJS = jsCode;
        
        // 格式化JavaScript
        processedJS = formatJavaScript(processedJS);
        
        // 基本语法检查
        if (!validateJavaScript(processedJS)) {
            reportError("JavaScript语法验证失败");
        }
        
        if (m_debugMode) {
            std::cout << "  ✓ JavaScript编译完成: " << processedJS.length() << " 字符" << std::endl;
        }
        
        return processedJS;
        
    } catch (const std::exception& e) {
        reportError("JavaScript编译错误: " + std::string(e.what()));
        return jsCode;  // 返回原始代码
    }
}

std::string JSCompiler::formatJavaScript(const std::string& js) {
    std::ostringstream formatted;
    
    int indentLevel = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < js.length(); ++i) {
        char c = js[i];
        char next = (i + 1 < js.length()) ? js[i + 1] : 0;
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
                formatted << c;
            } else if (c == '{') {
                formatted << " {\n";
                indentLevel++;
                for (int j = 0; j < indentLevel; ++j) {
                    formatted << "  ";
                }
            } else if (c == '}') {
                indentLevel--;
                formatted << "\n";
                for (int j = 0; j < indentLevel; ++j) {
                    formatted << "  ";
                }
                formatted << "}";
                if (next != ';' && next != ',' && next != ')' && next != '}') {
                    formatted << "\n";
                    for (int j = 0; j < indentLevel; ++j) {
                        formatted << "  ";
                    }
                }
            } else if (c == ';') {
                formatted << ";\n";
                for (int j = 0; j < indentLevel; ++j) {
                    formatted << "  ";
                }
            } else if (c != '\n' && c != '\r') {
                formatted << c;
            }
        } else {
            formatted << c;
            if (c == stringChar && (i == 0 || js[i - 1] != '\\')) {
                inString = false;
            }
        }
    }
    
    return formatted.str();
}

bool JSCompiler::validateJavaScript(const std::string& js) {
    // 基本的JavaScript语法检查
    
    // 检查括号平衡
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < js.length(); ++i) {
        char c = js[i];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
            } else if (c == '(') {
                parenCount++;
            } else if (c == ')') {
                parenCount--;
            } else if (c == '[') {
                bracketCount++;
            } else if (c == ']') {
                bracketCount--;
            }
        } else if (c == stringChar && (i == 0 || js[i - 1] != '\\')) {
            inString = false;
        }
    }
    
    if (braceCount != 0) {
        reportError("JavaScript大括号不匹配");
        return false;
    }
    
    if (parenCount != 0) {
        reportError("JavaScript圆括号不匹配");
        return false;
    }
    
    if (bracketCount != 0) {
        reportError("JavaScript方括号不匹配");
        return false;
    }
    
    return true;
}

void JSCompiler::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error("JSCompiler", message);
}

}