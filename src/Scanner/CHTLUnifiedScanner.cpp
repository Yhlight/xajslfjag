#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner()
    : sliceSize_(1024), minFragmentSize_(16), maxFragmentSize_(4096),
      debugMode_(false), position_(0), line_(1), column_(1) {
}

void CHTLUnifiedScanner::SetInput(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    errors_.clear();
}

std::vector<CodeFragmentPtr> CHTLUnifiedScanner::ScanAndSlice() {
    if (input_.empty()) {
        return {};
    }
    
    auto fragments = PerformSlicing();
    OptimizeFragmentSequence(fragments);
    
    if (debugMode_) {
        PrintFragments(fragments);
    }
    
    return fragments;
}

void CHTLUnifiedScanner::SetSliceSize(size_t size) {
    sliceSize_ = std::max(size, minFragmentSize_);
}

void CHTLUnifiedScanner::SetMinFragmentSize(size_t size) {
    minFragmentSize_ = size;
}

void CHTLUnifiedScanner::SetMaxFragmentSize(size_t size) {
    maxFragmentSize_ = size;
}

void CHTLUnifiedScanner::EnableDebugMode(bool enable) {
    debugMode_ = enable;
}

bool CHTLUnifiedScanner::HasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CHTLUnifiedScanner::GetErrors() const {
    return errors_;
}

void CHTLUnifiedScanner::ClearErrors() {
    errors_.clear();
}

size_t CHTLUnifiedScanner::GetTotalFragments() const {
    return input_.length() / sliceSize_ + 1;
}

