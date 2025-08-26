#include "CHTLUnifiedScanner.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace chtl {

// CHTL关键字定义
const std::unordered_map<std::string, bool> CHTLUnifiedScanner::chtlKeywords = {
    // 元素关键字
    {"text", true}, {"style", true}, {"script", true},
    
    // 模板和自定义关键字
    {"[Template]", true}, {"[Custom]", true}, {"[Origin]", true},
    {"[Import]", true}, {"[Namespace]", true}, {"[Configuration]", true},
    
    // 类型标识符
    {"@Style", true}, {"@Element", true}, {"@Var", true},
    {"@Html", true}, {"@JavaScript", true}, {"@Chtl", true}, {"@CJmod", true},
    {"@Config", true},
    
    // 操作关键字
    {"inherit", true}, {"delete", true}, {"insert", true},
    {"after", true}, {"before", true}, {"replace", true},
    {"at top", true}, {"at bottom", true},
    {"from", true}, {"as", true}, {"except", true},
    
    // 特殊关键字
    {"use", true}
};

// CHTL JS关键字定义
const std::unordered_map<std::string, bool> CHTLUnifiedScanner::chtljsKeywords = {
    {"{{", true}, {"}}", true},
    {"->", true},
    {"listen", true}, {"delegate", true}, {"animate", true},
    {"vir", true}, {"iNeverAway", true}, {"printMylove", true}
};

CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : config(), currentPolicy(ScannerPolicy::NORMAL) {
}

