#include "CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <regex>
#include <stack>

namespace CHTL {

// 修正后的代码片段类型定义
enum class FragmentType {
    CHTL,           // CHTL语法
    CHTL_JS,        // CHTL JS语法
    PURE_JS,        // 纯JavaScript语法  
    CSS,            // CSS语法
    HTML,           // HTML语法
    UNKNOWN
};

// 修正后的扫描器实现
std::vector<ScanResult> CHTLUnifiedScanner::scanAndSplit() {
    std::vector<ScanResult> results;
    
    try {
        // 执行智能语法识别扫描
        performIntelligentScan();
        
        // 验证每个片段的语法正确性
        validateFragmentSyntax();
        
        // 转换为扫描结果
        for (const auto& fragment : fragments) {
            ScanResult result;
            result.type = fragmentTypeToString(fragment.type);
            result.content = fragment.content;
            result.startPosition = fragment.startPos;
            result.endPosition = fragment.endPos;
            result.isValid = fragment.isValid;
            result.context = fragment.context;
            results.push_back(result);
        }
        
    } catch (const std::exception& e) {
        reportError("扫描过程发生异常: " + String(e.what()));
    }
    
    return results;
}

void CHTLUnifiedScanner::performIntelligentScan() {
    while (currentPosition < sourceCode.length()) {
        skipWhitespace();
        if (currentPosition >= sourceCode.length()) break;
        
        // 检测局部script块的特殊处理
        if (isLocalScriptBlock(currentPosition)) {
            processLocalScriptBlock();
        }
        // 检测其他CHTL语法
        else if (isCHTLSyntax(currentPosition)) {
            processCHTLFragment();
        }
        // 检测HTML片段
        else if (isHTMLFragment(currentPosition)) {
            processHTMLFragment();
        }
        else {
            // 跳过未识别内容
            currentPosition++;
        }
    }
}

bool CHTLUnifiedScanner::isLocalScriptBlock(size_t position) {
    // 检测 script { ... } 模式
    if (position + 6 < sourceCode.length()) {
        String keyword = sourceCode.substr(position, 6);
        if (keyword == "script") {
            size_t bracePos = findNextNonWhitespace(position + 6);
            return bracePos < sourceCode.length() && sourceCode[bracePos] == '{';
        }
    }
    return false;
}

void CHTLUnifiedScanner::processLocalScriptBlock() {
    size_t scriptStart = currentPosition;
    
    // 跳过 "script"
    currentPosition += 6;
    skipWhitespace();
    
    // 跳过 '{'
    if (currentPosition < sourceCode.length() && sourceCode[currentPosition] == '{') {
        currentPosition++;
    }
    
    size_t contentStart = currentPosition;
    size_t contentEnd = findMatchingBrace(currentPosition - 1);
    
    if (contentEnd == String::npos) {
        reportError("未找到匹配的结束大括号");
        return;
    }
    
    // 提取script内容
    String scriptContent = sourceCode.substr(contentStart, contentEnd - contentStart - 1);
    
    // 智能分析script内容类型
    analyzeScriptContent(scriptContent, contentStart);
    
    currentPosition = contentEnd;
}

void CHTLUnifiedScanner::analyzeScriptContent(const String& content, size_t startPos) {
    // 检查是否包含CHTL JS特有语法
    bool hasCHTLJSSyntax = false;
    bool hasPureJSSyntax = false;
    
    // CHTL JS特征检测
    StringVector chtljsFeatures = {
        "{{",           // 增强选择器 {{.box}}
        "}}",           // 增强选择器结束
        "&->",          // 事件绑定操作符  
        "listen",       // 增强监听器
        "delegate",     // 事件委托
        "animate",      // 动画功能
        "module",       // 模块语法
        "vir"           // 虚对象
    };
    
    for (const auto& feature : chtljsFeatures) {
        if (content.find(feature) != String::npos) {
            hasCHTLJSSyntax = true;
            break;
        }
    }
    
    // 纯JS特征检测
    StringVector pureJSFeatures = {
        "function",
        "var ",
        "let ",
        "const ",
        "=>",
        "document.",
        "window.",
        "console."
    };
    
    for (const auto& feature : pureJSFeatures) {
        if (content.find(feature) != String::npos) {
            hasPureJSSyntax = true;
            break;
        }
    }
    
    // 根据检测结果进行分片处理
    if (hasCHTLJSSyntax && hasPureJSSyntax) {
        // 混合语法，需要进一步分离
        separateMixedScriptContent(content, startPos);
    } else if (hasCHTLJSSyntax) {
        // 纯CHTL JS语法
        createFragment(FragmentType::CHTL_JS, content, startPos);
    } else {
        // 纯JavaScript语法
        createFragment(FragmentType::PURE_JS, content, startPos);
    }
}

void CHTLUnifiedScanner::separateMixedScriptContent(const String& content, size_t startPos) {
    // 使用正则表达式分离CHTL JS和纯JS语法
    std::regex chtljsPattern(R"(\{\{[^}]+\}\}[^;]*;?)");  // 匹配 {{...}} 相关语法
    std::regex listenPattern(R"(listen\s*\{[^}]*\})");    // 匹配 listen {...}
    std::regex delegatePattern(R"(delegate\s*\{[^}]*\})"); // 匹配 delegate {...}
    std::regex animatePattern(R"(animate\s*\{[^}]*\})");   // 匹配 animate {...}
    
    size_t currentPos = 0;
    size_t contentStart = startPos;
    
    std::sregex_iterator iter(content.begin(), content.end(), chtljsPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        const std::smatch& match = *iter;
        
        // 检查匹配前是否有纯JS内容
        if (match.position() > currentPos) {
            String jsContent = content.substr(currentPos, match.position() - currentPos);
            if (!jsContent.empty() && !isWhitespaceOnly(jsContent)) {
                createFragment(FragmentType::PURE_JS, jsContent, contentStart + currentPos);
            }
        }
        
        // 创建CHTL JS片段
        String chtljsContent = match.str();
        createFragment(FragmentType::CHTL_JS, chtljsContent, contentStart + match.position());
        
        currentPos = match.position() + match.length();
    }
    
    // 处理剩余的JS内容
    if (currentPos < content.length()) {
        String remainingJS = content.substr(currentPos);
        if (!isWhitespaceOnly(remainingJS)) {
            createFragment(FragmentType::PURE_JS, remainingJS, contentStart + currentPos);
        }
    }
}

void CHTLUnifiedScanner::createFragment(FragmentType type, const String& content, size_t startPos) {
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = content;
    fragment.startPos = startPos;
    fragment.endPos = startPos + content.length();
    fragment.isValid = true;
    fragment.context = determineContext(startPos);
    
    fragments.push_back(fragment);
}

String CHTLUnifiedScanner::determineContext(size_t position) {
    // 分析位置的上下文环境
    size_t searchStart = (position > 200) ? position - 200 : 0;
    String contextSegment = sourceCode.substr(searchStart, position - searchStart);
    
    // 检查是否在特定块内
    if (contextSegment.rfind("script") != String::npos) {
        return "local_script";
    }
    if (contextSegment.rfind("style") != String::npos) {
        return "local_style";
    }
    if (contextSegment.rfind("[Template]") != String::npos) {
        return "template";
    }
    if (contextSegment.rfind("[Custom]") != String::npos) {
        return "custom";
    }
    
    return "global";
}

bool CHTLUnifiedScanner::isWhitespaceOnly(const String& str) {
    return std::all_of(str.begin(), str.end(), [](char c) {
        return std::isspace(c);
    });
}

size_t CHTLUnifiedScanner::findMatchingBrace(size_t openBracePos) {
    if (openBracePos >= sourceCode.length() || sourceCode[openBracePos] != '{') {
        return String::npos;
    }
    
    int braceLevel = 1;
    size_t pos = openBracePos + 1;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < sourceCode.length() && braceLevel > 0) {
        char c = sourceCode[pos];
        
        // 处理字符串
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar && (pos == 0 || sourceCode[pos-1] != '\\')) {
            inString = false;
        }
        
