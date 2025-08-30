#include "UnifiedScanner.h"
#include "../Error/ErrorReporter.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace CHTL {

UnifiedScanner::UnifiedScanner() : m_debugMode(false) {}

UnifiedScanner::~UnifiedScanner() = default;

std::vector<CodeFragment> UnifiedScanner::scanCode(const std::string& code) {
    if (m_debugMode) {
        std::cout << "🔍 开始统一扫描，代码长度: " << code.length() << std::endl;
    }
    
    // 清理统计信息
    m_fragmentCounts.clear();
    m_scanErrors.clear();
    m_scanWarnings.clear();
    
    // 第一阶段：可变长度切片扫描
    std::vector<CodeFragment> initialFragments = variableLengthSlicing(code);
    
    // 第二阶段：最小单元切割
    std::vector<CodeFragment> refinedFragments;
    for (const auto& fragment : initialFragments) {
        if (fragment.needsForwardExpansion) {
            CodeFragment expanded = expandFragment(fragment, code);
            auto minimalUnits = minimalUnitCutting(expanded);
            refinedFragments.insert(refinedFragments.end(), minimalUnits.begin(), minimalUnits.end());
        } else {
            auto minimalUnits = minimalUnitCutting(fragment);
            refinedFragments.insert(refinedFragments.end(), minimalUnits.begin(), minimalUnits.end());
        }
    }
    
    // 第三阶段：上下文连续性保持
    std::vector<CodeFragment> finalFragments = mergeContinuousFragments(refinedFragments);
    
    // 第四阶段：优化和验证
    optimizeFragments(finalFragments);
    
    // 更新统计信息
    for (const auto& fragment : finalFragments) {
        m_fragmentCounts[fragment.type]++;
    }
    
    if (m_debugMode) {
        printScanStatistics();
    }
    
    return finalFragments;
}

std::vector<CodeFragment> UnifiedScanner::variableLengthSlicing(const std::string& code) {
    std::vector<CodeFragment> fragments;
    ScanContext context = createInitialContext();
    
    size_t pos = 0;
    size_t sliceStart = 0;
    size_t baseSliceSize = 100;  // 基础切片大小
    
    while (pos < code.length()) {
        size_t currentSliceSize = baseSliceSize;
        size_t sliceEnd = std::min(pos + currentSliceSize, code.length());
        
        // 提取当前切片
        std::string slice = code.substr(pos, sliceEnd - pos);
        
        // 检查是否需要前向扩增
        CodeFragment fragment;
        fragment.content = slice;
        fragment.startPos = pos;
        fragment.endPos = sliceEnd;
        fragment.lineNumber = std::count(code.begin(), code.begin() + pos, '\n') + 1;
        fragment.context = inferContextFromSurrounding(code, pos);
        fragment.type = detectFragmentType(slice, context);
        fragment.needsForwardExpansion = needsForwardExpansion(fragment, code);
        
        if (fragment.needsForwardExpansion) {
            // 需要扩增，寻找合适的边界
            size_t expandedEnd = findNextSignificantPosition(code, sliceEnd);
            if (expandedEnd > sliceEnd && expandedEnd <= code.length()) {
                fragment.content = code.substr(pos, expandedEnd - pos);
                fragment.endPos = expandedEnd;
                pos = expandedEnd;
            } else {
                pos = sliceEnd;
            }
        } else {
            pos = sliceEnd;
        }
        
        // 更新上下文
        updateContext(context, fragment.content);
        
        fragments.push_back(fragment);
    }
    
    if (m_debugMode) {
        std::cout << "  ✓ 可变长度切片完成，生成 " << fragments.size() << " 个片段" << std::endl;
    }
    
    return fragments;
}