CHTLUnifiedScanner::CHTLUnifiedScanner(const ScannerConfig& config)
    : config(config), currentPolicy(ScannerPolicy::NORMAL) {
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& code, const std::string& filePath) {
    std::vector<CodeFragment> fragments;
    
    if (code.empty()) {
        return fragments;
    }
    
    // 创建可变长度切片
    std::vector<std::string> slices = createVariableSlices(code);
    
    size_t currentLine = 1;
    size_t currentColumn = 1;
    
    // 处理每个切片
    for (size_t i = 0; i < slices.size(); ++i) {
        const std::string& slice = slices[i];
        
        // 检查是否需要调整切片边界
        if (i + 1 < slices.size() && config.enableVariableSlicing) {
            std::string adjustedSlice = slice;
            adjustSliceBoundary(adjustedSlice, slices[i + 1]);
            
            if (adjustedSlice != slice) {
                // 合并调整后的切片
                slices[i] = adjustedSlice + slices[i + 1];
                slices.erase(slices.begin() + i + 1);
            }
        }
        
        // 分析片段类型
        CodeFragmentType type = analyzeFragment(slices[i]);
        
        // 进行二次切割
        std::vector<CodeFragment> subFragments = secondarySplit(
            slices[i], type, currentLine, currentColumn
        );
        
        // 添加片段并设置文件路径
        for (auto& fragment : subFragments) {
            fragment.filePath = filePath;
            fragments.push_back(fragment);
        }
        
        // 更新位置
        auto [lines, lastLineLength] = calculatePosition(slices[i], 0);
        currentLine += lines - 1;
        if (lines > 1) {
            currentColumn = lastLineLength + 1;
        } else {
            currentColumn += slices[i].length();
        }
    }
    
    // 上下文分析和片段合并
    if (config.enableContextualAnalysis) {
        analyzeContext(fragments);
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return scan(buffer.str(), filePath);
}

void CHTLUnifiedScanner::setPolicyChangeBegin(const std::string& delimiter, ScannerPolicy policy) {
    policyDelimiterStart = delimiter;
    currentPolicy = policy;
    if (policy == ScannerPolicy::COLLECT) {
        collectedContent.clear();
    }
}

std::string CHTLUnifiedScanner::setPolicyChangeEnd(const std::string& delimiter, ScannerPolicy policy) {
    policyDelimiterEnd = delimiter;
    std::string result = collectedContent;
    collectedContent.clear();
    currentPolicy = policy;
    return result;
}

std::unique_ptr<CHTLUnifiedScanner::CJMODScanner> CHTLUnifiedScanner::getCJMODScanner() {
    return std::make_unique<CJMODScanner>(this);
}

bool CHTLUnifiedScanner::isCHTLKeyword(const std::string& token) const {
    return chtlKeywords.find(token) != chtlKeywords.end();
}

bool CHTLUnifiedScanner::isCHTLJSKeyword(const std::string& token) const {
    return chtljsKeywords.find(token) != chtljsKeywords.end();
}

bool CHTLUnifiedScanner::isValidSlicePoint(const std::string& code, size_t position) const {
    if (position >= code.length()) {
        return true;
    }
    
    // 检查是否在字符串内部
    bool inString = false;
    char stringChar = '\0';
    for (size_t i = 0; i < position; ++i) {
        if (!inString && (code[i] == '"' || code[i] == '\'')) {
            inString = true;
            stringChar = code[i];
        } else if (inString && code[i] == stringChar && (i == 0 || code[i-1] != '\\')) {
            inString = false;
        }
    }
    
    if (inString) {
        return false;
    }
    
    // 检查是否在注释内部
    if (position > 1) {
        if (code.substr(position - 2, 2) == "//" || 
            code.substr(position - 2, 2) == "/*" ||
            code.substr(position - 2, 2) == "--") {
            return false;
        }
    }
    
    // 检查是否在CHTL JS选择器内部
    size_t openBraces = 0;
    for (size_t i = 0; i < position; ++i) {
        if (i + 1 < code.length() && code[i] == '{' && code[i+1] == '{') {
            openBraces++;
            i++; // 跳过第二个{
        } else if (i + 1 < code.length() && code[i] == '}' && code[i+1] == '}') {
            if (openBraces > 0) openBraces--;
            i++; // 跳过第二个}
        }
    }
    
    return openBraces == 0;
}

std::vector<std::string> CHTLUnifiedScanner::createVariableSlices(const std::string& code) {
    std::vector<std::string> slices;
    size_t position = 0;
    size_t sliceSize = config.initialSliceSize;
    
    while (position < code.length()) {
        size_t endPos = std::min(position + sliceSize, code.length());
        
        // 寻找有效的切片点
        while (endPos < code.length() && !isValidSlicePoint(code, endPos)) {
            endPos++;
            if (endPos - position > config.maxSliceSize) {
                // 超过最大切片大小，强制切割
                break;
            }
        }
        
        slices.push_back(code.substr(position, endPos - position));
        position = endPos;
        
        // 动态调整切片大小
        if (endPos - position > sliceSize * 1.5) {
            sliceSize = std::min(sliceSize * 2, config.maxSliceSize);
        }
    }
    
    return slices;
}

void CHTLUnifiedScanner::adjustSliceBoundary(std::string& slice, const std::string& nextSlice) {
    // 检查下一个切片是否可能组成完整的CHTL或CHTL JS片段
    std::string combined = slice + nextSlice.substr(0, std::min(size_t(100), nextSlice.length()));
    
    // 检查CHTL块边界
    size_t openBraces = std::count(slice.begin(), slice.end(), '{');
    size_t closeBraces = std::count(slice.begin(), slice.end(), '}');
    
    if (openBraces > closeBraces) {
        // 可能需要扩展到包含完整的块
        size_t pos = 0;
        size_t braceCount = openBraces - closeBraces;
        
        for (char c : nextSlice) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            pos++;
            
            if (braceCount == 0) {
                slice += nextSlice.substr(0, pos);
                break;
            }
        }
    }
}

