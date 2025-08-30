#include "CSSCompiler.h"
#include "../Error/ErrorReporter.h"
#include <iostream>
#include <sstream>

namespace CHTL {

CSSCompiler::CSSCompiler() : m_debugMode(false) {}

CSSCompiler::~CSSCompiler() = default;

std::string CSSCompiler::compile(const std::string& cssCode) {
    try {
        if (m_debugMode) {
            std::cout << "🎨 开始CSS编译，代码长度: " << cssCode.length() << std::endl;
        }
        
        // 基本CSS处理
        std::string processedCSS = cssCode;
        
        // 移除多余空白
        processedCSS = removeExcessWhitespace(processedCSS);
        
        // 格式化CSS
        processedCSS = formatCSS(processedCSS);
        
        if (m_debugMode) {
            std::cout << "  ✓ CSS编译完成: " << processedCSS.length() << " 字符" << std::endl;
        }
        
        return processedCSS;
        
    } catch (const std::exception& e) {
        reportError("CSS编译错误: " + std::string(e.what()));
        return cssCode;  // 返回原始代码
    }
}

std::string CSSCompiler::removeExcessWhitespace(const std::string& css) {
    std::string result = css;
    
    // 移除多余的空格和换行
    std::regex multipleSpaces(R"(\s+)");
    result = std::regex_replace(result, multipleSpaces, " ");
    
    // 移除分号前的空格
    std::regex semicolonSpaces(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonSpaces, "; ");
    
    // 移除大括号周围的空格
    std::regex braceSpaces(R"(\s*\{\s*)");
    result = std::regex_replace(result, braceSpaces, " { ");
    
    std::regex closeBraceSpaces(R"(\s*\}\s*)");
    result = std::regex_replace(result, closeBraceSpaces, " } ");
    
    return result;
}

std::string CSSCompiler::formatCSS(const std::string& css) {
    std::ostringstream formatted;
    
    bool inRule = false;
    int indentLevel = 0;
    
    for (size_t i = 0; i < css.length(); ++i) {
        char c = css[i];
        
        if (c == '{') {
            formatted << " {\n";
            indentLevel++;
            inRule = true;
        } else if (c == '}') {
            indentLevel--;
            formatted << "\n";
            for (int j = 0; j < indentLevel; ++j) {
                formatted << "  ";
            }
            formatted << "}\n";
            inRule = false;
        } else if (c == ';' && inRule) {
            formatted << ";\n";
            for (int j = 0; j < indentLevel; ++j) {
                formatted << "  ";
            }
        } else if (c != '\n' && c != '\r') {
            formatted << c;
        }
    }
    
    return formatted.str();
}

void CSSCompiler::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error("CSSCompiler", message);
}

}