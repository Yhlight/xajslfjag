#include "UnifiedScanner.h"
#include <regex>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner() : m_debugMode(false) {
    initializeSyntaxPatterns();
}

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& source) {
    std::vector<CodeFragment> fragments;
    ScanContext context;
    context.source = source;
    
    while (context.position < source.length()) {
        CodeFragment fragment = scanFragment(source, context.position);
        if (!fragment.content.empty()) {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

CodeFragment CHTLUnifiedScanner::scanFragment(const std::string& source, size_t& position) {
    ScanContext context;
    context.source = source;
    context.position = position;
    
    // 跳过空白字符
    position = skipWhitespace(source, position);
    if (position >= source.length()) {
        return CodeFragment();
    }
    
    size_t startPos = position;
    size_t startLine = context.lineNumber;
    size_t startCol = context.columnNumber;
    
    // 检查不同类型的语法
    if (isCHTLSyntax(source.substr(position))) {
        return scanCHTLBlock(source, position);
    } else if (isCHTLJSSyntax(source.substr(position))) {
        return scanCHTLJSBlock(source, position);
    } else {
        // 默认扫描到下一个可能的CHTL/CHTL JS语法开始
        while (position < source.length()) {
            if (isCHTLSyntax(source.substr(position)) || 
                isCHTLJSSyntax(source.substr(position))) {
                break;
            }
            position++;
        }
        
        std::string content = source.substr(startPos, position - startPos);
        FragmentType type = identifyFragmentType(content, context);
        
        return CodeFragment(type, content, startPos, position, startLine, startCol);
    }
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitCHTLToMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    
    if (fragment.type != FragmentType::CHTL) {
        units.push_back(fragment);
        return units;
    }
    
    // CHTL最小单元：@Style, @Element, @Var, [Template], [Custom], [Origin], etc.
    std::vector<std::string> minimalUnits = getCHTLMinimalUnits();
    
    std::string content = fragment.content;
    size_t position = 0;
    
    while (position < content.length()) {
        size_t startPos = position;
        
        // 寻找下一个最小单元
        bool foundUnit = false;
        for (const auto& unit : minimalUnits) {
            if (content.substr(position, unit.length()) == unit) {
                // 找到单元开始，扫描到结束
                size_t endPos = position + unit.length();
                
                // 扫描到对应的结束位置（通常是大括号结束）
                if (endPos < content.length() && content[endPos] == '{') {
                    endPos = scanBraceBlock(content, endPos);
                }
                
                std::string unitContent = content.substr(position, endPos - position);
                units.emplace_back(FragmentType::CHTL, unitContent, 
                                 fragment.startPosition + position,
                                 fragment.startPosition + endPos,
                                 fragment.lineNumber, fragment.columnNumber);
                
                position = endPos;
                foundUnit = true;
                break;
            }
        }
        
        if (!foundUnit) {
            position++;
        }
    }
    
    return units;
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitCHTLJSToMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    
    if (fragment.type != FragmentType::CHTL_JS) {
        units.push_back(fragment);
        return units;
    }
    
    // CHTL JS最小单元：{{...}}, listen{...}, animate{...}, vir, &->, etc.
    std::vector<std::string> minimalUnits = getCHTLJSMinimalUnits();
    
    std::string content = fragment.content;
    size_t position = 0;
    
    while (position < content.length()) {
        bool foundUnit = false;
        
        // 检查增强选择器 {{...}}
        if (position + 1 < content.length() && 
            content.substr(position, 2) == "{{") {
            size_t endPos = content.find("}}", position + 2);
            if (endPos != std::string::npos) {
                endPos += 2;
                std::string unitContent = content.substr(position, endPos - position);
                units.emplace_back(FragmentType::CHTL_JS, unitContent,
                                 fragment.startPosition + position,
                                 fragment.startPosition + endPos,
                                 fragment.lineNumber, fragment.columnNumber);
                position = endPos;
                foundUnit = true;
            }
        }
        
        // 检查事件绑定操作符 &->
        if (!foundUnit && position + 2 < content.length() && 
            content.substr(position, 3) == "&->") {
            size_t endPos = position + 3;
            std::string unitContent = content.substr(position, endPos - position);
            units.emplace_back(FragmentType::CHTL_JS, unitContent,
                             fragment.startPosition + position,
                             fragment.startPosition + endPos,
                             fragment.lineNumber, fragment.columnNumber);
            position = endPos;
            foundUnit = true;
        }
        
        if (!foundUnit) {
            position++;
        }
    }
    
    return units;
}

std::vector<CodeFragment> CHTLUnifiedScanner::smartSlice(const std::string& source, size_t sliceSize) {
    std::vector<CodeFragment> fragments;
    size_t position = 0;
    
    while (position < source.length()) {
        size_t endPos = std::min(position + sliceSize, source.length());
        
        // 检查边界是否合理
        if (!isValidSliceBoundary(source, endPos)) {
            endPos = extendSliceForCompleteness(source, position, endPos);
        }
        
        std::string sliceContent = source.substr(position, endPos - position);
        FragmentType type = identifyFragmentType(sliceContent, ScanContext());
        
        fragments.emplace_back(type, sliceContent, position, endPos, 1, 1);
        position = endPos;
    }
    
    return fragments;
}

bool CHTLUnifiedScanner::isValidSliceBoundary(const std::string& source, size_t position) {
    if (position >= source.length()) {
        return true;
    }
    
    // 检查是否在字符串字面量中
    // 检查是否在注释中
    // 检查是否在语法关键字中间
    
    char ch = source[position];
    
    // 不应该在单词中间切断
    if (std::isalnum(ch) || ch == '_') {
        return false;
    }
    
    // 检查CHTL/CHTL JS语法边界
    if (position > 0) {
        std::string before = source.substr(std::max(0, (int)position - 10), 10);
        std::string after = source.substr(position, std::min(10, (int)(source.length() - position)));
        
        // 如果前面或后面有CHTL语法关键字，不应该在这里切断
        if (isCHTLSyntax(before + after)) {
            return false;
        }
    }
    
    return true;
}

size_t CHTLUnifiedScanner::extendSliceForCompleteness(const std::string& source, size_t start, size_t end) {
    size_t extendedEnd = end;
    
    // 向前查找到合理的边界
    while (extendedEnd < source.length() && !isValidSliceBoundary(source, extendedEnd)) {
        extendedEnd++;
    }
    
    // 确保不会无限扩展
    if (extendedEnd - start > source.length() / 2) {
        extendedEnd = end;
    }
    
    return extendedEnd;
}

FragmentType CHTLUnifiedScanner::identifyFragmentType(const std::string& content, const ScanContext& context) {
    if (isCHTLSyntax(content)) {
        return FragmentType::CHTL;
    } else if (isCHTLJSSyntax(content)) {
        return FragmentType::CHTL_JS;
    }
    
    // 简单的CSS/JS检测
    if (content.find("style") != std::string::npos && 
        content.find("{") != std::string::npos) {
        return FragmentType::CSS;
    }
    
    if (content.find("script") != std::string::npos || 
        content.find("function") != std::string::npos) {
        return FragmentType::JAVASCRIPT;
    }
    
    return FragmentType::HTML;
}

bool CHTLUnifiedScanner::isCHTLSyntax(const std::string& content) {
    // 检查CHTL关键字
    std::vector<std::string> chtlKeywords = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Configuration]", "[Namespace]",
        "@Style", "@Element", "@Var", "@Html", "@JavaScript",
        "style", "text", "inherit", "delete", "insert"
    };
    
    for (const auto& keyword : chtlKeywords) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSSyntax(const std::string& content) {
    // 检查CHTL JS关键字和语法
    return isEnhancedSelector(content) ||
           content.find("listen") != std::string::npos ||
           content.find("delegate") != std::string::npos ||
           content.find("animate") != std::string::npos ||
           content.find("vir") != std::string::npos ||
           content.find("&->") != std::string::npos ||
           content.find("module") != std::string::npos;
}

bool CHTLUnifiedScanner::isEnhancedSelector(const std::string& content) {
    return content.find("{{") != std::string::npos && 
           content.find("}}") != std::string::npos;
}

bool CHTLUnifiedScanner::isCHTLJSFunction(const std::string& content) {
    // 检查CHTL JS函数语法：functionName { key: value }
    std::regex chtlJSPattern(R"(\w+\s*\{[^}]*\})");
    return std::regex_search(content, chtlJSPattern);
}

std::vector<std::string> CHTLUnifiedScanner::getCHTLMinimalUnits() {
    return {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Configuration]", "[Namespace]",
        "@Style", "@Element", "@Var", "@Html", "@JavaScript", "@CJmod", "@Chtl",
        "inherit", "delete", "insert", "style", "text", "script"
    };
}

std::vector<std::string> CHTLUnifiedScanner::getCHTLJSMinimalUnits() {
    return {
        "{{", "}}", "&->", "listen", "delegate", "animate", "vir", "module"
    };
}

bool CHTLUnifiedScanner::isCompleteSyntaxUnit(const std::string& content, FragmentType type) {
    if (type == FragmentType::CHTL) {
        // 检查CHTL语法完整性
        return true; // 简化实现
    } else if (type == FragmentType::CHTL_JS) {
        // 检查CHTL JS语法完整性
        return true; // 简化实现
    }
    
    return true;
}

std::vector<CodeFragment> CHTLUnifiedScanner::doublePointerScan(const std::string& source, 
                                                               const std::string& keyword) {
    std::vector<CodeFragment> fragments;
    size_t frontPointer = 0;
    size_t backPointer = 0;
    
    while (frontPointer < source.length()) {
        // 前指针寻找关键字
        size_t keywordPos = source.find(keyword, frontPointer);
        if (keywordPos == std::string::npos) {
            break;
        }
        
        // 后指针收集内容
        std::string content = source.substr(backPointer, keywordPos - backPointer + keyword.length());
        if (!content.empty()) {
            fragments.emplace_back(FragmentType::CHTL_JS, content, 
                                 backPointer, keywordPos + keyword.length(), 1, 1);
        }
        
        frontPointer = keywordPos + keyword.length();
        backPointer = frontPointer;
    }
    
    return fragments;
}

CodeFragment CHTLUnifiedScanner::frontTruncate(const std::vector<CodeFragment>& fragments, 
                                              const std::string& keyword) {
    for (const auto& fragment : fragments) {
        if (fragment.content.find(keyword) != std::string::npos) {
            // 截取包含关键字的部分
            size_t keywordPos = fragment.content.find(keyword);
            std::string truncatedContent = fragment.content.substr(keywordPos);
            
            return CodeFragment(FragmentType::CHTL_JS, truncatedContent,
                              fragment.startPosition + keywordPos,
                              fragment.endPosition,
                              fragment.lineNumber, fragment.columnNumber);
        }
    }
    
    return CodeFragment();
}

std::string CHTLUnifiedScanner::generateScanReport(const std::vector<CodeFragment>& fragments) {
    std::stringstream ss;
    
    ss << "Unified Scanner Report:\n";
    ss << "======================\n\n";
    
    std::unordered_map<FragmentType, int> typeCounts;
    size_t totalSize = 0;
    
    for (const auto& fragment : fragments) {
        typeCounts[fragment.type]++;
        totalSize += fragment.content.length();
    }
    
    ss << "Fragment Statistics:\n";
    ss << "  Total Fragments: " << fragments.size() << "\n";
    ss << "  Total Size: " << totalSize << " characters\n\n";
    
    ss << "Fragment Types:\n";
    for (const auto& [type, count] : typeCounts) {
        ss << "  ";
        switch (type) {
            case FragmentType::CHTL: ss << "CHTL"; break;
            case FragmentType::CHTL_JS: ss << "CHTL JS"; break;
            case FragmentType::CSS: ss << "CSS"; break;
            case FragmentType::JAVASCRIPT: ss << "JavaScript"; break;
            case FragmentType::HTML: ss << "HTML"; break;
            default: ss << "Unknown"; break;
        }
        ss << ": " << count << " fragments\n";
    }
    
    return ss.str();
}

bool CHTLUnifiedScanner::validateFragmentIntegrity(const std::vector<CodeFragment>& fragments, 
                                                  const std::string& originalSource) {
    std::string reconstructed;
    
    for (const auto& fragment : fragments) {
        reconstructed += fragment.content;
    }
    
    return reconstructed == originalSource;
}

void CHTLUnifiedScanner::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

size_t CHTLUnifiedScanner::scanStringLiteral(const std::string& source, size_t position, char quote) {
    size_t pos = position + 1; // 跳过开始引号
    
    while (pos < source.length()) {
        if (source[pos] == quote) {
            return pos + 1; // 包含结束引号
        } else if (source[pos] == '\\') {
            pos += 2; // 跳过转义字符
        } else {
            pos++;
        }
    }
    
    return pos;
}

size_t CHTLUnifiedScanner::scanComment(const std::string& source, size_t position) {
    if (position + 1 < source.length()) {
        if (source.substr(position, 2) == "//") {
            // 单行注释
            size_t pos = source.find('\n', position);
            return (pos != std::string::npos) ? pos + 1 : source.length();
        } else if (source.substr(position, 2) == "/*") {
            // 多行注释
            size_t pos = source.find("*/", position + 2);
            return (pos != std::string::npos) ? pos + 2 : source.length();
        }
    }
    
    return position + 1;
}

CodeFragment CHTLUnifiedScanner::scanCHTLBlock(const std::string& source, size_t& position) {
    size_t startPos = position;
    
    // 扫描到CHTL块结束
    size_t endPos = scanBraceBlock(source, position);
    
    std::string content = source.substr(startPos, endPos - startPos);
    position = endPos;
    
    return CodeFragment(FragmentType::CHTL, content, startPos, endPos, 1, 1);
}

CodeFragment CHTLUnifiedScanner::scanCHTLJSBlock(const std::string& source, size_t& position) {
    size_t startPos = position;
    
    // 扫描到CHTL JS块结束
    size_t endPos = scanBraceBlock(source, position);
    
    std::string content = source.substr(startPos, endPos - startPos);
    position = endPos;
    
    return CodeFragment(FragmentType::CHTL_JS, content, startPos, endPos, 1, 1);
}

size_t CHTLUnifiedScanner::scanBraceBlock(const std::string& source, size_t position) {
    size_t braceCount = 0;
    size_t pos = position;
    
    while (pos < source.length()) {
        char ch = source[pos];
        
        if (ch == '{') {
            braceCount++;
        } else if (ch == '}') {
            braceCount--;
            if (braceCount == 0) {
                return pos + 1;
            }
        } else if (ch == '"' || ch == '\'') {
            pos = scanStringLiteral(source, pos, ch) - 1;
        } else if (pos + 1 < source.length() && 
                  (source.substr(pos, 2) == "//" || source.substr(pos, 2) == "/*")) {
            pos = scanComment(source, pos) - 1;
        }
        
        pos++;
    }
    
    return pos;
}

void CHTLUnifiedScanner::updatePosition(ScanContext& context, char ch) {
    context.position++;
    if (ch == '\n') {
        context.lineNumber++;
        context.columnNumber = 1;
    } else {
        context.columnNumber++;
    }
}

size_t CHTLUnifiedScanner::skipWhitespace(const std::string& source, size_t position) {
    while (position < source.length() && std::isspace(source[position])) {
        position++;
    }
    return position;
}

void CHTLUnifiedScanner::initializeSyntaxPatterns() {
    // 初始化语法模式映射
    m_syntaxPatterns["[Template]"] = FragmentType::CHTL;
    m_syntaxPatterns["[Custom]"] = FragmentType::CHTL;
    m_syntaxPatterns["[Origin]"] = FragmentType::CHTL;
    m_syntaxPatterns["{{"] = FragmentType::CHTL_JS;
    m_syntaxPatterns["listen"] = FragmentType::CHTL_JS;
    m_syntaxPatterns["delegate"] = FragmentType::CHTL_JS;
    m_syntaxPatterns["animate"] = FragmentType::CHTL_JS;
}

void CHTLUnifiedScanner::reportError(const std::string& message, size_t line, size_t column) {
    std::string fullMessage = message;
    if (line > 0) {
        fullMessage += " (Line: " + std::to_string(line) + ", Column: " + std::to_string(column) + ")";
    }
    // 使用ErrorReporter的正确接口
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, fullMessage);
}

} // namespace CHTL