CodeFragmentType CHTLUnifiedScanner::analyzeFragment(const std::string& fragment) const {
    std::string trimmed = fragment;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.empty()) {
        return CodeFragmentType::TEXT;
    }
    
    // 检查CHTL关键字
    for (const auto& [keyword, _] : chtlKeywords) {
        if (trimmed.find(keyword) != std::string::npos) {
            return CodeFragmentType::CHTL;
        }
    }
    
    // 检查CHTL JS特征
    if (trimmed.find("{{") != std::string::npos || 
        trimmed.find("->") != std::string::npos ||
        trimmed.find("vir ") != std::string::npos) {
        return CodeFragmentType::CHTL_JS;
    }
    
    // 检查CSS特征
    if (std::regex_search(trimmed, std::regex(R"([\w\-]+\s*:\s*[^;]+;)"))) {
        return CodeFragmentType::CSS;
    }
    
    // 检查JS特征
    if (std::regex_search(trimmed, std::regex(R"((function|const|let|var|if|for|while)\s*\()"))) {
        return CodeFragmentType::JS;
    }
    
    // 检查HTML标签
    if (std::regex_search(trimmed, std::regex(R"(<[^>]+>)"))) {
        return CodeFragmentType::HTML;
    }
    
    return CodeFragmentType::TEXT;
}

bool CHTLUnifiedScanner::isCompleteFragment(const std::string& fragment, CodeFragmentType type) const {
    switch (type) {
        case CodeFragmentType::CHTL: {
            // 检查块是否完整
            int braceCount = 0;
            for (char c : fragment) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
            }
            return braceCount == 0;
        }
        
        case CodeFragmentType::CHTL_JS: {
            // 检查{{}}是否匹配
            size_t openCount = 0;
            for (size_t i = 0; i < fragment.length() - 1; ++i) {
                if (fragment[i] == '{' && fragment[i+1] == '{') {
                    openCount++;
                    i++;
                } else if (fragment[i] == '}' && fragment[i+1] == '}') {
                    if (openCount > 0) openCount--;
                    i++;
                }
            }
            return openCount == 0;
        }
        
        default:
            return true;
    }
}

std::vector<CodeFragment> CHTLUnifiedScanner::secondarySplit(
    const std::string& fragment,
    CodeFragmentType type,
    size_t startLine,
    size_t startColumn) {
    
    std::vector<CodeFragment> results;
    
    if (type == CodeFragmentType::CHTL || type == CodeFragmentType::CHTL_JS) {
        // 基于最小单元进行二次切割
        size_t pos = 0;
        size_t currentLine = startLine;
        size_t currentColumn = startColumn;
        
        while (pos < fragment.length()) {
            size_t nextPos = pos;
            
            // 找到下一个有意义的边界
            if (type == CodeFragmentType::CHTL_JS) {
                // 处理CHTL JS特殊语法
                if (fragment.substr(pos, 2) == "{{") {
                    size_t endPos = fragment.find("}}", pos + 2);
                    if (endPos != std::string::npos) {
                        nextPos = endPos + 2;
                    }
                } else if (fragment.substr(pos, 2) == "->") {
                    nextPos = pos + 2;
                    // 继续查找方法名
                    while (nextPos < fragment.length() && 
                           (std::isalnum(fragment[nextPos]) || fragment[nextPos] == '_')) {
                        nextPos++;
                    }
                }
            }
            
            // 如果没有找到特殊边界，按常规方式切割
            if (nextPos == pos) {
                // 找到下一个空白或特殊字符
                while (nextPos < fragment.length() && 
                       !std::isspace(fragment[nextPos]) &&
                       fragment[nextPos] != '{' && 
                       fragment[nextPos] != '}' &&
                       fragment[nextPos] != ';') {
                    nextPos++;
                }
                if (nextPos < fragment.length()) {
                    nextPos++; // 包含边界字符
                }
            }
            
            if (nextPos > pos) {
                CodeFragment subFragment;
                subFragment.type = type;
                subFragment.content = fragment.substr(pos, nextPos - pos);
                subFragment.startLine = currentLine;
                subFragment.startColumn = currentColumn;
                
                auto [lines, lastLineLength] = calculatePosition(subFragment.content, 0);
                subFragment.endLine = currentLine + lines - 1;
                if (lines > 1) {
                    subFragment.endColumn = lastLineLength;
                } else {
                    subFragment.endColumn = currentColumn + subFragment.content.length() - 1;
                }
                
                results.push_back(subFragment);
                
                // 更新位置
                currentLine = subFragment.endLine;
                currentColumn = (lines > 1) ? lastLineLength + 1 : subFragment.endColumn + 1;
            }
            
            pos = nextPos;
        }
    } else {
        // 其他类型直接作为整体返回
        CodeFragment wholeFragment;
        wholeFragment.type = type;
        wholeFragment.content = fragment;
        wholeFragment.startLine = startLine;
        wholeFragment.startColumn = startColumn;
        
        auto [lines, lastLineLength] = calculatePosition(fragment, 0);
        wholeFragment.endLine = startLine + lines - 1;
        if (lines > 1) {
            wholeFragment.endColumn = lastLineLength;
        } else {
            wholeFragment.endColumn = startColumn + fragment.length() - 1;
        }
        
        results.push_back(wholeFragment);
    }
    
    return results;
}

