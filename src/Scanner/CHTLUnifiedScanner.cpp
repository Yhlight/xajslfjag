#include "CHTLUnifiedScanner.h"
#include "../CHTL/CHTLLexer/GlobalMap.h"
#include <algorithm>
#include <regex>
#include <stack>

namespace CHTL {

// 代码片段类型定义
enum class FragmentType {
    CHTL,
    CHTL_JS,
    CSS,
    JAVASCRIPT,
    HTML,
    UNKNOWN
};

// 代码片段结构
struct CodeFragment {
    FragmentType type;
    String content;
    size_t startPos;
    size_t endPos;
    int nestingLevel;
    String context;
    
    CodeFragment(FragmentType t, const String& c, size_t start, size_t end, int level = 0)
        : type(t), content(c), startPos(start), endPos(end), nestingLevel(level) {}
};

// CHTLUnifiedScanner实现
CHTLUnifiedScanner::CHTLUnifiedScanner(const String& source)
    : sourceCode(source), currentPosition(0), minUnitSize(10), maxLookahead(50) {
    fragments.clear();
    errors.clear();
}

std::vector<ScanResult> CHTLUnifiedScanner::scanAndSplit() {
    std::vector<ScanResult> results;
    
    try {
        // 执行变长切片扫描
        performVariableLengthSlicing();
        
        // 验证每个片段的上下文
        validateFragmentContexts();
        
        // 转换为扫描结果
        for (const auto& fragment : fragments) {
            ScanResult result;
            result.type = fragmentTypeToString(fragment.type);
            result.content = fragment.content;
            result.startPosition = fragment.startPos;
            result.endPosition = fragment.endPos;
            result.isValid = validateFragmentSyntax(fragment);
            result.context = fragment.context;
            results.push_back(result);
        }
        
    } catch (const std::exception& e) {
        reportError("扫描过程发生异常: " + String(e.what()));
    }
    
    return results;
}

void CHTLUnifiedScanner::performVariableLengthSlicing() {
    while (currentPosition < sourceCode.length()) {
        // 跳过空白字符
        skipWhitespace();
        if (currentPosition >= sourceCode.length()) break;
        
        // 检测片段类型并进行切割
        FragmentType detectedType = detectFragmentType(currentPosition);
        
        if (detectedType != FragmentType::UNKNOWN) {
            size_t fragmentStart = currentPosition;
            size_t fragmentEnd = findFragmentEnd(detectedType, fragmentStart);
            
            // 如果发现不完整的片段，尝试向前扩展
            if (needsExtension(detectedType, fragmentStart, fragmentEnd)) {
                fragmentEnd = extendFragment(detectedType, fragmentStart, fragmentEnd);
            }
            
            // 创建片段
            String fragmentContent = sourceCode.substr(fragmentStart, fragmentEnd - fragmentStart);
            CodeFragment fragment(detectedType, fragmentContent, fragmentStart, fragmentEnd);
            fragment.context = analyzeContext(fragmentStart, fragmentEnd);
            
            fragments.push_back(fragment);
            currentPosition = fragmentEnd;
        } else {
            // 未知类型，按最小单元处理
            handleUnknownFragment();
        }
    }
}

FragmentType CHTLUnifiedScanner::detectFragmentType(size_t position) {
    if (position >= sourceCode.length()) return FragmentType::UNKNOWN;
    
    // 检测CHTL JS语法
    if (isCHTLJSSyntax(position)) {
        return FragmentType::CHTL_JS;
    }
    
    // 检测样式块
    if (isStyleBlock(position)) {
        return FragmentType::CSS;
    }
    
    // 检测脚本块
    if (isScriptBlock(position)) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 检测CHTL语法
    if (isCHTLSyntax(position)) {
        return FragmentType::CHTL;
    }
    
    // 检测HTML片段
    if (isHTMLFragment(position)) {
        return FragmentType::HTML;
    }
    
    return FragmentType::UNKNOWN;
}

bool CHTLUnifiedScanner::isCHTLJSSyntax(size_t position) {
    // 检测CHTL JS特有语法
    StringVector chtljsKeywords = {"module", "listen", "delegate", "animate", "vir"};
    
    for (const auto& keyword : chtljsKeywords) {
        if (matchKeywordAtPosition(keyword, position)) {
            return true;
        }
    }
    
    // 检测{{...}}选择器语法
    if (position + 1 < sourceCode.length() && 
        sourceCode.substr(position, 2) == "{{") {
        return true;
    }
    
    // 检测&->事件绑定语法
    if (position + 2 < sourceCode.length() && 
        sourceCode.substr(position, 3) == "&->") {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isStyleBlock(size_t position) {
    // 检测style关键字
    if (matchKeywordAtPosition("style", position)) {
        // 确保后面跟着{
        size_t bracePos = findNextNonWhitespace(position + 5);
        return bracePos < sourceCode.length() && sourceCode[bracePos] == '{';
    }
    
    // 检测CSS选择器模式
    return detectCSSSelector(position);
}

bool CHTLUnifiedScanner::isScriptBlock(size_t position) {
    // 检测script关键字
    if (matchKeywordAtPosition("script", position)) {
        size_t bracePos = findNextNonWhitespace(position + 6);
        return bracePos < sourceCode.length() && sourceCode[bracePos] == '{';
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLSyntax(size_t position) {
    // 检测CHTL特有语法
    StringVector chtlKeywords = {"[Template]", "[Custom]", "[Origin]", "[Import]", 
                                "[Configuration]", "[Namespace]", "text"};
    
    for (const auto& keyword : chtlKeywords) {
        if (matchKeywordAtPosition(keyword, position)) {
            return true;
        }
    }
    
    // 检测HTML元素
    String word = extractWordAtPosition(position);
    if (HTMLElementMap::isHTMLElement(word)) {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isHTMLFragment(size_t position) {
    // 检测HTML标签
    if (position < sourceCode.length() && sourceCode[position] == '<') {
        return true;
    }
    
    return false;
}

size_t CHTLUnifiedScanner::findFragmentEnd(FragmentType type, size_t start) {
    size_t pos = start;
    int braceLevel = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < sourceCode.length()) {
        char c = sourceCode[pos];
        
        // 处理字符串
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar && (pos == 0 || sourceCode[pos-1] != '\\')) {
            inString = false;
        }
        
        if (!inString) {
            // 处理大括号嵌套
            if (c == '{') {
                braceLevel++;
            } else if (c == '}') {
                braceLevel--;
                if (braceLevel == 0) {
                    return pos + 1; // 包含结束大括号
                }
            }
            
            // 特殊处理不同类型的结束条件
            if (type == FragmentType::CHTL_JS && braceLevel == 0) {
                if (detectCHTLJSEnd(pos)) {
                    return pos;
                }
            }
        }
        
        pos++;
    }
    
    return pos;
}

bool CHTLUnifiedScanner::needsExtension(FragmentType type, size_t start, size_t end) {
    // 检查片段是否完整
    String fragment = sourceCode.substr(start, end - start);
    
    // 检查大括号配对
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (char c : fragment) {
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar) {
            inString = false;
        }
        
        if (!inString) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
        }
    }
    
    return braceCount != 0;
}

size_t CHTLUnifiedScanner::extendFragment(FragmentType type, size_t start, size_t currentEnd) {
    size_t maxExtension = std::min(sourceCode.length(), currentEnd + maxLookahead);
    
    for (size_t pos = currentEnd; pos < maxExtension; ++pos) {
        if (sourceCode[pos] == '}') {
            // 重新验证片段完整性
            if (!needsExtension(type, start, pos + 1)) {
                return pos + 1;
            }
        }
    }
    
    return currentEnd;
}

void CHTLUnifiedScanner::validateFragmentContexts() {
    for (auto& fragment : fragments) {
        fragment.context = analyzeContext(fragment.startPos, fragment.endPos);
        
        // 根据上下文调整片段类型
        adjustFragmentTypeByContext(fragment);
    }
}

String CHTLUnifiedScanner::analyzeContext(size_t start, size_t end) {
    // 分析片段的上下文环境
    String context = "global";
    
    // 检查是否在特定块内部
    if (isInElementContext(start)) {
        context = "element";
    } else if (isInStyleContext(start)) {
        context = "style";
    } else if (isInScriptContext(start)) {
        context = "script";
    } else if (isInTemplateContext(start)) {
        context = "template";
    }
    
    return context;
}

bool CHTLUnifiedScanner::validateFragmentSyntax(const CodeFragment& fragment) {
    switch (fragment.type) {
        case FragmentType::CHTL:
            return validateCHTLSyntax(fragment.content);
        case FragmentType::CHTL_JS:
            return validateCHTLJSSyntax(fragment.content);
        case FragmentType::CSS:
            return validateCSSSyntax(fragment.content);
        case FragmentType::JAVASCRIPT:
            return validateJSSyntax(fragment.content);
        case FragmentType::HTML:
            return validateHTMLSyntax(fragment.content);
        default:
            return false;
    }
}

bool CHTLUnifiedScanner::validateCHTLSyntax(const String& content) {
    // 基础CHTL语法验证
    if (content.empty()) return false;
    
    // 检查关键字
    std::regex chtlKeywordRegex(R"(\[(Template|Custom|Origin|Import|Configuration|Namespace)\])");
    if (std::regex_search(content, chtlKeywordRegex)) {
        return true;
    }
    
    // 检查HTML元素
    std::regex elementRegex(R"(\b(div|span|p|h[1-6]|html|body|head)\s*\{)");
    return std::regex_search(content, elementRegex);
}

bool CHTLUnifiedScanner::validateCHTLJSSyntax(const String& content) {
    // CHTL JS语法验证
    StringVector chtljsFeatures = {"module", "listen", "delegate", "animate", "vir", "{{", "&->"};
    
    for (const auto& feature : chtljsFeatures) {
        if (content.find(feature) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::validateCSSSyntax(const String& content) {
    // CSS语法验证
    std::regex cssRuleRegex(R"([^{}]+\s*\{[^{}]*\})");
    return std::regex_search(content, cssRuleRegex);
}

bool CHTLUnifiedScanner::validateJSSyntax(const String& content) {
    // JavaScript语法验证
    StringVector jsFeatures = {"function", "var", "let", "const", "=>"};
    
    for (const auto& feature : jsFeatures) {
        if (content.find(feature) != String::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::validateHTMLSyntax(const String& content) {
    // HTML语法验证
    std::regex htmlTagRegex(R"(<[^>]+>)");
    return std::regex_search(content, htmlTagRegex);
}

// 辅助方法实现
void CHTLUnifiedScanner::skipWhitespace() {
    while (currentPosition < sourceCode.length() && 
           std::isspace(sourceCode[currentPosition])) {
        currentPosition++;
    }
}

void CHTLUnifiedScanner::handleUnknownFragment() {
    // 按最小单元处理未知片段
    size_t start = currentPosition;
    size_t end = std::min(currentPosition + minUnitSize, sourceCode.length());
    
    String content = sourceCode.substr(start, end - start);
    CodeFragment fragment(FragmentType::UNKNOWN, content, start, end);
    fragments.push_back(fragment);
    
    currentPosition = end;
}

bool CHTLUnifiedScanner::matchKeywordAtPosition(const String& keyword, size_t position) {
    if (position + keyword.length() > sourceCode.length()) {
        return false;
    }
    
    String substring = sourceCode.substr(position, keyword.length());
    return substring == keyword;
}

String CHTLUnifiedScanner::extractWordAtPosition(size_t position) {
    if (position >= sourceCode.length()) return "";
    
    size_t start = position;
    size_t end = position;
    
    // 向后扩展直到非字母数字字符
    while (end < sourceCode.length() && 
           (std::isalnum(sourceCode[end]) || sourceCode[end] == '_')) {
        end++;
    }
    
    return sourceCode.substr(start, end - start);
}

size_t CHTLUnifiedScanner::findNextNonWhitespace(size_t position) {
    while (position < sourceCode.length() && std::isspace(sourceCode[position])) {
        position++;
    }
    return position;
}

bool CHTLUnifiedScanner::detectCSSSelector(size_t position) {
    // 检测CSS选择器模式
    std::regex selectorRegex(R"([.#]?[a-zA-Z][a-zA-Z0-9_-]*\s*\{)");
    String segment = sourceCode.substr(position, std::min(size_t(50), sourceCode.length() - position));
    return std::regex_search(segment, selectorRegex);
}

bool CHTLUnifiedScanner::detectCHTLJSEnd(size_t position) {
    // 检测CHTL JS片段的结束
    if (position + 1 < sourceCode.length()) {
        String twoChar = sourceCode.substr(position, 2);
        if (twoChar == "}}" || twoChar == ");") {
            return true;
        }
    }
    return false;
}

bool CHTLUnifiedScanner::isInElementContext(size_t position) {
    // 检查是否在元素上下文中
    return findNearestBlockType(position) == "element";
}

bool CHTLUnifiedScanner::isInStyleContext(size_t position) {
    return findNearestBlockType(position) == "style";
}

bool CHTLUnifiedScanner::isInScriptContext(size_t position) {
    return findNearestBlockType(position) == "script";
}

bool CHTLUnifiedScanner::isInTemplateContext(size_t position) {
    return findNearestBlockType(position) == "template";
}

String CHTLUnifiedScanner::findNearestBlockType(size_t position) {
    // 向前搜索最近的块类型标识
    if (position == 0) return "global";
    
    size_t searchStart = (position > 100) ? position - 100 : 0;
    String segment = sourceCode.substr(searchStart, position - searchStart);
    
    if (segment.rfind("style") != String::npos) return "style";
    if (segment.rfind("script") != String::npos) return "script";
    if (segment.rfind("[Template]") != String::npos) return "template";
    
    return "element";
}

void CHTLUnifiedScanner::adjustFragmentTypeByContext(CodeFragment& fragment) {
    // 根据上下文调整片段类型
    if (fragment.context == "style" && fragment.type == FragmentType::UNKNOWN) {
        fragment.type = FragmentType::CSS;
    } else if (fragment.context == "script" && fragment.type == FragmentType::UNKNOWN) {
        if (isCHTLJSSyntax(fragment.startPos)) {
            fragment.type = FragmentType::CHTL_JS;
        } else {
            fragment.type = FragmentType::JAVASCRIPT;
        }
    }
}

String CHTLUnifiedScanner::fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JAVASCRIPT";
        case FragmentType::HTML: return "HTML";
        default: return "UNKNOWN";
    }
}

void CHTLUnifiedScanner::reportError(const String& message) {
    errors.push_back(message);
}

bool CHTLUnifiedScanner::hasErrors() const {
    return !errors.empty();
}

StringVector CHTLUnifiedScanner::getErrors() const {
    return errors;
}

void CHTLUnifiedScanner::setMinUnitSize(size_t size) {
    minUnitSize = size;
}

void CHTLUnifiedScanner::setMaxLookahead(size_t lookahead) {
    maxLookahead = lookahead;
}

String CHTLUnifiedScanner::getSourceCode() const {
    return sourceCode;
}

std::vector<CodeFragment> CHTLUnifiedScanner::getFragments() const {
    return fragments;
}

} // namespace CHTL