bool UnifiedScanner::needsForwardExpansion(const CodeFragment& fragment, const std::string& fullCode) {
    // 检查片段是否在关键语法结构中间被截断
    
    // 1. 检查是否在块结构中间
    size_t openBraces = std::count(fragment.content.begin(), fragment.content.end(), '{');
    size_t closeBraces = std::count(fragment.content.begin(), fragment.content.end(), '}');
    
    if (openBraces != closeBraces) {
        return true;  // 块结构不完整，需要扩增
    }
    
    // 2. 检查是否截断了CHTL关键语法
    std::vector<std::string> chtlKeywords = getCHTLKeywords();
    for (const auto& keyword : chtlKeywords) {
        size_t keywordPos = fragment.content.find(keyword);
        if (keywordPos != std::string::npos) {
            // 检查关键字后是否有完整的块
            size_t afterKeyword = keywordPos + keyword.length();
            std::string remaining = fragment.content.substr(afterKeyword);
            
            // 如果关键字后没有完整的块，需要扩增
            if (remaining.find('{') != std::string::npos && remaining.find('}') == std::string::npos) {
                return true;
            }
        }
    }
    
    // 3. 检查CHTL JS语法
    if (hasEnhancedSelectors(fragment.content) || hasEventBinding(fragment.content)) {
        // 检查增强选择器和事件绑定是否完整
        std::regex incompleteRegex(R"(\{\{[^}]*$|&->[^{]*$)");
        if (std::regex_search(fragment.content, incompleteRegex)) {
            return true;
        }
    }
    
    return false;
}

CodeFragment UnifiedScanner::expandFragment(const CodeFragment& fragment, const std::string& fullCode) {
    CodeFragment expanded = fragment;
    
    // 从当前位置向前扩增，直到找到合适的边界
    size_t newEnd = fragment.endPos;
    int braceBalance = 0;
    bool inString = false;
    char stringChar = 0;
    
    // 计算当前的括号平衡
    for (char c : fragment.content) {
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceBalance++;
            } else if (c == '}') {
                braceBalance--;
            }
        } else if (c == stringChar && (fragment.content.size() > 1 && *((&c) - 1) != '\\')) {
            inString = false;
        }
    }
    
    // 向前扩增直到括号平衡
    while (newEnd < fullCode.length() && braceBalance > 0) {
        char c = fullCode[newEnd];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == '{') {
                braceBalance++;
            } else if (c == '}') {
                braceBalance--;
            }
        } else if (c == stringChar && (newEnd > 0 && fullCode[newEnd - 1] != '\\')) {
            inString = false;
        }
        
        newEnd++;
    }
    
    expanded.content = fullCode.substr(fragment.startPos, newEnd - fragment.startPos);
    expanded.endPos = newEnd;
    expanded.needsForwardExpansion = false;
    
    if (m_debugMode) {
        std::cout << "    ✓ 片段扩增: " << fragment.content.length() << " -> " << expanded.content.length() << " 字符" << std::endl;
    }
    
    return expanded;
}

std::vector<CodeFragment> UnifiedScanner::minimalUnitCutting(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    
    if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_JS) {
        // 对CHTL和CHTL JS进行最小单元切割
        std::vector<std::string> minimalUnits = splitToMinimalUnits(fragment.content, fragment.type);
        
        size_t currentPos = fragment.startPos;
        for (const auto& unit : minimalUnits) {
            CodeFragment unitFragment;
            unitFragment.content = unit;
            unitFragment.type = fragment.type;
            unitFragment.startPos = currentPos;
            unitFragment.endPos = currentPos + unit.length();
            unitFragment.lineNumber = fragment.lineNumber;
            unitFragment.context = fragment.context;
            unitFragment.isMinimalUnit = isMinimalUnit(unit, fragment.type);
            
            units.push_back(unitFragment);
            currentPos += unit.length();
        }
    } else {
        // 其他类型保持原样
        units.push_back(fragment);
    }
    
    return units;
}

std::vector<std::string> UnifiedScanner::splitToMinimalUnits(const std::string& code, FragmentType type) {
    std::vector<std::string> units;
    
    if (type == FragmentType::CHTL_JS) {
        // CHTL JS最小单元切割
        std::regex chtlJSPattern(R"(\{\{[^}]+\}\}->|&->|\w+\s*\{[^}]*\}|[^{}\s]+)");
        std::sregex_iterator iter(code.begin(), code.end(), chtlJSPattern);
        std::sregex_iterator end;
        
        size_t lastPos = 0;
        for (; iter != end; ++iter) {
            // 添加匹配前的内容
            if (iter->position() > lastPos) {
                std::string between = code.substr(lastPos, iter->position() - lastPos);
                if (!between.empty() && !std::all_of(between.begin(), between.end(), ::isspace)) {
                    units.push_back(between);
                }
            }
            
            units.push_back(iter->str());
            lastPos = iter->position() + iter->length();
        }
        
        // 添加剩余内容
        if (lastPos < code.length()) {
            std::string remaining = code.substr(lastPos);
            if (!remaining.empty() && !std::all_of(remaining.begin(), remaining.end(), ::isspace)) {
                units.push_back(remaining);
            }
        }
    } else {
        // CHTL最小单元切割
        std::regex chtlPattern(R"(\[[\w\s]+\]|\w+\s*\{[^}]*\}|@\w+|[^{}\s]+)");
        std::sregex_iterator iter(code.begin(), code.end(), chtlPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            units.push_back(iter->str());
        }
    }
    
    return units;
}

