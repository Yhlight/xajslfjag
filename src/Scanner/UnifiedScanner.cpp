#include "UnifiedScanner.h"
#include "../Util/StringUtils.h"
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const String& source, const ScannerConfig& cfg)
    : sourceCode(source), currentPosition(0), config(cfg) {
    sourceLength = sourceCode.length();
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanAndCut() {
    std::vector<CodeFragment> fragments;
    currentPosition = 0;
    
    while (currentPosition < sourceLength) {
        // 跳过空白字符
        while (currentPosition < sourceLength && std::isspace(sourceCode[currentPosition])) {
            if (config.preserveWhitespace) break;
            currentPosition++;
        }
        
        if (currentPosition >= sourceLength) break;
        
        // 扫描下一个片段
        CodeFragment fragment = scanNextFragment();
        if (!fragment.content.empty()) {
            fragments.push_back(fragment);
        }
    }
    
    // 优化片段
    return optimizeFragments(fragments);
}

CodeFragment CHTLUnifiedScanner::scanNextFragment() {
    if (currentPosition >= sourceLength) {
        return CodeFragment{"", FragmentType::UNKNOWN, currentPosition, currentPosition, 0, ""};
    }
    
    size_t startPos = currentPosition;
    String testFragment;
    
    // 尝试不同的切割策略
    for (size_t lookAhead = config.minFragmentSize; 
         lookAhead <= config.maxFragmentSize && startPos + lookAhead <= sourceLength; 
         lookAhead++) {
        
        testFragment = sourceCode.substr(startPos, lookAhead);
        FragmentType type = detectFragmentType(testFragment, startPos);
        
        if (type != FragmentType::UNKNOWN) {
            // 检查是否需要扩展
            if (needsExpansion(testFragment, startPos)) {
                size_t expansionSize = calculateExpansionSize(testFragment, startPos);
                if (startPos + lookAhead + expansionSize <= sourceLength) {
                    lookAhead += expansionSize;
                    testFragment = sourceCode.substr(startPos, lookAhead);
                }
            }
            
            // 执行精准切割
            size_t endPos = startPos + lookAhead;
            CodeFragment fragment = performPreciseCutting(testFragment)[0];
            fragment.startPosition = startPos;
            fragment.endPosition = endPos;
            fragment.context = analyzeContext(sourceCode, startPos);
            fragment.nestLevel = calculateNestLevel(sourceCode, startPos);
            
            currentPosition = endPos;
            return fragment;
        }
    }
    
    // 如果无法识别，作为未知片段处理
    size_t endPos = std::min(startPos + config.minFragmentSize, sourceLength);
    currentPosition = endPos;
    
    return CodeFragment{
        sourceCode.substr(startPos, endPos - startPos),
        FragmentType::UNKNOWN,
        startPos,
        endPos,
        0,
        ""
    };
}

FragmentType CHTLUnifiedScanner::detectFragmentType(const String& fragment, size_t position) const {
    // 按优先级检测片段类型
    
    // 1. CHTL语法检测
    if (isCHTLSyntax(fragment, position)) {
        return FragmentType::CHTL;
    }
    
    // 2. CHTL JS语法检测
    if (isCHTLJSSyntax(fragment, position)) {
        return FragmentType::CHTL_JS;
    }
    
    // 3. CSS检测
    if (isCSSFragment(fragment, position)) {
        return FragmentType::CSS;
    }
    
    // 4. JavaScript检测
    if (isJavaScriptFragment(fragment, position)) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 5. HTML检测
    if (isHTMLFragment(fragment, position)) {
        return FragmentType::HTML;
    }
    
    return FragmentType::UNKNOWN;
}

bool CHTLUnifiedScanner::isCHTLSyntax(const String& fragment, size_t position) const {
    // 检测CHTL关键语法特征
    return hasCHTLKeywords(fragment) || 
           hasCHTLBrackets(fragment) || 
           hasCHTLTemplates(fragment) ||
           hasCHTLImports(fragment) ||
           hasCHTLOrigin(fragment);
}

bool CHTLUnifiedScanner::hasCHTLKeywords(const String& fragment) const {
    // CHTL关键字列表
    StringVector keywords = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Configuration]", "[Namespace]",
        "@Style", "@Element", "@Var", "@Html", "@JavaScript", "@Chtl", "@CJmod",
        "inherit", "delete", "insert", "except", "from", "as", "use",
        "after", "before", "replace", "at top", "at bottom",
        "text", "style", "script"
    };
    
    for (const auto& keyword : keywords) {
        if (fragment.find(keyword) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::hasCHTLBrackets(const String& fragment) const {
    // 检测CHTL特有的括号结构
    return fragment.find("{") != String::npos && 
           (fragment.find("[") != String::npos || fragment.find("@") != String::npos);
}

bool CHTLUnifiedScanner::hasCHTLTemplates(const String& fragment) const {
    // 检测模板调用语法
    std::regex templatePattern(R"(@(Style|Element|Var)\s+\w+)");
    return std::regex_search(fragment, templatePattern);
}

bool CHTLUnifiedScanner::hasCHTLImports(const String& fragment) const {
    // 检测导入语法
    return Util::StringUtils::starts_with(Util::StringUtils::trim(fragment), "[Import]");
}

bool CHTLUnifiedScanner::hasCHTLOrigin(const String& fragment) const {
    // 检测原始嵌入
    return Util::StringUtils::starts_with(Util::StringUtils::trim(fragment), "[Origin]");
}

bool CHTLUnifiedScanner::isCHTLJSSyntax(const String& fragment, size_t position) const {
    if (!config.enableCHTLJSDetection) return false;
    
    return hasCHTLJSSelectors(fragment) ||
           hasCHTLJSOperators(fragment) ||
           hasCHTLJSFunctions(fragment) ||
           hasCHTLJSModules(fragment) ||
           hasCHTLJSVirtualObjects(fragment);
}

bool CHTLUnifiedScanner::hasCHTLJSSelectors(const String& fragment) const {
    // 检测增强选择器语法 {{selector}}
    std::regex selectorPattern(R"(\{\{[^}]+\}\})");
    return std::regex_search(fragment, selectorPattern);
}

bool CHTLUnifiedScanner::hasCHTLJSOperators(const String& fragment) const {
    // 检测CHTL JS特有操作符
    return fragment.find("->") != String::npos || 
           fragment.find("&->") != String::npos;
}

bool CHTLUnifiedScanner::hasCHTLJSFunctions(const String& fragment) const {
    // 检测CHTL JS函数：listen, delegate, animate等
    StringVector functions = {"listen", "delegate", "animate", "module"};
    
    for (const auto& func : functions) {
        std::regex funcPattern(func + R"(\s*\{)");
        if (std::regex_search(fragment, funcPattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::hasCHTLJSModules(const String& fragment) const {
    // 检测模块语法
    std::regex modulePattern(R"(module\s*\{)");
    return std::regex_search(fragment, modulePattern);
}

bool CHTLUnifiedScanner::hasCHTLJSVirtualObjects(const String& fragment) const {
    // 检测虚对象语法
    return Util::StringUtils::starts_with(Util::StringUtils::trim(fragment), "vir ");
}

bool CHTLUnifiedScanner::isCSSFragment(const String& fragment, size_t position) const {
    // 检测CSS属性语法
    std::regex cssPattern(R"([a-zA-Z-]+\s*:\s*[^;]+;?)");
    return std::regex_search(fragment, cssPattern) && 
           fragment.find("{{") == String::npos;  // 排除CHTL语法
}

bool CHTLUnifiedScanner::isJavaScriptFragment(const String& fragment, size_t position) const {
    // 检测JavaScript语法特征
    StringVector jsKeywords = {
        "function", "var", "let", "const", "if", "else", "for", "while",
        "return", "console", "document", "window", "addEventListener"
    };
    
    for (const auto& keyword : jsKeywords) {
        if (fragment.find(keyword) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::isHTMLFragment(const String& fragment, size_t position) const {
    // 检测HTML标签
    std::regex htmlPattern(R"(<[a-zA-Z][^>]*>)");
    return std::regex_search(fragment, htmlPattern);
}

std::vector<CodeFragment> CHTLUnifiedScanner::performPreciseCutting(const String& content) const {
    std::vector<CodeFragment> fragments;
    
    // 基于检测到的类型进行精准切割
    FragmentType type = detectFragmentType(content, 0);
    
    switch (type) {
        case FragmentType::CHTL: {
            size_t endPos = content.length();
            fragments.push_back(cutCHTLFragment(content, 0, endPos));
            break;
        }
        case FragmentType::CHTL_JS: {
            size_t endPos = content.length();
            fragments.push_back(cutCHTLJSFragment(content, 0, endPos));
            break;
        }
        case FragmentType::CSS: {
            size_t endPos = content.length();
            fragments.push_back(cutCSSFragment(content, 0, endPos));
            break;
        }
        case FragmentType::JAVASCRIPT: {
            size_t endPos = content.length();
            fragments.push_back(cutJSFragment(content, 0, endPos));
            break;
        }
        default: {
            fragments.push_back({content, type, 0, content.length(), 0, ""});
            break;
        }
    }
    
    return fragments;
}

CodeFragment CHTLUnifiedScanner::cutCHTLFragment(const String& content, size_t start, size_t& end) const {
    // CHTL片段的精准切割
    // 确保括号匹配
    auto bracePair = ScannerUtils::findMatchingBrace(content, start);
    if (bracePair.second != String::npos) {
        end = bracePair.second + 1;
    }
    
    return {content.substr(start, end - start), FragmentType::CHTL, start, end, 0, ""};
}

CodeFragment CHTLUnifiedScanner::cutCHTLJSFragment(const String& content, size_t start, size_t& end) const {
    // CHTL JS片段的精准切割
    // 处理选择器和操作符的边界
    
    String fragment = content.substr(start, end - start);
    
    // 确保选择器完整
    size_t selectorStart = fragment.find("{{");
    if (selectorStart != String::npos) {
        size_t selectorEnd = fragment.find("}}", selectorStart);
        if (selectorEnd != String::npos) {
            end = start + selectorEnd + 2;
        }
    }
    
    return {content.substr(start, end - start), FragmentType::CHTL_JS, start, end, 0, ""};
}

CodeFragment CHTLUnifiedScanner::cutCSSFragment(const String& content, size_t start, size_t& end) const {
    // CSS片段切割，确保属性完整
    size_t semicolon = content.find(';', start);
    if (semicolon != String::npos && semicolon < end) {
        end = semicolon + 1;
    }
    
    return {content.substr(start, end - start), FragmentType::CSS, start, end, 0, ""};
}

CodeFragment CHTLUnifiedScanner::cutJSFragment(const String& content, size_t start, size_t& end) const {
    // JavaScript片段切割
    size_t lineEnd = ScannerUtils::findLineEnd(content, start);
    if (lineEnd < end) {
        end = lineEnd;
    }
    
    return {content.substr(start, end - start), FragmentType::JAVASCRIPT, start, end, 0, ""};
}

bool CHTLUnifiedScanner::needsExpansion(const String& fragment, size_t position) const {
    // 检查是否需要向前扩展以获得完整语法
    
    // 检查未闭合的括号
    int braceCount = 0;
    for (char c : fragment) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
    
    return braceCount > 0;
}

size_t CHTLUnifiedScanner::calculateExpansionSize(const String& fragment, size_t position) const {
    // 计算需要扩展的大小
    size_t expansionSize = 0;
    size_t searchPos = position + fragment.length();
    
    while (searchPos < sourceLength) {
        if (sourceCode[searchPos] == '}') {
            expansionSize = searchPos - position - fragment.length() + 1;
            break;
        }
        searchPos++;
        
        // 防止无限扩展
        if (expansionSize > config.maxFragmentSize) {
            break;
        }
    }
    
    return expansionSize;
}

std::vector<CodeFragment> CHTLUnifiedScanner::optimizeFragments(const std::vector<CodeFragment>& fragments) const {
    std::vector<CodeFragment> optimized;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        CodeFragment current = fragments[i];
        
        // 尝试与前一个片段合并
        if (!optimized.empty() && shouldMergeFragments(optimized.back(), current)) {
            optimized.back() = mergeFragments(optimized.back(), current);
        } else {
            optimized.push_back(current);
        }
    }
    
    return optimized;
}

bool CHTLUnifiedScanner::shouldMergeFragments(const CodeFragment& prev, const CodeFragment& current) const {
    // 同类型的小片段可以合并
    return prev.type == current.type && 
           prev.content.length() < config.minFragmentSize &&
           current.content.length() < config.minFragmentSize;
}

CodeFragment CHTLUnifiedScanner::mergeFragments(const CodeFragment& first, const CodeFragment& second) const {
    return {
        first.content + second.content,
        first.type,
        first.startPosition,
        second.endPosition,
        std::max(first.nestLevel, second.nestLevel),
        first.context.empty() ? second.context : first.context
    };
}

String CHTLUnifiedScanner::analyzeContext(const String& content, size_t position) const {
    // 分析上下文：查找最近的父级元素或块
    size_t start = position;
    while (start > 0 && content[start] != '{' && content[start] != '<') {
        start--;
    }
    
    if (start > 0) {
        return content.substr(start, std::min(position - start + 50, content.length() - start));
    }
    
    return "";
}

int CHTLUnifiedScanner::calculateNestLevel(const String& content, size_t position) const {
    int level = 0;
    for (size_t i = 0; i < position && i < content.length(); ++i) {
        if (content[i] == '{') level++;
        else if (content[i] == '}') level--;
    }
    return std::max(0, level);
}

String CHTLUnifiedScanner::fragmentTypeToString(FragmentType type) const {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
        case FragmentType::HTML: return "HTML";
        case FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

void CHTLUnifiedScanner::printFragments(const std::vector<CodeFragment>& fragments) const {
    std::cout << "\n=== Fragment Analysis ===" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& frag = fragments[i];
        std::cout << "Fragment " << i << ": " << fragmentTypeToString(frag.type) 
                  << " [" << frag.startPosition << "-" << frag.endPosition << "]" << std::endl;
        std::cout << "Content: " << frag.content.substr(0, 50) 
                  << (frag.content.length() > 50 ? "..." : "") << std::endl;
        std::cout << "---" << std::endl;
    }
}

// Scanner辅助函数实现
namespace ScannerUtils {

bool startsWithCHTLKeyword(const String& str) {
    StringVector keywords = {"[Template]", "[Custom]", "[Origin]", "[Import]", "[Configuration]"};
    String trimmed = Util::StringUtils::trim(str);
    
    for (const auto& keyword : keywords) {
        if (Util::StringUtils::starts_with(trimmed, keyword)) {
            return true;
        }
    }
    return false;
}

bool containsCHTLJSSelector(const String& str) {
    return str.find("{{") != String::npos && str.find("}}") != String::npos;
}

std::pair<size_t, size_t> findMatchingBrace(const String& content, size_t start) {
    size_t openPos = content.find('{', start);
    if (openPos == String::npos) {
        return {String::npos, String::npos};
    }
    
    int braceCount = 1;
    for (size_t i = openPos + 1; i < content.length(); ++i) {
        if (content[i] == '{') {
            braceCount++;
        } else if (content[i] == '}') {
            braceCount--;
            if (braceCount == 0) {
                return {openPos, i};
            }
        }
    }
    
    return {openPos, String::npos};
}

size_t findLineEnd(const String& content, size_t start) {
    size_t pos = start;
    while (pos < content.length() && content[pos] != '\n' && content[pos] != '\r') {
        pos++;
    }
    return pos;
}

} // namespace Scanner

} // namespace CHTL