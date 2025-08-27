#include "CHTLUnifiedScanner.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const ScannerConfig& cfg)
    : config(cfg), currentPosition(0), currentLine(1), currentColumn(1) {
    Initialize();
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

void CHTLUnifiedScanner::SetSourceCode(const std::string& code) {
    sourceCode = code;
    Reset();
}

void CHTLUnifiedScanner::SetConfig(const ScannerConfig& cfg) {
    config = cfg;
}

bool CHTLUnifiedScanner::Scan() {
    if (sourceCode.empty()) {
        AddError("源代码为空");
        return false;
    }
    
    try {
        // 执行初始分片
        PerformInitialChunking();
        
        // 如果启用了最小单元切割，进行二次分割
        if (config.enableMinimalUnitSplit) {
            std::vector<CodeFragment> refinedFragments;
            for (const auto& fragment : fragments) {
                if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_JS) {
                    auto subFragments = PerformMinimalUnitSplit(fragment);
                    refinedFragments.insert(refinedFragments.end(), subFragments.begin(), subFragments.end());
                } else {
                    refinedFragments.push_back(fragment);
                }
            }
            fragments = std::move(refinedFragments);
        }
        
        // 合并连续的同类型片段
        MergeConsecutiveFragments();
        
        // 优化片段分布
        OptimizeFragmentDistribution();
        
        // 验证片段完整性
        if (!ValidateFragments()) {
            AddError("片段验证失败");
            return false;
        }
        
        return errors.empty();
    }
    catch (const std::exception& e) {
        AddError("扫描过程中发生异常: " + std::string(e.what()));
        return false;
    }
}

const std::vector<CodeFragment>& CHTLUnifiedScanner::GetFragments() const {
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::GetFragmentsByType(FragmentType type) const {
    std::vector<CodeFragment> result;
    std::copy_if(fragments.begin(), fragments.end(), std::back_inserter(result),
                [type](const CodeFragment& fragment) {
                    return fragment.type == type;
                });
    return result;
}

const std::vector<std::string>& CHTLUnifiedScanner::GetErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLUnifiedScanner::GetWarnings() const {
    return warnings;
}

void CHTLUnifiedScanner::Reset() {
    fragments.clear();
    errors.clear();
    warnings.clear();
    currentPosition = 0;
    currentLine = 1;
    currentColumn = 1;
}

bool CHTLUnifiedScanner::ValidateFragments() const {
    if (fragments.empty()) {
        return false;
    }
    
    // 检查片段连续性
    size_t expectedOffset = 0;
    for (const auto& fragment : fragments) {
        if (fragment.startOffset != expectedOffset) {
            return false;
        }
        if (!ValidateFragment(fragment)) {
            return false;
        }
        expectedOffset = fragment.endOffset;
    }
    
    // 检查是否覆盖了整个源代码
    return expectedOffset == sourceCode.length();
}

std::string CHTLUnifiedScanner::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL统一扫描器统计信息:\n";
    oss << "  源代码长度: " << sourceCode.length() << " 字符\n";
    oss << "  片段总数: " << fragments.size() << "\n";
    
    std::unordered_map<FragmentType, int> typeCounts;
    for (const auto& fragment : fragments) {
        typeCounts[fragment.type]++;
    }
    
    for (const auto& pair : typeCounts) {
        oss << "  " << FragmentTypeToString(pair.first) << " 片段: " << pair.second << "\n";
    }
    
    oss << "  错误数量: " << errors.size() << "\n";
    oss << "  警告数量: " << warnings.size() << "\n";
    
    return oss.str();
}

void CHTLUnifiedScanner::Initialize() {
    // 初始化扫描器状态
    Reset();
}

void CHTLUnifiedScanner::PerformInitialChunking() {
    size_t pos = 0;
    
    while (pos < sourceCode.length()) {
        size_t chunkStart = pos;
        size_t chunkSize = std::min(config.initialChunkSize, sourceCode.length() - pos);
        size_t chunkEnd = chunkStart + chunkSize;
        
        // 检查当前切片边界的合理性
        if (!ValidateFragmentBoundary(chunkStart, chunkEnd)) {
            // 如果边界不合理，尝试扩展片段
            if (config.enableAdaptiveExpansion) {
                chunkEnd = ExpandFragmentForCompleteness(chunkStart, chunkEnd);
            }
        }
        
        // 确保不超过最大切片大小
        if (chunkEnd - chunkStart > config.maxChunkSize) {
            chunkEnd = chunkStart + config.maxChunkSize;
        }
        
        // 确保不小于最小切片大小（除非到达文件末尾）
        if (chunkEnd - chunkStart < config.minChunkSize && chunkEnd < sourceCode.length()) {
            chunkEnd = std::min(chunkStart + config.minChunkSize, sourceCode.length());
        }
        
        std::string chunkContent = sourceCode.substr(chunkStart, chunkEnd - chunkStart);
        FragmentType type = AnalyzeFragmentType(chunkContent, chunkStart);
        
        AddFragment(chunkContent, type, chunkStart, chunkEnd);
        
        pos = chunkEnd;
    }
}