        if (!inString) {
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
            }
        }
        
        pos++;
    }
    
    return (braceLevel == 0) ? pos : String::npos;
}

size_t CHTLUnifiedScanner::findNextNonWhitespace(size_t position) {
    while (position < sourceCode.length() && std::isspace(sourceCode[position])) {
        position++;
    }
    return position;
}

void CHTLUnifiedScanner::skipWhitespace() {
    while (currentPosition < sourceCode.length() && 
           std::isspace(sourceCode[currentPosition])) {
        currentPosition++;
    }
}

bool CHTLUnifiedScanner::isCHTLSyntax(size_t position) {
    // 检测CHTL特有语法
    StringVector chtlKeywords = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", 
        "[Configuration]", "[Namespace]", "text", "style"
    };
    
    for (const auto& keyword : chtlKeywords) {
        if (matchKeywordAtPosition(keyword, position)) {
            return true;
        }
    }
    
    // 检测HTML元素
    String word = extractWordAtPosition(position);
    return HTMLElementMap::isHTMLElement(word);
}

bool CHTLUnifiedScanner::isHTMLFragment(size_t position) {
    return position < sourceCode.length() && sourceCode[position] == '<';
}

void CHTLUnifiedScanner::processCHTLFragment() {
    size_t start = currentPosition;
    size_t end = findCHTLFragmentEnd(start);
    
    String content = sourceCode.substr(start, end - start);
    createFragment(FragmentType::CHTL, content, start);
    
    currentPosition = end;
}

