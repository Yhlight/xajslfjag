#include "Scanner.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <unordered_map>

namespace CHTL {

Scanner::Scanner(const std::string& source)
    : source_(source), current_pos_(0), current_line_(1), current_column_(1), strict_mode_(false) {
}

void Scanner::setSource(const std::string& source) {
    source_ = source;
    reset();
}

bool Scanner::scan() {
    reset();
    
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (isAtEnd()) break;
        
        size_t start_pos = current_pos_;
        size_t start_line = current_line_;
        size_t start_column = current_column_;
        
        // 检查是否是特殊块的开始
        if (peek() == 's' && source_.substr(current_pos_, 6) == "script") {
            // 检查是否为script块
            size_t temp_pos = current_pos_ + 6;
            while (temp_pos < source_.length() && (source_[temp_pos] == ' ' || source_[temp_pos] == '\t')) {
                temp_pos++;
            }
            if (temp_pos < source_.length() && source_[temp_pos] == '{') {
                CodeFragment fragment = scanScriptBlock();
                if (!fragment.content.empty()) {
                    fragments_.push_back(fragment);
                    continue;
                }
            }
        }
        
        if (peek() == 's' && source_.substr(current_pos_, 5) == "style") {
            // 检查是否为style块
            size_t temp_pos = current_pos_ + 5;
            while (temp_pos < source_.length() && (source_[temp_pos] == ' ' || source_[temp_pos] == '\t')) {
                temp_pos++;
            }
            if (temp_pos < source_.length() && source_[temp_pos] == '{') {
                CodeFragment fragment = scanStyleBlock();
                if (!fragment.content.empty()) {
                    fragments_.push_back(fragment);
                    continue;
                }
            }
        }
        
        // 扫描普通CHTL块
        CodeFragment fragment = scanCHTLBlock();
        if (!fragment.content.empty()) {
            fragments_.push_back(fragment);
        }
    }
    
    return true;
}

std::vector<CodeFragment> Scanner::getFragmentsByType(FragmentType type) const {
    std::vector<CodeFragment> result;
    for (const auto& fragment : fragments_) {
        if (fragment.type == type) {
            result.push_back(fragment);
        }
    }
    return result;
}

void Scanner::reset() {
    fragments_.clear();
    current_pos_ = 0;
    current_line_ = 1;
    current_column_ = 1;
}

std::string Scanner::validateFragments() const {
    std::ostringstream oss;
    
    // 检查是否有重叠的片段
    for (size_t i = 0; i < fragments_.size(); ++i) {
        for (size_t j = i + 1; j < fragments_.size(); ++j) {
            const auto& frag1 = fragments_[i];
            const auto& frag2 = fragments_[j];
            
            if (!(frag1.end_offset <= frag2.start_offset || frag2.end_offset <= frag1.start_offset)) {
                oss << "Fragment overlap detected between fragments " << i << " and " << j << "; ";
            }
        }
    }
    
    // 检查片段是否覆盖了整个源代码
    if (!fragments_.empty()) {
        size_t total_coverage = 0;
        for (const auto& fragment : fragments_) {
            total_coverage += fragment.end_offset - fragment.start_offset;
        }
        
        // 这里不要求100%覆盖，因为可能有空白字符和注释
    }
    
    std::string result = oss.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back(); // 移除最后的空格和分号
        result.pop_back();
    }
    
    return result;
}

std::string Scanner::generateReport() const {
    std::ostringstream oss;
    
    oss << "=== Scanner Report ===" << std::endl;
    oss << "Source Length: " << source_.length() << " characters" << std::endl;
    oss << "Total Fragments: " << fragments_.size() << std::endl;
    oss << std::endl;
    
    // 统计各类型片段数量
    std::unordered_map<FragmentType, int> type_counts;
    for (const auto& fragment : fragments_) {
        type_counts[fragment.type]++;
    }
    
    oss << "Fragment Type Statistics:" << std::endl;
    for (const auto& pair : type_counts) {
        oss << "  " << fragmentTypeToString(pair.first) << ": " << pair.second << std::endl;
    }
    oss << std::endl;
    
    // 详细片段信息
    oss << "Fragment Details:" << std::endl;
    for (size_t i = 0; i < fragments_.size(); ++i) {
        const auto& fragment = fragments_[i];
        oss << "  " << i << ": " << fragmentTypeToString(fragment.type)
            << " [" << fragment.start_line << ":" << fragment.start_column 
            << " - " << fragment.end_line << ":" << fragment.end_column << "]"
            << " (" << fragment.content.length() << " chars)" << std::endl;
    }
    
    return oss.str();
}

std::string Scanner::fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL_JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JS: return "JS";
        case FragmentType::HTML: return "HTML";
        case FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

bool Scanner::isAtEnd() const {
    return current_pos_ >= source_.length();
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return source_[current_pos_];
}

char Scanner::peekNext() const {
    if (current_pos_ + 1 >= source_.length()) return '\0';
    return source_[current_pos_ + 1];
}

char Scanner::advance() {
    if (isAtEnd()) return '\0';
    char c = source_[current_pos_++];
    updatePosition(c);
    return c;
}

bool Scanner::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_pos_] != expected) return false;
    current_pos_++;
    updatePosition(expected);
    return true;
}

