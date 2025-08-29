#include "CHTLUnifiedScanner.h"
#include <regex>
#include <algorithm>
#include <unordered_map>
#include <stack>

namespace CHTL {

// CHTLUnifiedScanner实现类
class CHTLUnifiedScanner::Impl {
public:
    ScannerConfig config;
    std::unordered_map<FragmentType, std::function<bool(const std::string&, size_t)>> recognizers;
    
    // 内部状态
    size_t currentLine = 1;
    size_t currentColumn = 1;
    
    // CHTL关键字模式
    std::regex chtlKeywordPattern;
    std::regex chtljsPattern;
    std::regex cssPattern;
    std::regex jsPattern;
    
    Impl(const ScannerConfig& cfg) : config(cfg) {
        initializePatterns();
    }
    
    void initializePatterns() {
        // CHTL关键字模式
        chtlKeywordPattern = std::regex(
            R"(\b(text|style|script|div|span|body|html|head|\[Template\]|\[Custom\]|\[Origin\]|\[Import\]|\[Configuration\]|\[Namespace\]|@Style|@Element|@Var|inherit|delete|insert|use)\b)"
        );
        
        // CHTL JS模式 - 检测增强选择器和特殊语法
        chtljsPattern = std::regex(
            R"(\{\{[^}]+\}\}|->|&->|module\s*\{|listen\s*\{|delegate\s*\{|animate\s*\{|vir\s+\w+|iNeverAway\s*\{)"
        );
        
        // CSS模式 - 检测CSS语法
        cssPattern = std::regex(
            R"([\w-]+\s*:\s*[^;]+;|\.[\w-]+\s*\{|#[\w-]+\s*\{|@media|@keyframes)"
        );
        
        // JavaScript模式
        jsPattern = std::regex(
            R"(\b(function|const|let|var|if|else|for|while|return|class|extends|import|export)\b)"
        );
    }
    