void CHTLUnifiedScanner::PrintFragments(const std::vector<CodeFragmentPtr>& fragments) const {
    std::cout << "=== CHTL Unified Scanner Debug Output ===" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::string typeStr;
        switch (fragment->type) {
            case CodeFragmentType::CHTL: typeStr = "CHTL"; break;
            case CodeFragmentType::CHTL_JS: typeStr = "CHTL_JS"; break;
            case CodeFragmentType::CSS: typeStr = "CSS"; break;
            case CodeFragmentType::JS: typeStr = "JS"; break;
            default: typeStr = "UNKNOWN"; break;
        }
        
        std::cout << "Fragment " << i << " [" << typeStr << "] "
                  << "(" << fragment->startLine << ":" << fragment->startColumn << "-"
                  << fragment->endLine << ":" << fragment->endColumn << "): "
                  << fragment->content.substr(0, 50);
        if (fragment->content.length() > 50) {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
    std::cout << "=== End Scanner Debug Output ===" << std::endl;
}

std::vector<CodeFragmentPtr> CHTLUnifiedScanner::PerformSlicing() {
    std::vector<CodeFragmentPtr> fragments;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    
    while (position_ < input_.length()) {
        size_t sliceStart = position_;
        size_t sliceEnd = std::min(position_ + sliceSize_, input_.length());
        
        // 分析当前切片
        auto fragment = AnalyzeSlice(sliceStart, sliceEnd);
        
        if (fragment->type != CodeFragmentType::UNKNOWN) {
            // 调整边界以确保片段完整性
            size_t adjustedEnd = AdjustFragmentBoundary(sliceEnd, fragment->type);
            fragment->endPosition = adjustedEnd;
            fragment->content = GetSubstring(sliceStart, adjustedEnd);
            
            // 验证片段完整性
            if (!ValidateFragmentIntegrity(fragment)) {
                // 如果不完整，尝试扩展
                adjustedEnd = std::min(adjustedEnd + sliceSize_ / 2, input_.length());
                fragment->endPosition = adjustedEnd;
                fragment->content = GetSubstring(sliceStart, adjustedEnd);
            }
            
            // 执行最小单元切割
            auto minimalFragments = PerformMinimalUnitSlicing(fragment);
            fragments.insert(fragments.end(), minimalFragments.begin(), minimalFragments.end());
            
            position_ = adjustedEnd;
        } else {
            // 未知类型，跳过一个字符
            position_++;
            UpdatePosition(position_);
        }
    }
    
    return fragments;
}

CodeFragmentPtr CHTLUnifiedScanner::AnalyzeSlice(size_t start, size_t end) {
    std::string content = GetSubstring(start, end);
    CodeFragmentType type = DetermineFragmentType(content, start, end);
    
    size_t startLine = line_;
    size_t startColumn = column_;
    
    // 计算结束位置的行列
    size_t endLine = startLine;
    size_t endColumn = startColumn;
    for (size_t i = start; i < end && i < input_.length(); ++i) {
        if (input_[i] == '\n') {
            endLine++;
            endColumn = 1;
        } else {
            endColumn++;
        }
    }
    
    return std::make_shared<CodeFragment>(
        type, content, startLine, startColumn, endLine, endColumn, start, end
    );
}

CodeFragmentType CHTLUnifiedScanner::DetermineFragmentType(const std::string& content, size_t /*start*/, size_t /*end*/) {
    // 优先级顺序：CHTL JS > CHTL > CSS > JS
    
    if (IsCHTLJSFragment(content)) {
        return CodeFragmentType::CHTL_JS;
    }
    
    if (IsCHTLFragment(content)) {
        return CodeFragmentType::CHTL;
    }
    
    if (IsCSSFragment(content)) {
        return CodeFragmentType::CSS;
    }
    
    if (IsJSFragment(content)) {
        return CodeFragmentType::JS;
    }
    
    return CodeFragmentType::UNKNOWN;
}

bool CHTLUnifiedScanner::IsCHTLFragment(const std::string& content) const {
    // 检查CHTL特有语法模式
    std::vector<std::regex> chtlPatterns = {
        std::regex(R"(\[Template\]|\[Custom\]|\[Origin\]|\[Configuration\]|\[Import\]|\[Namespace\])"),
        std::regex(R"(@Style|@Element|@Var|@Html|@JavaScript|@Chtl|@CJmod)"),
        std::regex(R"(\btext\s*\{|\bstyle\s*\{|\bscript\s*\{)"),
        std::regex(R"(\binherit\b|\bdelete\b|\binsert\b|\bexcept\b)"),
        std::regex(R"(\b(div|span|p|h1|h2|h3|body|html)\s*\{)"),
        std::regex(R"(\.[a-zA-Z_][\w-]*\s*\{|#[a-zA-Z_][\w-]*\s*\{)")
    };
    
    for (const auto& pattern : chtlPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCHTLJSFragment(const std::string& content) const {
    // 检查CHTL JS特有语法模式
    std::vector<std::regex> chtljsPatterns = {
        std::regex(R"(\bmodule\s*\{|\blisten\s*\{|\bdelegate\s*\{|\banimate\s*\{)"),
        std::regex(R"(\bvir\s+\w+\s*=)"),
        std::regex(R"(\{\{[^}]*\}\})"),                     // 选择器语法
        std::regex(R"(&->|\{\{[^}]*\}\}->)"),               // 事件绑定和选择器链式调用
        std::regex(R"(\btarget:|duration:|easing:|begin:|end:|when:)"),
        std::regex(R"(\bat\s*:\s*[\d.]+)")                  // 动画关键帧
    };
    
    for (const auto& pattern : chtljsPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCSSFragment(const std::string& content) const {
    // 检查CSS语法模式
    std::vector<std::regex> cssPatterns = {
        std::regex(R"([a-zA-Z-]+\s*:\s*[^;{}]+;)"),         // CSS属性
        std::regex(R"(@media|@keyframes|@import|@charset)"), // CSS指令
        std::regex(R"(\.[a-zA-Z_][\w-]*\s*\{)"),            // 类选择器
        std::regex(R"(#[a-zA-Z_][\w-]*\s*\{)"),             // ID选择器
        std::regex(R"([a-zA-Z]+\s*\{)")                     // 元素选择器
    };
    
    for (const auto& pattern : cssPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsJSFragment(const std::string& content) const {
    // 检查JavaScript语法模式
    std::vector<std::regex> jsPatterns = {
        std::regex(R"(\bfunction\s+\w+|\bconst\s+\w+|\blet\s+\w+|\bvar\s+\w+)"),
        std::regex(R"(\bif\s*\(|\bfor\s*\(|\bwhile\s*\()"),
        std::regex(R"(\breturn\b|\bthrow\b|\btry\b|\bcatch\b)"),
        std::regex(R"(\w+\s*\(\s*\)|console\.|document\.|window\.)"),
        std::regex(R"(addEventListener|getElementById|querySelector)")
    };
    
    for (const auto& pattern : jsPatterns) {
        if (std::regex_search(content, pattern)) {
            return true;
        }
    }
    
    return false;
}

size_t CHTLUnifiedScanner::AdjustFragmentBoundary(size_t position, CodeFragmentType expectedType) {
    switch (expectedType) {
        case CodeFragmentType::CHTL:
            return IsValidCHTLBoundary(position) ? position : 
                   std::min(position + sliceSize_ / 4, input_.length());
        case CodeFragmentType::CHTL_JS:
            return IsValidCHTLJSBoundary(position) ? position :
                   std::min(position + sliceSize_ / 4, input_.length());
        case CodeFragmentType::CSS:
            return IsValidCSSBoundary(position) ? position :
                   std::min(position + sliceSize_ / 4, input_.length());
        case CodeFragmentType::JS:
            return IsValidJSBoundary(position) ? position :
                   std::min(position + sliceSize_ / 4, input_.length());
        default:
            return position;
    }
}

bool CHTLUnifiedScanner::IsValidCHTLBoundary(size_t position) const {
    if (position >= input_.length()) return true;
    
    char current = GetCharAt(position);
    char prev = position > 0 ? GetCharAt(position - 1) : '\0';
    
    // 在结构边界处是有效的
    return current == '}' || current == ';' || current == '\n' || 
           std::isspace(current) || (prev == '}' && std::isspace(current));
}

bool CHTLUnifiedScanner::IsValidCHTLJSBoundary(size_t position) const {
    if (position >= input_.length()) return true;
    
    char current = GetCharAt(position);
    
    // JavaScript语法边界
    return current == '}' || current == ';' || current == '\n' || 
           std::isspace(current);
}

bool CHTLUnifiedScanner::IsValidCSSBoundary(size_t position) const {
    if (position >= input_.length()) return true;
    
    char current = GetCharAt(position);
    
    // CSS语法边界
    return current == '}' || current == ';' || std::isspace(current);
}

bool CHTLUnifiedScanner::IsValidJSBoundary(size_t position) const {
    if (position >= input_.length()) return true;
    
    char current = GetCharAt(position);
    
    // JavaScript语法边界
    return current == '}' || current == ';' || current == '\n' || 
           std::isspace(current);
}

std::vector<CodeFragmentPtr> CHTLUnifiedScanner::PerformMinimalUnitSlicing(const CodeFragmentPtr& fragment) {
    std::vector<CodeFragmentPtr> result;
    
    if (fragment->type == CodeFragmentType::CHTL || fragment->type == CodeFragmentType::CHTL_JS) {
        std::vector<size_t> units;
        
        if (fragment->type == CodeFragmentType::CHTL) {
            units = FindCHTLMinimalUnits(fragment->content);
        } else {
            units = FindCHTLJSMinimalUnits(fragment->content);
        }
        
        if (units.size() <= 1) {
            result.push_back(fragment);
        } else {
            // 根据最小单元进行切分
            for (size_t i = 0; i < units.size() - 1; ++i) {
                size_t start = fragment->startPosition + units[i];
                size_t end = fragment->startPosition + units[i + 1];
                
                auto subFragment = std::make_shared<CodeFragment>(
                    fragment->type,
                    GetSubstring(start, end),
                    fragment->startLine, fragment->startColumn,
                    fragment->endLine, fragment->endColumn,
                    start, end
                );
                
                result.push_back(subFragment);
            }
        }
    } else {
        result.push_back(fragment);
    }
    
    return result;
}

std::vector<size_t> CHTLUnifiedScanner::FindCHTLMinimalUnits(const std::string& content) const {
    std::vector<size_t> units;
    units.push_back(0);
    
    std::regex chtlUnitPattern(R"(\{\{[^}]*\}\}->|\{\{[^}]*\}\}|[a-zA-Z_][\w-]*\s*\{)");
    std::sregex_iterator iter(content.begin(), content.end(), chtlUnitPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        units.push_back(iter->position());
        units.push_back(iter->position() + iter->length());
    }
    
    units.push_back(content.length());
    
    // 去重并排序
    std::sort(units.begin(), units.end());
    units.erase(std::unique(units.begin(), units.end()), units.end());
    
    return units;
}

std::vector<size_t> CHTLUnifiedScanner::FindCHTLJSMinimalUnits(const std::string& content) const {
    std::vector<size_t> units;
    units.push_back(0);
    
    std::regex chtljsUnitPattern(R"(\{\{[^}]*\}\}->|\{\{[^}]*\}\}|&->|\bvir\s+\w+|[a-zA-Z_][\w-]*\s*\{)");
    std::sregex_iterator iter(content.begin(), content.end(), chtljsUnitPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        units.push_back(iter->position());
        units.push_back(iter->position() + iter->length());
    }
    
    units.push_back(content.length());
    
    // 去重并排序
    std::sort(units.begin(), units.end());
    units.erase(std::unique(units.begin(), units.end()), units.end());
    
    return units;
}

bool CHTLUnifiedScanner::RequiresContextMerging(const CodeFragmentPtr& current, const CodeFragmentPtr& next) const {
    // 如果两个片段类型相同且相邻，可能需要合并
    return current->type == next->type && 
           current->endPosition == next->startPosition &&
           (current->content.length() + next->content.length()) <= maxFragmentSize_;
}

CodeFragmentPtr CHTLUnifiedScanner::MergeFragments(const CodeFragmentPtr& first, const CodeFragmentPtr& second) const {
    return std::make_shared<CodeFragment>(
        first->type,
        first->content + second->content,
        first->startLine, first->startColumn,
        second->endLine, second->endColumn,
        first->startPosition, second->endPosition
    );
}

bool CHTLUnifiedScanner::ValidateFragmentIntegrity(const CodeFragmentPtr& fragment) const {
    switch (fragment->type) {
        case CodeFragmentType::CHTL:
            return IsCHTLComplete(fragment->content);
        case CodeFragmentType::CHTL_JS:
            return IsCHTLJSComplete(fragment->content);
        default:
            return true; // CSS和JS的完整性检查相对简单
    }
}

bool CHTLUnifiedScanner::IsCHTLComplete(const std::string& content) const {
    // 简单的括号匹配检查
    int braceCount = 0;
    int bracketCount = 0;
    
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        else if (c == '[') bracketCount++;
        else if (c == ']') bracketCount--;
    }
    
    return braceCount == 0 && bracketCount == 0;
}

bool CHTLUnifiedScanner::IsCHTLJSComplete(const std::string& content) const {
    // CHTL JS的完整性检查
    int braceCount = 0;
    int parenCount = 0;
    bool inSelector = false;
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        if (i < content.length() - 1 && content.substr(i, 2) == "{{") {
            inSelector = true;
            i++; // 跳过下一个字符
            continue;
        }
        
        if (i < content.length() - 1 && content.substr(i, 2) == "}}") {
            inSelector = false;
            i++; // 跳过下一个字符
            continue;
        }
        
        if (!inSelector) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
            else if (c == '(') parenCount++;
            else if (c == ')') parenCount--;
        }
    }
    
    return braceCount == 0 && parenCount == 0 && !inSelector;
}

char CHTLUnifiedScanner::GetCharAt(size_t position) const {
    return position < input_.length() ? input_[position] : '\0';
}

std::string CHTLUnifiedScanner::GetSubstring(size_t start, size_t end) const {
    if (start >= input_.length()) return "";
    end = std::min(end, input_.length());
    return input_.substr(start, end - start);
}

bool CHTLUnifiedScanner::MatchPattern(size_t position, const std::string& pattern) const {
    if (position + pattern.length() > input_.length()) return false;
    return input_.substr(position, pattern.length()) == pattern;
}

void CHTLUnifiedScanner::UpdatePosition(size_t newPosition) {
    while (position_ < newPosition && position_ < input_.length()) {
        if (input_[position_] == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        position_++;
    }
}

void CHTLUnifiedScanner::ReportError(const std::string& message) {
    std::string error = "Line " + std::to_string(line_) + 
                       ", Column " + std::to_string(column_) + ": " + message;
    errors_.push_back(error);
}

void CHTLUnifiedScanner::OptimizeFragmentSequence(std::vector<CodeFragmentPtr>& fragments) {
    MergeAdjacentSameTypeFragments(fragments);
    AdjustFragmentSizes(fragments);
}

void CHTLUnifiedScanner::MergeAdjacentSameTypeFragments(std::vector<CodeFragmentPtr>& fragments) {
    if (fragments.size() <= 1) return;
    
    std::vector<CodeFragmentPtr> merged;
    merged.reserve(fragments.size());
    
    auto current = fragments[0];
    
    for (size_t i = 1; i < fragments.size(); ++i) {
        auto next = fragments[i];
        
        if (RequiresContextMerging(current, next)) {
            current = MergeFragments(current, next);
        } else {
            merged.push_back(current);
            current = next;
        }
    }
    
    merged.push_back(current);
    fragments = std::move(merged);
}

void CHTLUnifiedScanner::AdjustFragmentSizes(std::vector<CodeFragmentPtr>& fragments) {
    for (auto& fragment : fragments) {
        if (fragment->content.length() < minFragmentSize_) {
            // 小片段标记为需要合并
            continue;
        }
        
        if (fragment->content.length() > maxFragmentSize_) {
            // 大片段需要进一步分割
            // 这里可以实现更复杂的分割逻辑
            continue;
        }
    }
}

} // namespace CHTL