std::vector<CodeFragment> UnifiedScanner::mergeContinuousFragments(const std::vector<CodeFragment>& fragments) {
    std::vector<CodeFragment> merged;
    
    if (fragments.empty()) {
        return merged;
    }
    
    CodeFragment current = fragments[0];
    
    for (size_t i = 1; i < fragments.size(); ++i) {
        if (shouldMaintainContinuity(fragments, i)) {
            // 合并连续片段
            current.content += fragments[i].content;
            current.endPos = fragments[i].endPos;
        } else {
            // 结束当前片段，开始新片段
            merged.push_back(current);
            current = fragments[i];
        }
    }
    
    merged.push_back(current);
    
    if (m_debugMode) {
        std::cout << "  ✓ 连续性合并: " << fragments.size() << " -> " << merged.size() << " 个片段" << std::endl;
    }
    
    return merged;
}

bool UnifiedScanner::shouldMaintainContinuity(const std::vector<CodeFragment>& fragments, size_t index) {
    if (index == 0 || index >= fragments.size()) {
        return false;
    }
    
    const CodeFragment& prev = fragments[index - 1];
    const CodeFragment& curr = fragments[index];
    
    // 相同类型的CHTL或CHTL JS片段应该保持连续
    if (prev.type == curr.type && 
        (prev.type == FragmentType::CHTL || prev.type == FragmentType::CHTL_JS)) {
        
        // 检查是否在同一个逻辑块中
        if (prev.context == curr.context) {
            return true;
        }
    }
    
    return false;
}

FragmentType UnifiedScanner::detectFragmentType(const std::string& code, const ScanContext& context) {
    // 首先检查注释
    if (isGeneratorComment(code)) {
        return FragmentType::COMMENT_GENERATOR;
    }
    
    // 检查普通注释
    std::regex commentRegex(R"(^\s*(//|/\*))");
    if (std::regex_search(code, commentRegex)) {
        return FragmentType::COMMENT_NORMAL;
    }
    
    // 根据上下文和内容检测类型
    if (isCHTLJSCode(code, context)) {
        return FragmentType::CHTL_JS;
    }
    
    if (isCHTLCode(code, context)) {
        return FragmentType::CHTL;
    }
    
    if (isCSSCode(code, context)) {
        return FragmentType::CSS;
    }
    
    if (isJavaScriptCode(code, context)) {
        return FragmentType::JAVASCRIPT;
    }
    
    return FragmentType::UNKNOWN;
}