void Scanner::skipWhitespace() {
    while (!isAtEnd() && (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')) {
        advance();
    }
}

CodeFragment Scanner::scanCHTLBlock() {
    size_t start_pos = current_pos_;
    size_t start_line = current_line_;
    size_t start_column = current_column_;
    
    std::string content;
    
    // 扫描到下一个特殊块或文件结束
    while (!isAtEnd()) {
        // 检查是否遇到script或style块
        if (peek() == 's') {
            if (source_.substr(current_pos_, 6) == "script" || 
                source_.substr(current_pos_, 5) == "style") {
                // 检查是否真的是块开始
                size_t temp_pos = current_pos_ + (source_.substr(current_pos_, 6) == "script" ? 6 : 5);
                while (temp_pos < source_.length() && (source_[temp_pos] == ' ' || source_[temp_pos] == '\t')) {
                    temp_pos++;
                }
                if (temp_pos < source_.length() && source_[temp_pos] == '{') {
                    break; // 遇到了特殊块，停止扫描
                }
            }
        }
        
        content += advance();
    }
    
    // 检测片段类型
    FragmentType type = detectFragmentType(content);
    
    return createFragment(type, content, start_pos);
}

CodeFragment Scanner::scanStyleBlock() {
    size_t start_pos = current_pos_;
    
    // 跳过"style"关键字
    for (int i = 0; i < 5; ++i) {
        if (!isAtEnd()) advance();
    }
    
    // 跳过空白字符到'{'
    skipWhitespace();
    
    if (peek() != '{') {
        // 不是有效的style块，回退
        current_pos_ = start_pos;
        return scanCHTLBlock();
    }
    
    // 消费'{'
    advance();
    
    // 扫描到匹配的'}'
    std::string content = scanToMatchingBrace('{', '}');
    
    return createFragment(FragmentType::CSS, content, start_pos);
}

CodeFragment Scanner::scanScriptBlock() {
    size_t start_pos = current_pos_;
    
    // 跳过"script"关键字
    for (int i = 0; i < 6; ++i) {
        if (!isAtEnd()) advance();
    }
    
    // 跳过空白字符到'{'
    skipWhitespace();
    
    if (peek() != '{') {
        // 不是有效的script块，回退
        current_pos_ = start_pos;
        return scanCHTLBlock();
    }
    
    // 消费'{'
    advance();
    
    // 扫描到匹配的'}'
    std::string content = scanToMatchingBrace('{', '}');
    
    // 检测是CHTL JS还是普通JS
    FragmentType type = FragmentType::CHTL_JS; // 默认认为是CHTL JS
    
    // 如果包含CHTL JS特有语法，确认为CHTL JS
    if (content.find("{{") != std::string::npos || 
        content.find("}}") != std::string::npos ||
        content.find("listen") != std::string::npos ||
        content.find("delegate") != std::string::npos ||
        content.find("animate") != std::string::npos ||
        content.find("vir") != std::string::npos ||
        content.find("&->") != std::string::npos) {
        type = FragmentType::CHTL_JS;
    } else {
        type = FragmentType::JS;
    }
    
    return createFragment(type, content, start_pos);
}

std::string Scanner::scanToMatchingBrace(char open_brace, char close_brace) {
    std::string content;
    int brace_count = 1; // 已经消费了一个开始括号
    
    while (!isAtEnd() && brace_count > 0) {
        char c = peek();
        
        if (c == open_brace) {
            brace_count++;
        } else if (c == close_brace) {
            brace_count--;
        }
        
        if (brace_count > 0) { // 不包含最后的结束括号
            content += advance();
        } else {
            advance(); // 消费结束括号但不包含在内容中
        }
    }
    
    return content;
}

FragmentType Scanner::detectFragmentType(const std::string& content) const {
    // 简单的类型检测逻辑
    if (content.empty()) {
        return FragmentType::UNKNOWN;
    }
    
    // 检查是否包含HTML标签
    if (content.find('<') != std::string::npos && content.find('>') != std::string::npos) {
        return FragmentType::HTML;
    }
    
    // 检查CHTL特有语法
    if (content.find("[Template]") != std::string::npos ||
        content.find("[Custom]") != std::string::npos ||
        content.find("[Origin]") != std::string::npos ||
        content.find("[Import]") != std::string::npos ||
        content.find("[Configuration]") != std::string::npos ||
        content.find("[Namespace]") != std::string::npos ||
        content.find("text {") != std::string::npos) {
        return FragmentType::CHTL;
    }
    
    // 默认为CHTL
    return FragmentType::CHTL;
}

void Scanner::updatePosition(char c) {
    if (c == '\n') {
        current_line_++;
        current_column_ = 1;
    } else {
        current_column_++;
    }
}

CodeFragment Scanner::createFragment(FragmentType type, const std::string& content, size_t start_pos) {
    CodeFragment fragment(type);
    fragment.content = content;
    fragment.start_offset = start_pos;
    fragment.end_offset = current_pos_;
    fragment.start_line = current_line_;
    fragment.start_column = current_column_;
    // 这里简化处理，实际应该计算准确的结束位置
    fragment.end_line = current_line_;
    fragment.end_column = current_column_;
    
    return fragment;
}

} // namespace CHTL