FragmentType CHTLUnifiedScanner::AnalyzeFragmentType(const std::string& fragment, size_t startPos) const {
    // 统计各种语法模式的出现次数
    int chtlPatterns = 0;
    int chtlJSPatterns = 0;
    int cssPatterns = 0;
    int jsPatterns = 0;
    
    size_t pos = 0;
    while (pos < fragment.length()) {
        if (DetectCHTLPattern(fragment, pos)) {
            chtlPatterns++;
        }
        if (DetectCHTLJSPattern(fragment, pos)) {
            chtlJSPatterns++;
        }
        if (DetectCSSPattern(fragment, pos)) {
            cssPatterns++;
        }
        if (DetectJSPattern(fragment, pos)) {
            jsPatterns++;
        }
        pos++;
    }
    
    // 根据模式数量判断片段类型
    if (chtlPatterns > 0 && chtlJSPatterns > 0) {
        return FragmentType::MIXED;
    } else if (chtlPatterns > 0) {
        return FragmentType::CHTL;
    } else if (chtlJSPatterns > 0) {
        return FragmentType::CHTL_JS;
    } else if (cssPatterns > jsPatterns) {
        return FragmentType::CSS;
    } else if (jsPatterns > 0) {
        return FragmentType::JS;
    }
    
    return FragmentType::UNKNOWN;
}

bool CHTLUnifiedScanner::ValidateFragmentBoundary(size_t startPos, size_t endPos) const {
    if (endPos >= sourceCode.length()) {
        return true; // 文件末尾总是有效边界
    }
    
    // 检查是否在字符串字面量中间切断
    size_t pos = startPos;
    bool inString = false;
    char stringChar = '\0';
    
    while (pos < endPos && pos < sourceCode.length()) {
        char c = sourceCode[pos];
        
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar) {
            inString = false;
        }
        
        pos++;
    }
    
    // 如果在字符串中间切断，边界不合理
    if (inString) {
        return false;
    }
    
    // 检查是否在CHTL语法中间切断
    if (IsCHTLVariableReference(sourceCode, endPos - 2) || 
        IsCHTLOperator(sourceCode, endPos - 2)) {
        return false;
    }
    
    return true;
}

size_t CHTLUnifiedScanner::ExpandFragmentForCompleteness(size_t startPos, size_t currentEndPos) const {
    size_t newEndPos = currentEndPos;
    
    // 向前查找完整的语法单元
    while (newEndPos < sourceCode.length() && 
           newEndPos - startPos < config.maxChunkSize) {
        
        if (ValidateFragmentBoundary(startPos, newEndPos)) {
            break;
        }
        
        newEndPos++;
    }
    
    return std::min(newEndPos, sourceCode.length());
}

std::vector<CodeFragment> CHTLUnifiedScanner::PerformMinimalUnitSplit(const CodeFragment& fragment) const {
    std::vector<CodeFragment> result;
    
    if (fragment.type != FragmentType::CHTL && fragment.type != FragmentType::CHTL_JS) {
        result.push_back(fragment);
        return result;
    }
    
    size_t pos = 0;
    size_t currentStart = 0;
    
    while (pos < fragment.content.length()) {
        size_t unitEnd = FindMinimalUnitBoundary(fragment.content, pos, fragment.type);
        
        if (unitEnd > pos) {
            // 检查是否需要与后续内容连接
            bool shouldContinue = false;
            if (config.enableContextAnalysis) {
                std::string before = fragment.content.substr(currentStart, unitEnd - currentStart);
                std::string after = (unitEnd < fragment.content.length()) ? 
                                   fragment.content.substr(unitEnd, std::min(size_t(50), fragment.content.length() - unitEnd)) : "";
                shouldContinue = CheckContextDependency(before, after);
            }
            
            if (!shouldContinue || unitEnd == fragment.content.length()) {
                // 创建子片段
                std::string subContent = fragment.content.substr(currentStart, unitEnd - currentStart);
                if (!subContent.empty()) {
                    size_t subStartOffset = fragment.startOffset + currentStart;
                    size_t subEndOffset = fragment.startOffset + unitEnd;
                    auto [startLine, startCol] = CalculateLineColumn(subStartOffset);
                    auto [endLine, endCol] = CalculateLineColumn(subEndOffset);
                    
                    result.emplace_back(subContent, fragment.type, 
                                      startLine, startCol, endLine, endCol,
                                      subStartOffset, subEndOffset);
                }
                currentStart = unitEnd;
            }
            
            pos = unitEnd;
        } else {
            pos++;
        }
    }
    
    // 处理剩余内容
    if (currentStart < fragment.content.length()) {
        std::string remainingContent = fragment.content.substr(currentStart);
        if (!remainingContent.empty()) {
            size_t subStartOffset = fragment.startOffset + currentStart;
            size_t subEndOffset = fragment.endOffset;
            auto [startLine, startCol] = CalculateLineColumn(subStartOffset);
            auto [endLine, endCol] = CalculateLineColumn(subEndOffset);
            
            result.emplace_back(remainingContent, fragment.type,
                              startLine, startCol, endLine, endCol,
                              subStartOffset, subEndOffset);
        }
    }
    
    return result.empty() ? std::vector<CodeFragment>{fragment} : result;
}

