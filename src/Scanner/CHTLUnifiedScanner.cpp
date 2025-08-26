#include "CHTLUnifiedScanner.h"
#include "../Common/StateMachine.h"
#include <algorithm>
#include <regex>
#include <iostream>

namespace CHTL {
namespace Scanner {

// CodeFragment实现
bool CodeFragment::isEmpty() const {
    return content.empty();
}

size_t CodeFragment::getLength() const {
    return content.length();
}

std::string CodeFragment::getLocationString() const {
    return file_path + ":" + std::to_string(start_line) + ":" + std::to_string(start_column);
}

// ScanResult实现
void ScanResult::addFragment(const CodeFragment& fragment) {
    fragments.push_back(fragment);
    success = true;
}

void ScanResult::addError(const std::string& error) {
    errors.push_back(error);
    success = false;
}

void ScanResult::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

// CHTLUnifiedScanner实现
CHTLUnifiedScanner::CHTLUnifiedScanner() 
    : strategy(ScanStrategy::CONTEXT_AWARE), 
      chunk_size(512), 
      min_unit_size(16),
      enable_cjmod_scanning(true),
      strict_boundary_detection(true),
      debug_mode(false),
      total_scanned_bytes(0),
      fragment_count(0) {
    
    // 初始化状态机
    state_machine = std::make_unique<Common::StateMachine>();
    state_machine->initializeStandardRules();
    state_machine->initializeCHTLRules();
    state_machine->initializeCHTLJSRules();
    
    // 初始化CJMOD扫描器
    cjmod_scanner = std::make_unique<CJMODScanner>(this);
    
    // 初始化模式
    initializePatterns();
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

void CHTLUnifiedScanner::initializePatterns() {
    // CHTL语法模式
    syntax_patterns[FragmentType::CHTL] = {
        R"(\b(html|head|body|div|span|p|h[1-6]|ul|ol|li|a|img|input|button|form|table|tr|td|th)\s*\{)",
        R"(\[Template\]|\[Custom\]|\[Origin\]|\[Configuration\]|\[Namespace\])",
        R"(\btext\s*\{)",
        R"(\bstyle\s*\{)",
        R"(\bscript\s*\{)"
    };
    
    // CHTL JS语法模式
    syntax_patterns[FragmentType::CHTL_JS] = {
        R"(\{\{[^}]+\}\})",                    // 增强选择器
        R"(\bvir\b)",                          // 虚对象
        R"(\blisten\s*\{)",                    // listen函数
        R"(\bdelegate\s*\{)",                  // delegate函数
        R"(\banimate\s*\{)",                   // animate函数
        R"(->)",                               // 箭头操作符
        R"(\biNeverAway\b|\bprintMylove\b)"    // 特殊函数
    };
    
    // CSS语法模式
    syntax_patterns[FragmentType::CSS] = {
        R"([.#]?[\w-]+\s*\{[^}]*\})",         // CSS规则
        R"(@media|@import|@keyframes|@font-face)", // CSS at-rules
        R"([\w-]+\s*:\s*[^;]+;)"              // CSS属性
    };
    
    // JavaScript语法模式
    syntax_patterns[FragmentType::JAVASCRIPT] = {
        R"(\bfunction\s+\w+\s*\([^)]*\)\s*\{)",
        R"(\bvar\s+\w+|\blet\s+\w+|\bconst\s+\w+)",
        R"(\bif\s*\(|\bfor\s*\(|\bwhile\s*\()",
        R"(\btry\s*\{|\bcatch\s*\(|\bfinally\s*\{)"
    };
    
    // 嵌套检测模式
    nesting_patterns = {
        {"open_brace", R"(\{)"},
        {"close_brace", R"(\})"},
        {"open_paren", R"(\()"},
        {"close_paren", R"(\))"},
        {"open_bracket", R"(\[)"},
        {"close_bracket", R"(\])"},
        {"enhanced_selector_open", R"(\{\{)"},
        {"enhanced_selector_close", R"(\}\})"}
    };
    
    // 边界检测模式
    boundary_patterns = {
        {"chtl_block_start", R"(\b\w+\s*\{)"},
        {"chtl_block_end", R"(\})"},
        {"script_block_start", R"(\bscript\s*\{)"},
        {"style_block_start", R"(\bstyle\s*\{)"},
        {"comment_start", R"(/\*|//|--)"},
        {"string_literal", R"("(?:[^"\\]|\\.)*"|'(?:[^'\\]|\\.)*')"}
    };
}

ScanResult CHTLUnifiedScanner::scanSource(const std::string& source, const std::string& file_path) {
    ScanResult result;
    result.file_path = file_path;
    
    if (source.empty()) {
        result.addWarning("Empty source code");
        return result;
    }
    
    // 重置扫描状态
    current_position = 0;
    current_line = 1;
    current_column = 1;
    state_machine->reset();
    
    try {
        // 创建扫描上下文
        ScanContext context;
        context.source = source;
        context.file_path = file_path;
        context.strategy = strategy;
        
        // 执行扫描
        std::vector<CodeFragment> fragments;
        switch (strategy) {
            case ScanStrategy::VARIABLE_LENGTH:
                fragments = scanWithVariableLength(context);
                break;
            case ScanStrategy::MINIMAL_UNIT:
                fragments = scanWithMinimalUnit(context);
                break;
            case ScanStrategy::CONTEXT_AWARE:
                fragments = scanWithContextAware(context);
                break;
            case ScanStrategy::HYBRID:
                fragments = scanWithHybrid(context);
                break;
        }
        
        // 后处理片段
        fragments = postProcessFragments(fragments, context);
        
        // 添加到结果
        for (const auto& fragment : fragments) {
            result.addFragment(fragment);
        }
        
        // 更新统计
        total_scanned_bytes += source.length();
        fragment_count += fragments.size();
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.addError("Scanner exception: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanWithVariableLength(const ScanContext& context) {
    std::vector<CodeFragment> fragments;
    const std::string& source = context.source;
    
    while (current_position < source.length()) {
        size_t chunk_start = current_position;
        size_t chunk_end = std::min(current_position + chunk_size, source.length());
        
        // 检查下一个片段是否形成完整的语法单元
        if (!isReasonableBoundary(source, chunk_end)) {
            // 向前扩展以找到合适的边界
            chunk_end = findNextSuitableBoundary(source, chunk_end);
        }
        
        // 提取片段
        std::string chunk_content = source.substr(chunk_start, chunk_end - chunk_start);
        
        // 检测片段类型
        FragmentType type = detectFragmentType(chunk_content, chunk_start);
        
        // 创建片段
        CodeFragment fragment;
        fragment.type = type;
        fragment.content = chunk_content;
        fragment.start_position = chunk_start;
        fragment.end_position = chunk_end;
        fragment.start_line = current_line;
        fragment.start_column = current_column;
        fragment.file_path = context.file_path;
        
        fragments.push_back(fragment);
        
        // 更新位置
        updatePosition(chunk_content);
        current_position = chunk_end;
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanWithMinimalUnit(const ScanContext& context) {
    std::vector<CodeFragment> fragments;
    const std::string& source = context.source;
    
    // 首先进行可变长度扫描
    auto initial_fragments = scanWithVariableLength(context);
    
    // 对CHTL和CHTL JS片段进行最小单元二次切割
    for (const auto& fragment : initial_fragments) {
        if (fragment.type == FragmentType::CHTL || fragment.type == FragmentType::CHTL_JS) {
            auto sub_fragments = splitIntoMinimalUnits(fragment);
            fragments.insert(fragments.end(), sub_fragments.begin(), sub_fragments.end());
        } else {
            fragments.push_back(fragment);
        }
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanWithContextAware(const ScanContext& context) {
    std::vector<CodeFragment> fragments;
    const std::string& source = context.source;
    
    // 使用状态机进行上下文感知扫描
    while (current_position < source.length()) {
        // 检测当前位置的语法上下文
        auto current_state = state_machine->getCurrentState();
        
        // 根据上下文确定扫描策略
        size_t fragment_end = findContextAwareBoundary(source, current_position, current_state);
        
        // 提取片段内容
        std::string content = source.substr(current_position, fragment_end - current_position);
        
        // 检测片段类型（结合状态机信息）
        FragmentType type = detectFragmentTypeWithContext(content, current_position, current_state);
        
        // 创建片段
        CodeFragment fragment = createFragment(content, current_position, fragment_end, type, context);
        fragments.push_back(fragment);
        
        // 更新状态机
        state_machine->autoTransition(content, current_position);
        
        // 更新位置
        updatePosition(content);
        current_position = fragment_end;
    }
    
    return fragments;
}

std::vector<CodeFragment> CHTLUnifiedScanner::scanWithHybrid(const ScanContext& context) {
    // 混合策略：结合可变长度和上下文感知
    auto context_fragments = scanWithContextAware(context);
    
    // 对需要进一步切割的片段使用最小单元策略
    std::vector<CodeFragment> refined_fragments;
    
    for (const auto& fragment : context_fragments) {
        if (needsFurtherSplitting(fragment)) {
            auto sub_fragments = splitIntoMinimalUnits(fragment);
            refined_fragments.insert(refined_fragments.end(), sub_fragments.begin(), sub_fragments.end());
        } else {
            refined_fragments.push_back(fragment);
        }
    }
    
    return refined_fragments;
}

FragmentType CHTLUnifiedScanner::detectFragmentType(const std::string& content, size_t position) {
    // 检查是否匹配各种语法模式
    for (const auto& [type, patterns] : syntax_patterns) {
        for (const auto& pattern : patterns) {
            if (std::regex_search(content, std::regex(pattern, std::regex_constants::icase))) {
                return type;
            }
        }
    }
    
    // 基于位置上下文的类型检测
    auto context_type = detectContextualType(content, position);
    if (context_type != FragmentType::UNKNOWN) {
        return context_type;
    }
    
    // 如果包含HTML标签，归类为HTML
    if (std::regex_search(content, std::regex(R"(<[^>]+>)"))) {
        return FragmentType::HTML;
    }
    
    // 默认为混合类型
    return FragmentType::MIXED;
}

FragmentType CHTLUnifiedScanner::detectFragmentTypeWithContext(const std::string& content, 
                                                               size_t position, 
                                                               Common::StateType state) {
    // 结合状态机状态的类型检测
    switch (state) {
        case Common::StateType::LOCAL_STYLE_BLOCK:
        case Common::StateType::GLOBAL_STYLE_BLOCK:
            return FragmentType::CSS;
            
        case Common::StateType::LOCAL_SCRIPT_BLOCK:
            // 检查是否包含CHTL JS语法
            if (containsCHTLJSSyntax(content)) {
                return FragmentType::CHTL_JS;
            }
            return FragmentType::JAVASCRIPT;
            
        case Common::StateType::GLOBAL_SCRIPT_BLOCK:
            return FragmentType::JAVASCRIPT;
            
        case Common::StateType::ENHANCED_SELECTOR:
        case Common::StateType::VIR_OBJECT:
        case Common::StateType::LISTEN_FUNCTION:
        case Common::StateType::DELEGATE_FUNCTION:
        case Common::StateType::ANIMATE_FUNCTION:
            return FragmentType::CHTL_JS;
            
        case Common::StateType::HTML_ELEMENT:
        case Common::StateType::TEXT_NODE:
            return FragmentType::CHTL;
            
        default:
            return detectFragmentType(content, position);
    }
}

bool CHTLUnifiedScanner::containsCHTLJSSyntax(const std::string& content) {
    // 检查CHTL JS特有语法
    const std::vector<std::string> chtljs_markers = {
        R"(\{\{[^}]+\}\})",                    // 增强选择器
        R"(->)",                               // 箭头操作符
        R"(\bvir\b)",                          // 虚对象
        R"(\blisten\s*\{)",                    // listen函数
        R"(\bdelegate\s*\{)",                  // delegate函数
        R"(\banimate\s*\{)",                   // animate函数
        R"(\biNeverAway\b|\bprintMylove\b)"    // 特殊函数
    };
    
    for (const auto& pattern : chtljs_markers) {
        if (std::regex_search(content, std::regex(pattern))) {
            return true;
        }
    }
    
    return false;
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitIntoMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    const std::string& content = fragment.content;
    
    if (fragment.type == FragmentType::CHTL_JS) {
        // 对CHTL JS进行特殊处理，例如 {{box}}->click 分割为 {{box}}-> 和 click
        units = splitCHTLJSMinimalUnits(fragment);
    } else if (fragment.type == FragmentType::CHTL) {
        // 对CHTL进行基于语法的最小单元分割
        units = splitCHTLMinimalUnits(fragment);
    } else {
        // 其他类型保持原样
        units.push_back(fragment);
    }
    
    return units;
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitCHTLJSMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    const std::string& content = fragment.content;
    
    // 使用正则表达式匹配CHTL JS的最小单元
    std::regex unit_pattern(R"((\{\{[^}]+\}\})|(\->)|(\w+)|([^\w\s]+))");
    std::sregex_iterator iter(content.begin(), content.end(), unit_pattern);
    std::sregex_iterator end;
    
    size_t current_pos = fragment.start_position;
    
    for (; iter != end; ++iter) {
        const auto& match = *iter;
        std::string unit_content = match.str();
        
        if (!unit_content.empty() && !std::all_of(unit_content.begin(), unit_content.end(), ::isspace)) {
            CodeFragment unit;
            unit.type = FragmentType::CHTL_JS;
            unit.content = unit_content;
            unit.start_position = current_pos;
            unit.end_position = current_pos + unit_content.length();
            unit.file_path = fragment.file_path;
            
            units.push_back(unit);
        }
        
        current_pos += unit_content.length();
    }
    
    return units;
}

std::vector<CodeFragment> CHTLUnifiedScanner::splitCHTLMinimalUnits(const CodeFragment& fragment) {
    std::vector<CodeFragment> units;
    const std::string& content = fragment.content;
    
    // CHTL的最小单元分割（基于语法结构）
    std::regex unit_pattern(R"((\[[\w]+\])|(\w+\s*\{)|(\})|([^{}\s]+))");
    std::sregex_iterator iter(content.begin(), content.end(), unit_pattern);
    std::sregex_iterator end;
    
    size_t current_pos = fragment.start_position;
    
    for (; iter != end; ++iter) {
        const auto& match = *iter;
        std::string unit_content = match.str();
        
        if (!unit_content.empty() && !std::all_of(unit_content.begin(), unit_content.end(), ::isspace)) {
            CodeFragment unit;
            unit.type = FragmentType::CHTL;
            unit.content = unit_content;
            unit.start_position = current_pos;
            unit.end_position = current_pos + unit_content.length();
            unit.file_path = fragment.file_path;
            
            units.push_back(unit);
        }
        
        current_pos += unit_content.length();
    }
    
    return units;
}

bool CHTLUnifiedScanner::isReasonableBoundary(const std::string& source, size_t position) {
    if (position >= source.length()) return true;
    
    // 检查是否在字符串或注释中间
    if (isInStringLiteral(source, position) || isInComment(source, position)) {
        return false;
    }
    
    // 检查是否在关键语法结构中间
    if (isInKeywordStructure(source, position)) {
        return false;
    }
    
    // 检查括号嵌套平衡
    if (!isBracketBalanced(source, position)) {
        return false;
    }
    
    return true;
}

size_t CHTLUnifiedScanner::findNextSuitableBoundary(const std::string& source, size_t start_pos) {
    size_t pos = start_pos;
    const size_t max_extension = chunk_size; // 最大扩展长度
    
    while (pos < source.length() && (pos - start_pos) < max_extension) {
        if (isReasonableBoundary(source, pos)) {
            return pos;
        }
        pos++;
    }
    
    // 如果找不到合适的边界，返回原始位置
    return std::min(start_pos + max_extension, source.length());
}

size_t CHTLUnifiedScanner::findContextAwareBoundary(const std::string& source, size_t start_pos, 
                                                   Common::StateType state) {
    size_t pos = start_pos;
    
    // 根据当前状态查找相应的结束标记
    switch (state) {
        case Common::StateType::LOCAL_STYLE_BLOCK:
        case Common::StateType::LOCAL_SCRIPT_BLOCK:
            return findBlockEnd(source, start_pos);
            
        case Common::StateType::ENHANCED_SELECTOR:
            return findEnhancedSelectorEnd(source, start_pos);
            
        default:
            return findNextSuitableBoundary(source, start_pos + chunk_size);
    }
}

CodeFragment CHTLUnifiedScanner::createFragment(const std::string& content, size_t start_pos, 
                                               size_t end_pos, FragmentType type, 
                                               const ScanContext& context) {
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = content;
    fragment.start_position = start_pos;
    fragment.end_position = end_pos;
    fragment.start_line = current_line;
    fragment.start_column = current_column;
    fragment.file_path = context.file_path;
    
    return fragment;
}

void CHTLUnifiedScanner::updatePosition(const std::string& content) {
    for (char c : content) {
        if (c == '\n') {
            current_line++;
            current_column = 1;
        } else {
            current_column++;
        }
    }
}

// CJMODScanner实现
CJMODScanner::CJMODScanner(CHTLUnifiedScanner* parent) : parent_scanner(parent) {}

CJMODScanner::~CJMODScanner() = default;

std::vector<CodeFragment> CJMODScanner::doublePointerScan(const std::string& source, 
                                                         const std::string& keyword) {
    std::vector<CodeFragment> fragments;
    
    size_t front_pointer = 0;
    size_t back_pointer = 0;
    bool collecting = false;
    
    while (front_pointer < source.length()) {
        // 前指针扫描关键字
        size_t keyword_pos = source.find(keyword, front_pointer);
        
        if (keyword_pos != std::string::npos) {
            if (!collecting) {
                // 开始收集
                back_pointer = keyword_pos;
                collecting = true;
            }
            
            // 找到语法片段的结束位置
            size_t fragment_end = findCJMODFragmentEnd(source, keyword_pos);
            
            // 创建片段
            CodeFragment fragment;
            fragment.type = FragmentType::CHTL_JS; // CJMOD通常是CHTL JS的扩展
            fragment.content = source.substr(back_pointer, fragment_end - back_pointer);
            fragment.start_position = back_pointer;
            fragment.end_position = fragment_end;
            
            fragments.push_back(fragment);
            
            // 移动指针
            front_pointer = fragment_end;
            collecting = false;
        } else {
            break;
        }
    }
    
    return fragments;
}

std::string CJMODScanner::prefixCapture(const std::string& source, size_t position, 
                                       const std::string& keyword) {
    // 前置截取：处理 arg ** arg2 这样的语法
    size_t keyword_pos = source.find(keyword, position);
    if (keyword_pos == std::string::npos) {
        return "";
    }
    
    // 向前查找可能的前缀
    size_t prefix_start = keyword_pos;
    while (prefix_start > 0 && std::isalnum(source[prefix_start - 1])) {
        prefix_start--;
    }
    
    if (prefix_start < keyword_pos) {
        return source.substr(prefix_start, keyword_pos - prefix_start);
    }
    
    return "";
}

} // namespace Scanner
} // namespace CHTL