bool UnifiedScanner::isCHTLCode(const std::string& code, const ScanContext& context) {
    // 检查CHTL特有的语法
    if (hasCHTLKeywords(code) || hasTemplateUsage(code) || hasCustomUsage(code) ||
        hasOriginBlock(code) || hasImportStatement(code) || hasNamespaceDeclaration(code) ||
        hasConfigurationBlock(code)) {
        return true;
    }
    
    // 检查是否在CHTL上下文中
    if (context.inTemplateBlock || context.inCustomBlock) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCHTLJSCode(const std::string& code, const ScanContext& context) {
    // 检查CHTL JS特有的语法
    if (hasEnhancedSelectors(code) || hasEventBinding(code) || hasAnimateBlock(code) ||
        hasListenBlock(code) || hasVirtualObject(code) || hasModuleImport(code) ||
        hasDelegateBlock(code)) {
        return true;
    }
    
    // 检查是否在script块中且包含CHTL JS语法
    if (context.inScriptBlock && hasCHTLJSKeywords(code)) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isCSSCode(const std::string& code, const ScanContext& context) {
    // 检查CSS语法模式
    std::regex cssPattern(R"(^\s*[.#]?\w+\s*\{[^}]*\}\s*$|^\s*@\w+|^\s*\w+\s*:\s*[^;]+;)");
    
    if (std::regex_search(code, cssPattern) && !isCHTLCode(code, context) && !isCHTLJSCode(code, context)) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isJavaScriptCode(const std::string& code, const ScanContext& context) {
    // 检查JavaScript语法模式
    std::regex jsPattern(R"(^\s*(var|let|const|function|if|for|while|return|console\.)|^\s*\w+\s*\([^)]*\)\s*[{;]|^\s*\w+\.\w+)");
    
    if (std::regex_search(code, jsPattern) && !isCHTLJSCode(code, context)) {
        return true;
    }
    
    return false;
}

bool UnifiedScanner::isGeneratorComment(const std::string& code) {
    std::regex generatorCommentRegex(R"(^\s*--\s*.*)");
    return std::regex_search(code, generatorCommentRegex);
}

ScanContext UnifiedScanner::createInitialContext() {
    ScanContext context;
    context.currentNamespace = "";
    context.inStyleBlock = false;
    context.inScriptBlock = false;
    context.inOriginBlock = false;
    context.inTemplateBlock = false;
    context.inCustomBlock = false;
    context.originType = "";
    context.braceDepth = 0;
    context.parenDepth = 0;
    return context;
}

void UnifiedScanner::updateContext(ScanContext& context, const std::string& code) {
    // 更新括号深度
    for (char c : code) {
        if (c == '{') context.braceDepth++;
        else if (c == '}') context.braceDepth--;
        else if (c == '(') context.parenDepth++;
        else if (c == ')') context.parenDepth--;
    }
    
    // 检查块状态
    if (code.find("style {") != std::string::npos) {
        context.inStyleBlock = true;
    }
    if (code.find("script {") != std::string::npos) {
        context.inScriptBlock = true;
    }
    if (code.find("[Template]") != std::string::npos) {
        context.inTemplateBlock = true;
    }
    if (code.find("[Custom]") != std::string::npos) {
        context.inCustomBlock = true;
    }
    if (code.find("[Origin]") != std::string::npos) {
        context.inOriginBlock = true;
        
        // 提取Origin类型
        std::regex originTypeRegex(R"(\[Origin\]\s*@(\w+))");
        std::smatch match;
        if (std::regex_search(code, match, originTypeRegex)) {
            context.originType = match[1].str();
        }
    }
    
    // 检查命名空间
    std::regex namespaceRegex(R"(\[Namespace\]\s+(\w+))");
    std::smatch nsMatch;
    if (std::regex_search(code, nsMatch, namespaceRegex)) {
        context.currentNamespace = nsMatch[1].str();
    }
    
    // 当括号深度为0时，退出相应的块
    if (context.braceDepth == 0) {
        if (context.inStyleBlock || context.inScriptBlock) {
            context.inStyleBlock = false;
            context.inScriptBlock = false;
        }
    }
}

bool UnifiedScanner::hasCHTLKeywords(const std::string& code) {
    std::vector<std::string> keywords = getCHTLKeywords();
    
    for (const auto& keyword : keywords) {
        if (code.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::hasCHTLJSKeywords(const std::string& code) {
    std::vector<std::string> keywords = getCHTLJSKeywords();
    
    for (const auto& keyword : keywords) {
        if (code.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool UnifiedScanner::hasEnhancedSelectors(const std::string& code) {
    std::regex selectorRegex(R"(\{\{[^}]+\}\})");
    return std::regex_search(code, selectorRegex);
}

bool UnifiedScanner::hasEventBinding(const std::string& code) {
    return code.find("&->") != std::string::npos;
}

bool UnifiedScanner::hasAnimateBlock(const std::string& code) {
    std::regex animateRegex(R"(\banimate\s*\{)");
    return std::regex_search(code, animateRegex);
}

bool UnifiedScanner::hasListenBlock(const std::string& code) {
    std::regex listenRegex(R"(\blisten\s*\{)");
    return std::regex_search(code, listenRegex);
}

bool UnifiedScanner::hasVirtualObject(const std::string& code) {
    std::regex virRegex(R"(\bvir\s+\w+\s*=)");
    return std::regex_search(code, virRegex);
}

bool UnifiedScanner::hasModuleImport(const std::string& code) {
    std::regex moduleRegex(R"(\bmodule\s*\{)");
    return std::regex_search(code, moduleRegex);
}

bool UnifiedScanner::hasDelegateBlock(const std::string& code) {
    std::regex delegateRegex(R"(\bdelegate\s*\{)");
    return std::regex_search(code, delegateRegex);
}

bool UnifiedScanner::hasTemplateUsage(const std::string& code) {
    std::regex templateRegex(R"(\[Template\]\s*@\w+|\@\w+\s+\w+)");
    return std::regex_search(code, templateRegex);
}

bool UnifiedScanner::hasCustomUsage(const std::string& code) {
    std::regex customRegex(R"(\[Custom\]\s*@\w+)");
    return std::regex_search(code, customRegex);
}

bool UnifiedScanner::hasOriginBlock(const std::string& code) {
    std::regex originRegex(R"(\[Origin\]\s*@\w+)");
    return std::regex_search(code, originRegex);
}

bool UnifiedScanner::hasImportStatement(const std::string& code) {
    std::regex importRegex(R"(\[Import\])");
    return std::regex_search(code, importRegex);
}

bool UnifiedScanner::hasNamespaceDeclaration(const std::string& code) {
    std::regex namespaceRegex(R"(\[Namespace\])");
    return std::regex_search(code, namespaceRegex);
}

bool UnifiedScanner::hasConfigurationBlock(const std::string& code) {
    std::regex configRegex(R"(\[Configuration\])");
    return std::regex_search(code, configRegex);
}

std::vector<std::string> UnifiedScanner::getCHTLKeywords() {
    return {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]",
        "text", "style", "inherit", "delete", "insert", "replace", "except", "use",
        "from", "as", "after", "before", "at top", "at bottom"
    };
}

std::vector<std::string> UnifiedScanner::getCHTLJSKeywords() {
    return {
        "{{", "}}", "&->", "->", "animate", "listen", "vir", "delegate", "module"
    };
}

size_t UnifiedScanner::findNextSignificantPosition(const std::string& code, size_t startPos) {
    // 寻找下一个重要的语法边界
    std::vector<char> significantChars = {'{', '}', ';', '\n'};
    
    for (size_t i = startPos; i < code.length(); ++i) {
        char c = code[i];
        
        if (std::find(significantChars.begin(), significantChars.end(), c) != significantChars.end()) {
            return i + 1;
        }
    }
    
    return code.length();
}

size_t UnifiedScanner::findBlockEnd(const std::string& code, size_t startPos, char openChar, char closeChar) {
    int depth = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = startPos; i < code.length(); ++i) {
        char c = code[i];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            } else if (c == openChar) {
                depth++;
            } else if (c == closeChar) {
                depth--;
                if (depth == 0) {
                    return i;
                }
            }
        } else if (c == stringChar && (i == 0 || code[i - 1] != '\\')) {
            inString = false;
        }
    }
    
    return std::string::npos;
}

bool UnifiedScanner::isInsideString(const std::string& code, size_t pos) {
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < pos && i < code.length(); ++i) {
        char c = code[i];
        
        if (!inString) {
            if (c == '"' || c == '\'') {
                inString = true;
                stringChar = c;
            }
        } else if (c == stringChar && (i == 0 || code[i - 1] != '\\')) {
            inString = false;
        }
    }
    
    return inString;
}

bool UnifiedScanner::isInsideComment(const std::string& code, size_t pos) {
    // 检查是否在单行注释中
    size_t lineStart = code.rfind('\n', pos);
    if (lineStart == std::string::npos) lineStart = 0;
    
    std::string line = code.substr(lineStart, pos - lineStart);
    if (line.find("//") != std::string::npos) {
        return true;
    }
    
    // 检查是否在多行注释中
    size_t lastCommentStart = code.rfind("/*", pos);
    size_t lastCommentEnd = code.rfind("*/", pos);
    
    if (lastCommentStart != std::string::npos && 
        (lastCommentEnd == std::string::npos || lastCommentStart > lastCommentEnd)) {
        return true;
    }
    
    return false;
}

std::string UnifiedScanner::inferContextFromSurrounding(const std::string& code, size_t pos) {
    // 向前查找最近的块标识
    size_t searchStart = (pos > 200) ? pos - 200 : 0;
    std::string surrounding = code.substr(searchStart, pos - searchStart + 100);
    
    if (surrounding.find("[Template]") != std::string::npos) return "template";
    if (surrounding.find("[Custom]") != std::string::npos) return "custom";
    if (surrounding.find("[Origin]") != std::string::npos) return "origin";
    if (surrounding.find("style {") != std::string::npos) return "style";
    if (surrounding.find("script {") != std::string::npos) return "script";
    
    return "global";
}

bool UnifiedScanner::isMinimalUnit(const std::string& code, FragmentType type) {
    if (type == FragmentType::CHTL_JS) {
        // CHTL JS最小单元判断
        return hasEnhancedSelectors(code) || hasEventBinding(code) || 
               code.length() < 50;  // 简单的长度判断
    }
    
    if (type == FragmentType::CHTL) {
        // CHTL最小单元判断
        return hasTemplateUsage(code) || hasCustomUsage(code) || 
               code.length() < 100;  // 简单的长度判断
    }
    
    return true;
}

void UnifiedScanner::optimizeFragments(std::vector<CodeFragment>& fragments) {
    removeEmptyFragments(fragments);
    mergeCompatibleFragments(fragments);
}

void UnifiedScanner::removeEmptyFragments(std::vector<CodeFragment>& fragments) {
    fragments.erase(
        std::remove_if(fragments.begin(), fragments.end(),
            [](const CodeFragment& fragment) {
                return fragment.content.empty() || 
                       std::all_of(fragment.content.begin(), fragment.content.end(), ::isspace);
            }),
        fragments.end()
    );
}

void UnifiedScanner::mergeCompatibleFragments(std::vector<CodeFragment>& fragments) {
    if (fragments.size() < 2) return;
    
    std::vector<CodeFragment> merged;
    CodeFragment current = fragments[0];
    
    for (size_t i = 1; i < fragments.size(); ++i) {
        const CodeFragment& next = fragments[i];
        
        // 如果类型相同且连续，则合并
        if (current.type == next.type && current.endPos == next.startPos) {
            current.content += next.content;
            current.endPos = next.endPos;
        } else {
            merged.push_back(current);
            current = next;
        }
    }
    
    merged.push_back(current);
    fragments = merged;
}

bool UnifiedScanner::validateFragment(const CodeFragment& fragment) {
    if (fragment.content.empty()) {
        return false;
    }
    
    if (fragment.startPos >= fragment.endPos) {
        return false;
    }
    
    return true;
}

void UnifiedScanner::printScanStatistics() {
    std::cout << "\n📊 扫描统计信息:\n";
    std::cout << "----------------------------------------\n";
    
    for (const auto& pair : m_fragmentCounts) {
        std::string typeName;
        switch (pair.first) {
            case FragmentType::CHTL: typeName = "CHTL"; break;
            case FragmentType::CHTL_JS: typeName = "CHTL JS"; break;
            case FragmentType::CSS: typeName = "CSS"; break;
            case FragmentType::JAVASCRIPT: typeName = "JavaScript"; break;
            case FragmentType::HTML: typeName = "HTML"; break;
            case FragmentType::COMMENT_GENERATOR: typeName = "生成器注释"; break;
            case FragmentType::COMMENT_NORMAL: typeName = "普通注释"; break;
            default: typeName = "未知"; break;
        }
        
        std::cout << "  " << typeName << ": " << pair.second << " 个片段\n";
    }
    
    if (!m_scanErrors.empty()) {
        std::cout << "\n⚠️ 扫描错误:\n";
        for (const auto& error : m_scanErrors) {
            std::cout << "  ❌ " << error << "\n";
        }
    }
    
    if (!m_scanWarnings.empty()) {
        std::cout << "\n⚠️ 扫描警告:\n";
        for (const auto& warning : m_scanWarnings) {
            std::cout << "  ⚠️ " << warning << "\n";
        }
    }
}

void UnifiedScanner::reportError(const std::string& message) {
    m_scanErrors.push_back(message);
    ErrorReporter reporter;
    reporter.error("UnifiedScanner", message);
}

void UnifiedScanner::reportWarning(const std::string& message) {
    m_scanWarnings.push_back(message);
    ErrorReporter reporter;
    reporter.warning("UnifiedScanner", message);
}

}