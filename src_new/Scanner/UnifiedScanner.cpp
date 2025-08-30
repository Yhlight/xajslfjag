#include "UnifiedScanner.h"
#include <algorithm>
#include <regex>

namespace CHTL {

UnifiedScanner::UnifiedScanner() {
}

std::vector<CodeFragment> UnifiedScanner::smartSlice(const std::string& source, size_t sliceSize) {
    std::vector<CodeFragment> fragments;
    
    if (source.empty()) {
        return fragments;
    }
    
    size_t position = 0;
    size_t lineNumber = 1;
    size_t columnNumber = 1;
    
    while (position < source.length()) {
        size_t endPosition = std::min(position + sliceSize, source.length());
        
        // 检查切片边界是否合理
        if (!isValidSliceBoundary(source, endPosition) && endPosition < source.length()) {
            // 扩展切片以包含完整语法单元
            endPosition = extendSliceForCompleteness(source, position, endPosition);
        }
        
        // 创建代码片段
        std::string content = source.substr(position, endPosition - position);
        FragmentType type = identifyFragmentType(content);
        
        CodeFragment fragment(type, content, position, endPosition, lineNumber, columnNumber);
        fragments.push_back(fragment);
        
        // 更新位置信息
        for (size_t i = position; i < endPosition; ++i) {
            if (source[i] == '\n') {
                lineNumber++;
                columnNumber = 1;
            } else {
                columnNumber++;
            }
        }
        
        position = endPosition;
    }
    
    return fragments;
}

bool UnifiedScanner::isValidSliceBoundary(const std::string& source, size_t position) {
    if (position >= source.length()) return true;
    
    // 检查是否在字符串中间
    size_t quoteCount = 0;
    for (size_t i = 0; i < position; ++i) {
        if (source[i] == '"' && (i == 0 || source[i-1] != '\\')) {
            quoteCount++;
        }
    }
    if (quoteCount % 2 != 0) return false; // 在字符串中间
    
    // 检查是否在注释中间
    if (position > 1 && source.substr(position-2, 2) == "/*") return false;
    
    // 检查是否在大括号中间
    int braceCount = 0;
    for (size_t i = 0; i < position; ++i) {
        if (source[i] == '{') braceCount++;
        else if (source[i] == '}') braceCount--;
    }
    
    // 如果在块中间，检查下一个片段是否可能组成完整语法
    if (braceCount > 0) {
        // 向前查看，检查是否可能组成完整的CHTL或CHTL JS代码片段
        std::string nextPart = source.substr(position, std::min(size_t(100), source.length() - position));
        if (isCHTLSyntax(nextPart) || isCHTLJSSyntax(nextPart)) {
            return false; // 需要扩增
        }
    }
    
    return true;
}

size_t UnifiedScanner::extendSliceForCompleteness(const std::string& source, size_t start, size_t end) {
    size_t newEnd = end;
    
    // 向前扩增一定长度，确保语法完整性
    while (newEnd < source.length() && !isValidSliceBoundary(source, newEnd)) {
        newEnd = std::min(newEnd + 100, source.length());
    }
    
    return newEnd;
}

FragmentType UnifiedScanner::identifyFragmentType(const std::string& content) {
    if (isCHTLSyntax(content)) {
        return FragmentType::CHTL;
    } else if (isCHTLJSSyntax(content)) {
        return FragmentType::CHTL_JS;
    } else if (content.find("style") != std::string::npos && content.find("{") != std::string::npos) {
        return FragmentType::CSS;
    } else if (content.find("script") != std::string::npos || content.find("function") != std::string::npos) {
        return FragmentType::JAVASCRIPT;
    } else {
        return FragmentType::UNKNOWN;
    }
}

bool UnifiedScanner::isCHTLSyntax(const std::string& content) {
    // 检查CHTL语法特征
    std::vector<std::string> chtlKeywords = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", 
        "[Configuration]", "[Info]", "[Export]", "text", "style"
    };
    
    for (const auto& keyword : chtlKeywords) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSSyntax(const std::string& content) {
    // 检查CHTL JS语法特征
    if (isEnhancedSelector(content)) return true;
    
    std::vector<std::string> chtlJSKeywords = {
        "listen", "delegate", "animate", "vir", "&->", "module", "load"
    };
    
    for (const auto& keyword : chtlJSKeywords) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::isEnhancedSelector(const std::string& content) {
    // 检查增强选择器 {{...}}
    return content.find("{{") != std::string::npos && content.find("}}") != std::string::npos;
}

std::vector<std::string> UnifiedScanner::getCHTLMinimalUnits() {
    return {
        "{", "}", "[", "]", "(", ")", ";", ":", "=", ",", ".",
        "text", "style", "script", "[Template]", "[Custom]", "[Origin]",
        "[Import]", "[Namespace]", "[Configuration]"
    };
}

std::vector<std::string> UnifiedScanner::getCHTLJSMinimalUnits() {
    return {
        "{{", "}}", "&->", "->", "listen", "delegate", "animate", "vir",
        "module", "load", "click", "mouseenter", "mouseleave"
    };
}

std::vector<CodeFragment> UnifiedScanner::preciseSlice(const std::string& content, FragmentType type) {
    std::vector<CodeFragment> fragments;
    
    if (type == FragmentType::CHTL_JS) {
        // 对CHTL JS进行精确切割
        // 例如{{box}}->click将被切割为{{box}}->和click
        
        std::regex enhancedPattern(R"(\{\{[^}]+\}\}->?)");
        std::sregex_iterator iter(content.begin(), content.end(), enhancedPattern);
        std::sregex_iterator end;
        
        size_t lastPos = 0;
        for (; iter != end; ++iter) {
            std::smatch match = *iter;
            
            // 添加前面的部分
            if (match.position() > lastPos) {
                std::string beforeMatch = content.substr(lastPos, match.position() - lastPos);
                if (!beforeMatch.empty()) {
                    fragments.emplace_back(FragmentType::CHTL_JS, beforeMatch, lastPos, match.position());
                }
            }
            
            // 添加匹配的部分
            fragments.emplace_back(FragmentType::CHTL_JS, match.str(), match.position(), match.position() + match.length());
            
            lastPos = match.position() + match.length();
        }
        
        // 添加剩余部分
        if (lastPos < content.length()) {
            std::string remaining = content.substr(lastPos);
            if (!remaining.empty()) {
                fragments.emplace_back(FragmentType::CHTL_JS, remaining, lastPos, content.length());
            }
        }
    } else {
        // 其他类型直接返回原片段
        fragments.emplace_back(type, content, 0, content.length());
    }
    
    return fragments;
}

void UnifiedScanner::reportError(const std::string& message, size_t line, size_t column) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, message, "", line, column);
}

} // namespace CHTL