void CHTLUnifiedScanner::analyzeContext(std::vector<CodeFragment>& fragments) {
    // 合并应该连续的片段
    for (size_t i = 0; i < fragments.size() - 1; ) {
        if (shouldMergeFragments(fragments[i], fragments[i + 1])) {
            // 合并片段
            fragments[i].content += fragments[i + 1].content;
            fragments[i].endLine = fragments[i + 1].endLine;
            fragments[i].endColumn = fragments[i + 1].endColumn;
            
            // 删除已合并的片段
            fragments.erase(fragments.begin() + i + 1);
        } else {
            i++;
        }
    }
}

bool CHTLUnifiedScanner::shouldMergeFragments(const CodeFragment& prev, const CodeFragment& current) const {
    // 相同类型且相邻的片段应该合并
    if (prev.type != current.type) {
        return false;
    }
    
    // 检查是否真正相邻
    if (prev.endLine == current.startLine && prev.endColumn + 1 == current.startColumn) {
        return true;
    }
    
    if (prev.endLine + 1 == current.startLine && current.startColumn == 1) {
        return true;
    }
    
    return false;
}

std::pair<size_t, size_t> CHTLUnifiedScanner::calculatePosition(const std::string& code, size_t offset) const {
    size_t lines = 1;
    size_t lastLineLength = 0;
    
    for (size_t i = offset; i < code.length(); ++i) {
        if (code[i] == '\n') {
            lines++;
            lastLineLength = 0;
        } else {
            lastLineLength++;
        }
    }
    
    return {lines, lastLineLength};
}

// CJMODScanner 实现
void CHTLUnifiedScanner::CJMODScanner::scanKeyword(const std::string& keyword, std::function<void()> callback) {
    capturedKeywords.push_back(keyword);
    if (callback) {
        callback();
    }
}

std::string CHTLUnifiedScanner::CJMODScanner::scanWithDoublePointer(
    const std::string& code,
    const std::string& startKeyword,
    const std::string& endKeyword) {
    
    size_t start = 0;
    size_t end = 0;
    
    // 查找开始关键字
    size_t startPos = code.find(startKeyword);
    if (startPos != std::string::npos) {
        start = startPos + startKeyword.length();
    }
    
    // 查找结束关键字
    size_t endPos = code.find(endKeyword, start);
    if (endPos != std::string::npos) {
        end = endPos;
    } else {
        end = code.length();
    }
    
    return code.substr(start, end - start);
}

std::string CHTLUnifiedScanner::CJMODScanner::preCut(const std::string& code, const std::string& keyword) {
    size_t pos = code.find(keyword);
    if (pos != std::string::npos && pos > 0) {
        // 向前查找合适的截取点
        size_t cutPos = pos;
        while (cutPos > 0 && !std::isspace(code[cutPos - 1])) {
            cutPos--;
        }
        return code.substr(cutPos, pos - cutPos);
    }
    return "";
}

std::string CHTLUnifiedScanner::CJMODScanner::peekKeyword(int offset) const {
    int targetIndex = static_cast<int>(currentPosition) + offset;
    if (targetIndex >= 0 && targetIndex < static_cast<int>(capturedKeywords.size())) {
        return capturedKeywords[targetIndex];
    }
    return "";
}

} // namespace chtl