    void updatePosition(char ch) {
        if (ch == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
    }
};

CHTLUnifiedScanner::CHTLUnifiedScanner(const ScannerConfig& config) 
    : pImpl(std::make_unique<Impl>(config)) {}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

std::vector<CodeFragment> CHTLUnifiedScanner::scan(const std::string& sourceCode) {
    std::vector<CodeFragment> fragments;
    size_t position = 0;
    size_t sliceSize = pImpl->config.initialSliceSize;
    
    pImpl->currentLine = 1;
    pImpl->currentColumn = 1;
    
    while (position < sourceCode.length()) {
        // 确定当前切片大小
        size_t currentSliceEnd = std::min(position + sliceSize, sourceCode.length());
        
        // 检查切片边界是否合理
        if (pImpl->config.enableVariableLengthSlicing && currentSliceEnd < sourceCode.length()) {
            // 检查是否需要扩展切片
            if (!isValidCutPoint(sourceCode, currentSliceEnd)) {
                currentSliceEnd = expandSliceForward(sourceCode, currentSliceEnd);
            }
        }
        
        // 提取切片内容
        std::string sliceContent = sourceCode.substr(position, currentSliceEnd - position);
        
        // 检测片段类型
        FragmentType type = detectFragmentType(sliceContent, 0);
        
        // 记录起始位置
        size_t startLine = pImpl->currentLine;
        size_t startColumn = pImpl->currentColumn;
        
        // 更新位置信息
        for (char ch : sliceContent) {
            pImpl->updatePosition(ch);
        }
        
        // 创建代码片段
        CodeFragment fragment(type, sliceContent, startLine, startColumn, 
                            pImpl->currentLine, pImpl->currentColumn);
        
        // 如果启用最小单元切片，对CHTL和CHTL JS片段进行二次切割
        if (pImpl->config.enableMinimalUnitSlicing && 
            (type == FragmentType::CHTL || type == FragmentType::CHTLJS)) {
            auto subFragments = performSecondarySlicing(fragment);
            fragments.insert(fragments.end(), subFragments.begin(), subFragments.end());
        } else {
            fragments.push_back(fragment);
        }
        
        position = currentSliceEnd;
    }
    
    return fragments;
}

bool CHTLUnifiedScanner::isValidCutPoint(const std::string& content, size_t position) {
    if (position >= content.length()) return true;
    
    // 检查是否在字符串字面量中
    bool inString = false;
    char stringChar = 0;
    for (size_t i = 0; i < position; i++) {
        if (!inString && (content[i] == '"' || content[i] == '\'')) {
            inString = true;
            stringChar = content[i];
        } else if (inString && content[i] == stringChar && 
                   (i == 0 || content[i-1] != '\\')) {
            inString = false;
        }
    }
    if (inString) return false;
    
    // 检查是否在注释中
    if (position > 0) {
        // 单行注释
        if (content[position-1] == '/' && position < content.length() && content[position] == '/') {
            return false;
        }
        // 多行注释
        if (position > 1 && content[position-2] == '/' && content[position-1] == '*') {
            size_t endComment = content.find("*/", position);
            return endComment == std::string::npos;
        }
    }
    
    // 检查是否在CHTL结构中
    std::stack<char> brackets;
    for (size_t i = 0; i < position; i++) {
        if (content[i] == '{') brackets.push('{');
        else if (content[i] == '}' && !brackets.empty()) brackets.pop();
    }
    if (!brackets.empty()) return false;
    
    // 检查是否在CHTL JS增强选择器中
    size_t doubleBraceStart = content.rfind("{{", position);
    if (doubleBraceStart != std::string::npos) {
        size_t doubleBraceEnd = content.find("}}", doubleBraceStart);
        if (doubleBraceEnd == std::string::npos || doubleBraceEnd >= position) {
            return false;
        }
    }
    
    return true;
}

FragmentType CHTLUnifiedScanner::detectFragmentType(const std::string& content, size_t position) {
    // 首先检查是否有自定义识别器
    for (const auto& [type, recognizer] : pImpl->recognizers) {
        if (recognizer(content, position)) {
            return type;
        }
    }
    
    // 使用正则表达式匹配检测类型
    // 优先级：CHTL JS > CHTL > CSS > JS
    
    // 检测CHTL JS特征
    if (std::regex_search(content, pImpl->chtljsPattern)) {
        return FragmentType::CHTLJS;
    }
    
    // 检测CHTL特征
    if (std::regex_search(content, pImpl->chtlKeywordPattern)) {
        return FragmentType::CHTL;
    }
    
    // 检测是否在style块中（CSS）
    size_t styleStart = content.find("style");
    if (styleStart != std::string::npos) {
        size_t braceStart = content.find("{", styleStart);
        if (braceStart != std::string::npos && position > braceStart) {
            return FragmentType::CSS;
        }
    }
    
    // 检测是否在script块中（JS）
    size_t scriptStart = content.find("script");
    if (scriptStart != std::string::npos) {
        size_t braceStart = content.find("{", scriptStart);
        if (braceStart != std::string::npos && position > braceStart) {
            // 再次检查是否包含CHTL JS语法
            std::string scriptContent = content.substr(braceStart);
            if (std::regex_search(scriptContent, pImpl->chtljsPattern)) {
                return FragmentType::CHTLJS;
            }
            return FragmentType::JS;
        }
    }
    
    // 检测CSS模式
    if (std::regex_search(content, pImpl->cssPattern)) {
        return FragmentType::CSS;
    }
    
    // 检测JS模式
    if (std::regex_search(content, pImpl->jsPattern)) {
        return FragmentType::JS;
    }
    
    return FragmentType::UNKNOWN;
}

std::vector<CodeFragment> CHTLUnifiedScanner::performSecondarySlicing(const CodeFragment& fragment) {
    std::vector<CodeFragment> subFragments;
    const std::string& content = fragment.content;
    size_t position = 0;
    
    // 对CHTL和CHTL JS片段进行最小单元切割
    while (position < content.length()) {
        size_t unitEnd = position;
        
        if (fragment.type == FragmentType::CHTLJS) {
            // 处理CHTL JS最小单元
            // 例如：{{box}}-> 应该被切割为 {{box}} 和 ->
            size_t doubleBraceStart = content.find("{{", position);
            if (doubleBraceStart != std::string::npos && doubleBraceStart == position) {
                size_t doubleBraceEnd = content.find("}}", doubleBraceStart);
                if (doubleBraceEnd != std::string::npos) {
                    unitEnd = doubleBraceEnd + 2;
                }
            }
            // 处理箭头操作符
            else if (position + 1 < content.length() && 
                     content[position] == '-' && content[position + 1] == '>') {
                unitEnd = position + 2;
            }
            // 处理其他CHTL JS语法单元
            else {
                // 查找下一个特殊标记
                size_t nextSpecial = content.find_first_of("{}->&", position + 1);
                unitEnd = (nextSpecial != std::string::npos) ? nextSpecial : content.length();
            }
        }
        else if (fragment.type == FragmentType::CHTL) {
            // 处理CHTL最小单元
            // 查找CHTL关键字或结构边界
            size_t nextBrace = content.find_first_of("{}", position + 1);
            size_t nextKeyword = std::string::npos;
            
            // 查找下一个CHTL关键字
            std::smatch match;
            std::string searchStr = content.substr(position);
            if (std::regex_search(searchStr, match, pImpl->chtlKeywordPattern)) {
                if (match.position() > 0) {
                    nextKeyword = position + match.position();
                }
            }
            
            if (nextBrace != std::string::npos && nextKeyword != std::string::npos) {
                unitEnd = std::min(nextBrace, nextKeyword);
            } else if (nextBrace != std::string::npos) {
                unitEnd = nextBrace;
            } else if (nextKeyword != std::string::npos) {
                unitEnd = nextKeyword;
            } else {
                unitEnd = content.length();
            }
        }
        
        // 确保至少前进一个字符
        if (unitEnd <= position) {
            unitEnd = position + 1;
        }
        
        // 创建子片段
        std::string unitContent = content.substr(position, unitEnd - position);
        if (!unitContent.empty()) {
            // 计算子片段的行列位置
            size_t subLine = fragment.startLine;
            size_t subColumn = fragment.startColumn;
            
            // 更新到当前位置
            for (size_t i = 0; i < position; i++) {
                if (content[i] == '\n') {
                    subLine++;
                    subColumn = 1;
                } else {
                    subColumn++;
                }
            }
            
            size_t endLine = subLine;
            size_t endColumn = subColumn;
            
            // 计算结束位置
            for (char ch : unitContent) {
                if (ch == '\n') {
                    endLine++;
                    endColumn = 1;
                } else {
                    endColumn++;
                }
            }
            
            subFragments.emplace_back(fragment.type, unitContent, 
                                    subLine, subColumn, endLine, endColumn);
        }
        
        position = unitEnd;
    }
    
    return subFragments;
}

size_t CHTLUnifiedScanner::expandSliceForward(const std::string& content, size_t position) {
    size_t maxExpand = std::min(position + pImpl->config.maxSliceSize, content.length());
    
    // 查找合适的切割点
    for (size_t i = position; i < maxExpand; i++) {
        if (isValidCutPoint(content, i)) {
            return i;
        }
    }
    
    return maxExpand;
}

size_t CHTLUnifiedScanner::expandSliceBackward(const std::string& content, size_t position) {
    if (position == 0) return 0;
    
    size_t minPosition = position > pImpl->config.maxSliceSize ? 
                         position - pImpl->config.maxSliceSize : 0;
    
    // 向后查找合适的切割点
    for (size_t i = position; i > minPosition; i--) {
        if (isValidCutPoint(content, i)) {
            return i;
        }
    }
    
    return minPosition;
}

void CHTLUnifiedScanner::setFragmentRecognizer(FragmentType type, 
                                               std::function<bool(const std::string&, size_t)> recognizer) {
    pImpl->recognizers[type] = recognizer;
}

void CHTLUnifiedScanner::reset() {
    pImpl->currentLine = 1;
    pImpl->currentColumn = 1;
    pImpl->recognizers.clear();
}

bool CHTLUnifiedScanner::isCHTLMinimalUnit(const std::string& content, size_t start, size_t end) {
    // 判断是否为CHTL最小单元
    std::string unit = content.substr(start, end - start);
    
    // CHTL最小单元包括：
    // 1. 单个关键字
    // 2. 属性定义（key: value;）
    // 3. 单个括号
    // 4. 注释
    
    std::smatch match;
    if (std::regex_match(unit, match, pImpl->chtlKeywordPattern)) {
        return true;
    }
    
    // 检查是否为属性定义
    if (std::regex_match(unit, std::regex(R"(^\s*[\w-]+\s*:\s*[^;]+;\s*$)"))) {
        return true;
    }
    
    // 检查是否为单个结构符号
    if (unit == "{" || unit == "}" || unit == ";" || unit == ":") {
        return true;
    }
    
    return false;
}

bool CHTLUnifiedScanner::isCHTLJSMinimalUnit(const std::string& content, size_t start, size_t end) {
    // 判断是否为CHTL JS最小单元
    std::string unit = content.substr(start, end - start);
    
    // CHTL JS最小单元包括：
    // 1. 增强选择器 {{...}}
    // 2. 箭头操作符 ->
    // 3. 事件绑定操作符 &->
    // 4. CHTL JS关键字
    
    // 检查增强选择器
    if (std::regex_match(unit, std::regex(R"(^\{\{[^}]+\}\}$)"))) {
        return true;
    }
    
    // 检查操作符
    if (unit == "->" || unit == "&->") {
        return true;
    }
    
    // 检查CHTL JS函数关键字
    if (unit == "module" || unit == "listen" || unit == "delegate" || 
        unit == "animate" || unit == "vir" || unit == "iNeverAway") {
        return true;
    }
    
    return false;
}

} // namespace CHTL