bool CHTLUnifiedScanner::DetectCHTLPattern(const std::string& text, size_t pos) const {
    if (pos >= text.length()) return false;
    
    // 检测CHTL语法模式
    static const std::vector<std::string> chtlPatterns = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]",
        "style", "script", "text", "@Style", "@Element", "@Var", "@Html", "@JavaScript"
    };
    
    for (const std::string& pattern : chtlPatterns) {
        if (pos + pattern.length() <= text.length() &&
            text.substr(pos, pattern.length()) == pattern) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::DetectCHTLJSPattern(const std::string& text, size_t pos) const {
    if (pos >= text.length()) return false;
    
    // 检测CHTL JS语法模式
    if (IsCHTLVariableReference(text, pos) || IsCHTLOperator(text, pos)) {
        return true;
    }
    
    static const std::vector<std::string> chtlJSPatterns = {
        "vir", "listen", "delegate", "animate", "&->"
    };
    
    for (const std::string& pattern : chtlJSPatterns) {
        if (pos + pattern.length() <= text.length() &&
            text.substr(pos, pattern.length()) == pattern) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::DetectCSSPattern(const std::string& text, size_t pos) const {
    if (pos >= text.length()) return false;
    
    // 简单的CSS模式检测
    static const std::vector<std::string> cssPatterns = {
        "color:", "background:", "font-", "margin:", "padding:", "border:"
    };
    
    for (const std::string& pattern : cssPatterns) {
        if (pos + pattern.length() <= text.length() &&
            text.substr(pos, pattern.length()) == pattern) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::DetectJSPattern(const std::string& text, size_t pos) const {
    if (pos >= text.length()) return false;
    
    // 简单的JavaScript模式检测
    static const std::vector<std::string> jsPatterns = {
        "function", "var", "let", "const", "if", "for", "while", "console.", "document."
    };
    
    for (const std::string& pattern : jsPatterns) {
        if (pos + pattern.length() <= text.length() &&
            text.substr(pos, pattern.length()) == pattern) {
            return true;
        }
    }
    
    return false;
}

size_t CHTLUnifiedScanner::FindMinimalUnitBoundary(const std::string& text, size_t startPos, FragmentType type) const {
    size_t pos = startPos;
    
    if (type == FragmentType::CHTL_JS) {
        // 查找CHTL JS最小单元边界
        if (pos < text.length() && text.substr(pos, 2) == "{{") {
            // 查找对应的}}
            pos += 2;
            while (pos < text.length() - 1) {
                if (text.substr(pos, 2) == "}}") {
                    return pos + 2;
                }
                pos++;
            }
        }
        
        if (pos < text.length() && text.substr(pos, 2) == "->") {
            return pos + 2;
        }
        
        if (pos < text.length() && text.substr(pos, 3) == "&->") {
            return pos + 3;
        }
    }
    
    // 默认情况下查找下一个空白字符或特殊符号
    while (pos < text.length() && !std::isspace(text[pos]) && 
           text[pos] != '{' && text[pos] != '}' && text[pos] != ';') {
        pos++;
    }
    
    return pos;
}

bool CHTLUnifiedScanner::IsCHTLVariableReference(const std::string& text, size_t pos) const {
    return pos + 1 < text.length() && text.substr(pos, 2) == "{{";
}

bool CHTLUnifiedScanner::IsCHTLOperator(const std::string& text, size_t pos) const {
    if (pos + 1 < text.length() && text.substr(pos, 2) == "->") {
        return true;
    }
    if (pos + 2 < text.length() && text.substr(pos, 3) == "&->") {
        return true;
    }
    return false;
}

bool CHTLUnifiedScanner::CheckContextDependency(const std::string& before, const std::string& after) const {
    // 检查前后文是否有依赖关系
    // 例如：{{box}}->click 应该保持在一起
    
    if (before.length() >= 2 && before.substr(before.length() - 2) == "}}") {
        if (after.length() >= 2 && after.substr(0, 2) == "->") {
            return true; // 选择器和操作符应该连在一起
        }
    }
    
    return false;
}

size_t CHTLUnifiedScanner::SkipWhitespace(size_t pos) const {
    while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
        pos++;
    }
    return pos;
}

size_t CHTLUnifiedScanner::FindNextNonWhitespace(size_t pos) const {
    return SkipWhitespace(pos);
}

size_t CHTLUnifiedScanner::FindMatchingBrace(size_t startPos, char openBrace, char closeBrace) const {
    if (startPos >= sourceCode.length() || sourceCode[startPos] != openBrace) {
        return std::string::npos;
    }
    
    int depth = 1;
    size_t pos = startPos + 1;
    
    while (pos < sourceCode.length() && depth > 0) {
        if (sourceCode[pos] == openBrace) {
            depth++;
        } else if (sourceCode[pos] == closeBrace) {
            depth--;
        }
        pos++;
    }
    
    return (depth == 0) ? pos - 1 : std::string::npos;
}

void CHTLUnifiedScanner::UpdatePosition(size_t newPos) {
    while (currentPosition < newPos && currentPosition < sourceCode.length()) {
        if (sourceCode[currentPosition] == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        currentPosition++;
    }
}

std::pair<size_t, size_t> CHTLUnifiedScanner::CalculateLineColumn(size_t offset) const {
    size_t line = 1;
    size_t column = 1;
    
    for (size_t i = 0; i < offset && i < sourceCode.length(); ++i) {
        if (sourceCode[i] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
    }
    
    return {line, column};
}

void CHTLUnifiedScanner::AddFragment(const std::string& content, FragmentType type,
                                    size_t startPos, size_t endPos) {
    auto [startLine, startCol] = CalculateLineColumn(startPos);
    auto [endLine, endCol] = CalculateLineColumn(endPos);
    
    fragments.emplace_back(content, type, startLine, startCol, endLine, endCol, startPos, endPos);
}

void CHTLUnifiedScanner::AddError(const std::string& message) {
    errors.push_back("错误: " + message);
}

void CHTLUnifiedScanner::AddWarning(const std::string& message) {
    warnings.push_back("警告: " + message);
}

void CHTLUnifiedScanner::MergeConsecutiveFragments() {
    if (fragments.size() <= 1) return;
    
    std::vector<CodeFragment> merged;
    merged.reserve(fragments.size());
    
    CodeFragment current = fragments[0];
    
    for (size_t i = 1; i < fragments.size(); ++i) {
        const CodeFragment& next = fragments[i];
        
        if (current.type == next.type && current.endOffset == next.startOffset) {
            // 合并片段
            current.content += next.content;
            current.endLine = next.endLine;
            current.endColumn = next.endColumn;
            current.endOffset = next.endOffset;
        } else {
            merged.push_back(current);
            current = next;
        }
    }
    
    merged.push_back(current);
    fragments = std::move(merged);
}

void CHTLUnifiedScanner::OptimizeFragmentDistribution() {
    // 可以在这里实现片段分布优化算法
    // 例如：重新平衡片段大小，优化缓存友好性等
}

bool CHTLUnifiedScanner::ValidateFragment(const CodeFragment& fragment) const {
    // 验证片段的基本完整性
    if (fragment.content.empty()) {
        return false;
    }
    
    if (fragment.startOffset >= fragment.endOffset) {
        return false;
    }
    
    if (fragment.endOffset > sourceCode.length()) {
        return false;
    }
    
    return true;
}

// 辅助函数实现

std::string FragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JS: return "JavaScript";
        case FragmentType::MIXED: return "MIXED";
        case FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

// ScannerFactory实现

std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::CreateDefault() {
    return std::make_unique<CHTLUnifiedScanner>();
}

std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::CreateHighPerformance() {
    ScannerConfig config;
    config.initialChunkSize = 2048;
    config.maxChunkSize = 16384;
    config.enableContextAnalysis = false;
    config.enableMinimalUnitSplit = false;
    
    return std::make_unique<CHTLUnifiedScanner>(config);
}

std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::CreatePrecision() {
    ScannerConfig config;
    config.initialChunkSize = 256;
    config.maxChunkSize = 1024;
    config.minChunkSize = 32;
    config.enableContextAnalysis = true;
    config.enableMinimalUnitSplit = true;
    config.enableAdaptiveExpansion = true;
    
    return std::make_unique<CHTLUnifiedScanner>(config);
}

std::unique_ptr<CHTLUnifiedScanner> ScannerFactory::CreateDebug() {
    ScannerConfig config;
    config.initialChunkSize = 128;
    config.maxChunkSize = 512;
    config.enableContextAnalysis = true;
    config.enableMinimalUnitSplit = true;
    config.enableAdaptiveExpansion = true;
    
    return std::make_unique<CHTLUnifiedScanner>(config);
}

} // namespace CHTL