void CHTLUnifiedScanner::processHTMLFragment() {
    size_t start = currentPosition;
    size_t end = findHTMLFragmentEnd(start);
    
    String content = sourceCode.substr(start, end - start);
    createFragment(FragmentType::HTML, content, start);
    
    currentPosition = end;
}

size_t CHTLUnifiedScanner::findCHTLFragmentEnd(size_t start) {
    // 简化实现：找到下一个主要语法块
    size_t pos = start;
    int braceLevel = 0;
    
    while (pos < sourceCode.length()) {
        char c = sourceCode[pos];
        
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
            if (braceLevel == 0) {
                return pos + 1;
            }
        }
        
        pos++;
    }
    
    return pos;
}

size_t CHTLUnifiedScanner::findHTMLFragmentEnd(size_t start) {
    // 简化实现：找到HTML标签结束
    size_t pos = start + 1;
    
    while (pos < sourceCode.length()) {
        if (sourceCode[pos] == '>') {
            return pos + 1;
        }
        pos++;
    }
    
    return pos;
}

bool CHTLUnifiedScanner::matchKeywordAtPosition(const String& keyword, size_t position) {
    if (position + keyword.length() > sourceCode.length()) {
        return false;
    }
    
    return sourceCode.substr(position, keyword.length()) == keyword;
}

String CHTLUnifiedScanner::extractWordAtPosition(size_t position) {
    if (position >= sourceCode.length()) return "";
    
    size_t end = position;
    while (end < sourceCode.length() && 
           (std::isalnum(sourceCode[end]) || sourceCode[end] == '_')) {
        end++;
    }
    
    return sourceCode.substr(position, end - position);
}

void CHTLUnifiedScanner::validateFragmentSyntax() {
    for (auto& fragment : fragments) {
        switch (fragment.type) {
            case FragmentType::CHTL_JS:
                fragment.isValid = validateCHTLJSSyntax(fragment.content);
                break;
            case FragmentType::PURE_JS:
                fragment.isValid = validatePureJSSyntax(fragment.content);
                break;
            case FragmentType::CHTL:
                fragment.isValid = validateCHTLSyntax(fragment.content);
                break;
            default:
                fragment.isValid = true;
                break;
        }
    }
}

bool CHTLUnifiedScanner::validateCHTLJSSyntax(const String& content) {
    // 验证CHTL JS语法
    StringVector requiredFeatures = {"{{", "}}"};
    
    for (const auto& feature : requiredFeatures) {
        if (content.find(feature) == String::npos) {
            return false; // 如果声称是CHTL JS但没有特有语法，则无效
        }
    }
    
    return true;
}

bool CHTLUnifiedScanner::validatePureJSSyntax(const String& content) {
    // 验证纯JavaScript语法（简化检查）
    if (content.empty()) return false;
    
    // 检查是否包含CHTL JS特有语法（不应该包含）
    StringVector chtljsFeatures = {"{{", "}}", "&->"};
    
    for (const auto& feature : chtljsFeatures) {
        if (content.find(feature) != String::npos) {
            return false; // 纯JS不应包含CHTL JS语法
        }
    }
    
    return true;
}

bool CHTLUnifiedScanner::validateCHTLSyntax(const String& content) {
    // 基础CHTL语法验证
    return !content.empty();
}

String CHTLUnifiedScanner::fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::PURE_JS: return "PURE_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::HTML: return "HTML";
        default: return "UNKNOWN";
    }
}

void CHTLUnifiedScanner::reportError(const String& message) {
    errors.push_back(message);
}

} // namespace CHTL