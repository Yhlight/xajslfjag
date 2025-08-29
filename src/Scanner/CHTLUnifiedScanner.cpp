#include "CHTLUnifiedScanner.h"
#include <chrono>
#include <iostream>

namespace CHTL {

String CodeFragment::toString() const {
    String typeStr;
    switch (type) {
        case FragmentType::CHTL: typeStr = "CHTL"; break;
        case FragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
        case FragmentType::CSS: typeStr = "CSS"; break;
        case FragmentType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
        case FragmentType::HTML: typeStr = "HTML"; break;
        case FragmentType::COMMENT: typeStr = "COMMENT"; break;
        case FragmentType::WHITESPACE: typeStr = "WHITESPACE"; break;
        default: typeStr = "UNKNOWN"; break;
    }
    
    return "Fragment{" + typeStr + ", pos:" + std::to_string(startPos) + 
           "-" + std::to_string(endPos) + ", line:" + std::to_string(lineNumber) + 
           ", col:" + std::to_string(columnNumber) + ", content:\"" + 
           (content.length() > 50 ? content.substr(0, 50) + "..." : content) + "\"}";
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const String& source, const ScannerConfig& config)
    : source(source), currentPos(0), currentLine(1), currentColumn(1), config(config) {
    resetPosition();
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanAll() {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::vector<CodeFragment> fragments;
    reset();
    
    while (hasMoreFragments()) {
        CodeFragment fragment = scanNext();
        if (!fragment.isEmpty()) {
            // 如果启用最小单元切割，进一步分割复合片段
            if (config.enableMinimalUnitSplitting && 
                (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_JS)) {
                auto subFragments = splitToMinimalUnits(fragment);
                fragments.insert(fragments.end(), subFragments.begin(), subFragments.end());
            } else {
                fragments.push_back(fragment);
            }
            updateStats(fragment);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    stats.scanTime = duration.count() / 1000.0; // 转换为毫秒
    stats.totalFragments = fragments.size();
    
    return fragments;
}

CodeFragment CHTLUnifiedScanner::scanNext() {
    if (!hasMoreFragments()) {
        return CodeFragment{};
    }
    
    // 跳过空白字符（如果配置要求）
    if (!config.preserveWhitespace) {
        skipWhitespace(currentPos);
    }
    
    if (isAtEnd(currentPos)) {
        return CodeFragment{};
    }
    
    return scanFragment();
}

bool CHTLUnifiedScanner::hasMoreFragments() const {
    return currentPos < source.length();
}

void CHTLUnifiedScanner::reset() {
    currentPos = 0;
    resetPosition();
    clearErrors();
    stats = ScanStats{};
}

CodeFragment CHTLUnifiedScanner::scanFragment() {
    size_t startPos = currentPos;
    size_t startLine = currentLine;
    size_t startColumn = currentColumn;
    
    // 检测片段类型
    FragmentType type = detectFragmentType(startPos, config.minValidationLength);
    
    // 找到片段结束位置
    size_t endPos = findFragmentEnd(startPos, type);
    
    // 如果启用可变长度切片，检查是否需要扩展
    if (config.enableVariableLengthSlicing) {
        String content = substring(startPos, endPos - startPos);
        if (needsExtension(content, type)) {
            endPos = extendFragment(startPos, endPos, type);
        }
    }
    
    // 创建片段
    String content = substring(startPos, endPos - startPos);
    CodeFragment fragment(type, content, startPos, endPos, startLine, startColumn);
    
    // 上下文验证
    if (config.enableContextValidation && !validateFragmentContext(fragment)) {
        reportInvalidFragment(type, startPos);
    }
    
    // 更新位置
    for (size_t i = startPos; i < endPos; ++i) {
        updatePosition(charAt(i));
    }
    currentPos = endPos;
    
    return fragment;
}

FragmentType CHTLUnifiedScanner::detectFragmentType(size_t start, size_t length) {
    if (isAtEnd(start)) {
        return FragmentType::UNKNOWN;
    }
    
    // 检查注释
    if (matchCommentPattern(start)) {
        return FragmentType::COMMENT;
    }
    
    // 检查空白字符
    if (std::isspace(charAt(start))) {
        return FragmentType::WHITESPACE;
    }
    
    // 检查CHTL JS模式
    if (matchCHTLJSPattern(start)) {
        return FragmentType::CHTL_JS;
    }
    
    // 检查CSS模式
    if (matchCSSPattern(start)) {
        return FragmentType::CSS;
    }
    
    // 检查JavaScript模式
    if (matchJSPattern(start)) {
        return FragmentType::JAVASCRIPT;
    }
    
    // 检查CHTL模式
    if (matchCHTLPattern(start)) {
        return FragmentType::CHTL;
    }
    
    return FragmentType::UNKNOWN;
}

size_t CHTLUnifiedScanner::findFragmentEnd(size_t start, FragmentType type) {
    size_t pos = start;
    
    switch (type) {
        case FragmentType::CHTL:
            return findCHTLFragmentEnd(pos);
        case FragmentType::CHTL_JS:
            return findCHTLJSFragmentEnd(pos);
        case FragmentType::CSS:
            return findCSSFragmentEnd(pos);
        case FragmentType::JAVASCRIPT:
            return findJSFragmentEnd(pos);
        case FragmentType::COMMENT:
            return findCommentEnd(pos);
        case FragmentType::WHITESPACE:
            return findWhitespaceEnd(pos);
        default:
            return pos + 1;
    }
}

bool CHTLUnifiedScanner::needsExtension(const String& fragment, FragmentType type) {
    if (type == FragmentType::CHTL) {
        return !validateCHTLFragment(fragment);
    } else if (type == FragmentType::CHTL_JS) {
        return !validateCHTLJSFragment(fragment);
    }
    return false;
}

size_t CHTLUnifiedScanner::extendFragment(size_t start, size_t currentEnd, FragmentType type) {
    size_t newEnd = currentEnd;
    size_t maxExtension = std::min(start + config.maxChunkSize, source.length());
    
    // 逐步扩展直到找到合适的边界
    while (newEnd < maxExtension) {
        String extendedContent = substring(start, newEnd - start);
        if (type == FragmentType::CHTL && validateCHTLFragment(extendedContent)) {
            break;
        } else if (type == FragmentType::CHTL_JS && validateCHTLJSFragment(extendedContent)) {
            break;
        }
        newEnd++;
    }
    
    return newEnd;
}

bool CHTLUnifiedScanner::validateCHTLFragment(const String& fragment) {
    // 检查CHTL片段的完整性
    size_t braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (char c : fragment) {
        if (inString) {
            if (c == stringChar && (fragment.size() == 1 || fragment[fragment.size()-2] != '\\')) {
                inString = false;
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                if (braceCount > 0) braceCount--;
            }
        }
    }
    
    return braceCount == 0 && !inString;
}

bool CHTLUnifiedScanner::validateCHTLJSFragment(const String& fragment) {
    // 检查CHTL JS片段的完整性
    if (Utils::startsWith(fragment, "{{") && Utils::endsWith(fragment, "}}")) {
        return true;
    }
    
    // 检查CHTL JS函数语法
    for (const String& func : {"listen", "animate", "delegate", "printMyLove", "iNeverAway"}) {
        if (fragment.find(func) != String::npos) {
            // 简单检查花括号匹配
            size_t braceCount = 0;
            for (char c : fragment) {
                if (c == '{') braceCount++;
                else if (c == '}') {
                    if (braceCount > 0) braceCount--;
                }
            }
            return braceCount == 0;
        }
    }
    
    return true;
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitToMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> result;
    
    if (fragment.type == FragmentType::CHTL) {
        auto breakpoints = findCHTLBreakpoints(fragment.content);
        size_t lastPos = 0;
        
        for (size_t bp : breakpoints) {
            if (bp > lastPos) {
                String subContent = fragment.content.substr(lastPos, bp - lastPos);
                CodeFragment subFragment(fragment.type, subContent, 
                                       fragment.startPos + lastPos, 
                                       fragment.startPos + bp,
                                       fragment.lineNumber, fragment.columnNumber);
                result.push_back(subFragment);
                lastPos = bp;
            }
        }
        
        // 添加最后一个片段
        if (lastPos < fragment.content.length()) {
            String subContent = fragment.content.substr(lastPos);
            CodeFragment subFragment(fragment.type, subContent,
                                   fragment.startPos + lastPos,
                                   fragment.endPos,
                                   fragment.lineNumber, fragment.columnNumber);
            result.push_back(subFragment);
        }
    } else if (fragment.type == FragmentType::CHTL_JS) {
        auto breakpoints = findCHTLJSBreakpoints(fragment.content);
        // 类似处理...
    }
    
    return result.empty() ? std::vector<CodeFragment>{fragment} : result;
}

std::vector<size_t> CHTLUnifiedScanner::findCHTLBreakpoints(const String& content) {
    std::vector<size_t> breakpoints;
    
    // 查找CHTL语法的断点，如关键字边界、操作符等
    for (size_t i = 0; i < content.length(); ++i) {
        // 检查关键字断点
        if (i == 0 || std::isspace(content[i-1])) {
            for (const String& keyword : {"div", "span", "text", "style", "script"}) {
                if (content.substr(i, keyword.length()) == keyword) {
                    if (i + keyword.length() == content.length() || 
                        !std::isalnum(content[i + keyword.length()])) {
                        if (i > 0) breakpoints.push_back(i);
                    }
                }
            }
        }
        
        // 检查符号断点
        if (content[i] == '{' || content[i] == '}' || content[i] == ';') {
            breakpoints.push_back(i + 1);
        }
    }
    
    return breakpoints;
}

std::vector<size_t> CHTLUnifiedScanner::findCHTLJSBreakpoints(const String& content) {
    std::vector<size_t> breakpoints;
    
    // 查找CHTL JS的断点，如{{}}、->等
    for (size_t i = 0; i < content.length() - 1; ++i) {
        if (content.substr(i, 2) == "{{" || content.substr(i, 2) == "}}" ||
            content.substr(i, 2) == "->" || content.substr(i, 3) == "&->") {
            breakpoints.push_back(i);
        }
    }
    
    return breakpoints;
}

// 双指针扫描实现（用于CJMOD）
CodeFragment CHTLUnifiedScanner::dualPointerScan(const String& keyword) {
    PointerPair pointers{currentPos, currentPos, false};
    
    while (pointers.frontPtr < source.length()) {
        // 预览检查关键字
        if (previewForKeyword(pointers.frontPtr, keyword)) {
            pointers.isCollecting = true;
            pointers.backPtr = pointers.frontPtr;
        }
        
        if (pointers.isCollecting) {
            // 同步向前移动
            synchronizePointers(pointers);
            
            // 检查是否完成收集
            String collected = substring(pointers.backPtr, 
                                       pointers.frontPtr - pointers.backPtr);
            if (validateCHTLJSFragment(collected)) {
                // 创建片段并返回
                CodeFragment fragment(FragmentType::CHTL_JS, collected,
                                    pointers.backPtr, pointers.frontPtr,
                                    currentLine, currentColumn);
                currentPos = pointers.frontPtr;
                return fragment;
            }
        } else {
            pointers.frontPtr++;
        }
    }
    
    return CodeFragment{};
}

bool CHTLUnifiedScanner::previewForKeyword(size_t pos, const String& keyword) const {
    if (pos + keyword.length() > source.length()) {
        return false;
    }
    return source.substr(pos, keyword.length()) == keyword;
}

void CHTLUnifiedScanner::synchronizePointers(PointerPair& pointers) {
    // 滑动窗口式同步移动
    pointers.frontPtr++;
    
    // 检查是否需要移动后指针
    if (pointers.frontPtr - pointers.backPtr > config.maxChunkSize) {
        pointers.backPtr++;
    }
}

// 前置截取实现（用于CJMOD）
String CHTLUnifiedScanner::preemptiveCapture(size_t pos, const String& pattern) {
    if (shouldPreempt(pos, pattern)) {
        // 查找模式的开始位置
        size_t patternStart = pos;
        while (patternStart > 0 && source.substr(patternStart, pattern.length()) != pattern) {
            patternStart--;
        }
        
        if (patternStart > 0) {
            // 截取从模式开始到当前位置的内容
            String captured = substring(patternStart, pos - patternStart);
            return captured;
        }
    }
    return "";
}

bool CHTLUnifiedScanner::shouldPreempt(size_t pos, const String& pattern) const {
    // 检查是否需要前置截取
    // 例如：处理 "arg ** arg2" 中的 "**" 模式
    if (pos >= pattern.length()) {
        String preceding = substring(pos - pattern.length(), pattern.length());
        return preceding == pattern;
    }
    return false;
}

// 模式匹配实现
bool CHTLUnifiedScanner::matchCHTLPattern(size_t pos) const {
    // 检查CHTL关键字
    String word = extractWordAt(pos);
    if (isCHTLKeyword(word) || isHTMLTag(word)) {
        return true;
    }
    
    // 检查CHTL语法结构
    if (charAt(pos) == '[' && pos + 8 < source.length()) {
        String bracket = substring(pos, 9);
        if (bracket == "[Template" || bracket == "[Custom]" || bracket == "[Origin]") {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::matchCHTLJSPattern(size_t pos) const {
    // 检查{{...}}选择器
    if (charAt(pos) == '{' && pos + 1 < source.length() && charAt(pos + 1) == '{') {
        return true;
    }
    
    // 检查事件绑定 &->
    if (charAt(pos) == '&' && pos + 2 < source.length() && 
        charAt(pos + 1) == '-' && charAt(pos + 2) == '>') {
        return true;
    }
    
    // 检查CHTL JS函数
    String word = extractWordAt(pos);
    if (word == "listen" || word == "animate" || word == "delegate" || 
        word == "printMyLove" || word == "iNeverAway" || word == "vir") {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::matchCSSPattern(size_t pos) const {
    if (isInStyleBlock(pos)) {
        String word = extractWordAt(pos);
        return isCSSProperty(word) || charAt(pos) == '.' || charAt(pos) == '#';
    }
    return false;
}

bool CHTLUnifiedScanner::matchJSPattern(size_t pos) const {
    if (isInScriptBlock(pos) && !isInCHTLJSContext(pos)) {
        String word = extractWordAt(pos);
        return isJSKeyword(word);
    }
    return false;
}

bool CHTLUnifiedScanner::matchCommentPattern(size_t pos) const {
    if (charAt(pos) == '/' && pos + 1 < source.length()) {
        char next = charAt(pos + 1);
        return next == '/' || next == '*';
    }
    if (charAt(pos) == '-' && pos + 1 < source.length() && charAt(pos + 1) == '-') {
        return true;
    }
    return false;
}

// 工具方法实现
char CHTLUnifiedScanner::charAt(size_t pos) const {
    return (pos < source.length()) ? source[pos] : '\0';
}

String CHTLUnifiedScanner::substring(size_t start, size_t length) const {
    if (start >= source.length()) return "";
    return source.substr(start, std::min(length, source.length() - start));
}

bool CHTLUnifiedScanner::isAtEnd(size_t pos) const {
    return pos >= source.length();
}

void CHTLUnifiedScanner::skipWhitespace(size_t& pos) const {
    while (pos < source.length() && std::isspace(charAt(pos))) {
        pos++;
    }
}

void CHTLUnifiedScanner::updatePosition(char c) {
    if (c == '\n') {
        currentLine++;
        currentColumn = 1;
    } else {
        currentColumn++;
    }
}

void CHTLUnifiedScanner::resetPosition() {
    currentLine = 1;
    currentColumn = 1;
}

void CHTLUnifiedScanner::reportError(const String& message, size_t pos) {
    if (pos == SIZE_MAX) pos = currentPos;
    String error = "Error at position " + std::to_string(pos) + 
                   " (line " + std::to_string(currentLine) + 
                   ", column " + std::to_string(currentColumn) + "): " + message;
    errors.push_back(error);
}

void CHTLUnifiedScanner::updateStats(const CodeFragment& fragment) {
    stats.totalChars += fragment.content.length();
    incrementFragmentCount(fragment.type);
}

void CHTLUnifiedScanner::incrementFragmentCount(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: stats.chtlFragments++; break;
        case FragmentType::CHTL_JS: stats.chtljsFragments++; break;
        case FragmentType::CSS: stats.cssFragments++; break;
        case FragmentType::JAVASCRIPT: stats.jsFragments++; break;
        case FragmentType::COMMENT: stats.commentFragments++; break;
        case FragmentType::WHITESPACE: stats.whitespaceFragments++; break;
        default: stats.unknownFragments++; break;
    }
}

// 工厂方法实现
std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::createScanner(const String& source) {
    return std::make_unique<CHTLUnifiedScanner>(source, getDefaultConfig());
}

std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::createScannerWithConfig(
    const String& source, const ScannerConfig& config) {
    return std::make_unique<CHTLUnifiedScanner>(source, config);
}

ScannerConfig ScannerFactory::getDefaultConfig() {
    return ScannerConfig{};
}

ScannerConfig ScannerFactory::getPerformanceConfig() {
    ScannerConfig config;
    config.enableVariableLengthSlicing = false;
    config.enableContextValidation = false;
    config.enableMinimalUnitSplitting = false;
    config.preserveWhitespace = false;
    config.preserveComments = false;
    config.initialChunkSize = 2048;
    config.maxChunkSize = 8192;
    return config;
}

ScannerConfig ScannerFactory::getDebugConfig() {
    ScannerConfig config;
    config.enableVariableLengthSlicing = true;
    config.enableContextValidation = true;
    config.enableMinimalUnitSplitting = true;
    config.preserveWhitespace = true;
    config.preserveComments = true;
    config.initialChunkSize = 512;
    config.maxChunkSize = 2048;
    config.minValidationLength = 8;
    return config;
}

} // namespace CHTL