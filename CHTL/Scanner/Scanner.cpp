#include "Scanner.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

Scanner::Scanner() 
    : position(0), currentLine(1), currentColumn(1),
      debugMode(false), sliceSize(1024), state(ScannerState::NORMAL),
      bufferType(FragmentType::UNKNOWN), bufferStartLine(1), bufferStartColumn(1),
      frontPointer(0), backPointer(0), collectMode(false) {
}

Scanner::~Scanner() = default;

void Scanner::SetSource(const std::string& source) {
    this->source = source;
    position = 0;
    currentLine = 1;
    currentColumn = 1;
    state = ScannerState::NORMAL;
    contextStack.clear();
    buffer.clear();
    errors.clear();
}

CodeFragment Scanner::NextFragment() {
    buffer.clear();
    bufferStartLine = currentLine;
    bufferStartColumn = currentColumn;
    
    while (!IsEOF()) {
        switch (state) {
            case ScannerState::NORMAL:
                if (ScanCHTL()) {
                    return CreateFragment(FragmentType::CHTL, buffer);
                }
                break;
                
            case ScannerState::IN_STYLE:
                if (ScanStyle()) {
                    return CreateFragment(FragmentType::CSS, buffer);
                }
                break;
                
            case ScannerState::IN_SCRIPT:
                if (ScanScript()) {
                    // 判断是否包含CHTL JS特征
                    if (ContainsCHTLJSFeatures(buffer)) {
                        // 对CHTL JS代码进行精准切割
                        auto fragments = SplitCHTLJSFragment(buffer);
                        if (!fragments.empty()) {
                            // 返回第一个片段，其余的放入缓冲区
                            // TODO: 实现片段缓冲机制
                            return fragments[0];
                        }
                        return CreateFragment(FragmentType::CHTLJS, buffer);
                    }
                    return CreateFragment(FragmentType::JS, buffer);
                }
                break;
                
            case ScannerState::IN_COMMENT:
                if (ScanComment()) {
                    // 注释不生成片段，继续扫描
                    buffer.clear();
                    bufferStartLine = currentLine;
                    bufferStartColumn = currentColumn;
                }
                break;
                
            case ScannerState::IN_ORIGIN:
                if (ScanOrigin()) {
                    // 原始嵌入根据类型决定片段类型
                    return CreateFragment(bufferType, buffer);
                }
                break;
                
            default:
                Advance();
                break;
        }
    }
    
    // 处理最后的缓冲区内容
    if (!buffer.empty()) {
        return CreateFragment(bufferType, buffer);
    }
    
    return CodeFragment{FragmentType::UNKNOWN, "", 0, 0, 0, 0, ""};
}

bool Scanner::IsEOF() const {
    return position >= source.length();
}

char Scanner::CurrentChar() const {
    if (IsEOF()) return '\0';
    return source[position];
}

char Scanner::PeekChar(int offset) const {
    size_t pos = position + offset;
    if (pos >= source.length()) return '\0';
    return source[pos];
}

void Scanner::Advance() {
    if (!IsEOF()) {
        if (CurrentChar() == '\n') {
            currentLine++;
            currentColumn = 1;
        } else {
            currentColumn++;
        }
        position++;
    }
}

void Scanner::AdvanceN(int n) {
    for (int i = 0; i < n; ++i) {
        Advance();
    }
}

bool Scanner::ScanCHTL() {
    // 扫描CHTL代码直到遇到style、script或其他特殊块
    while (!IsEOF()) {
        if (IsStartOfStyle()) {
            state = ScannerState::IN_STYLE;
            return !buffer.empty();
        }
        
        if (IsStartOfScript()) {
            state = ScannerState::IN_SCRIPT;
            return !buffer.empty();
        }
        
        // 检查注释
        if (CurrentChar() == '/' && (PeekChar() == '/' || PeekChar() == '*')) {
            if (!buffer.empty()) {
                return true;
            }
            state = ScannerState::IN_COMMENT;
            return false;
        }
        
        if (CurrentChar() == '-' && PeekChar() == '-') {
            if (!buffer.empty()) {
                return true;
            }
            state = ScannerState::IN_COMMENT;
            return false;
        }
        
        // 处理字符串
        if (CurrentChar() == '"' || CurrentChar() == '\'') {
            char quote = CurrentChar();
            buffer += CurrentChar();
            Advance();
            ScanString(quote);
            continue;
        }
        
        buffer += CurrentChar();
        Advance();
    }
    
    return !buffer.empty();
}

bool Scanner::ScanStyle() {
    // 查找style块的开始
    std::string styleStart = "style";
    size_t startPos = source.find(styleStart, position);
    
    if (startPos == std::string::npos) {
        state = ScannerState::NORMAL;
        return false;
    }
    
    // 移动到style后
    while (position < startPos + styleStart.length()) {
        Advance();
    }
    
    // 跳过空白字符
    while (!IsEOF() && std::isspace(CurrentChar())) {
        Advance();
    }
    
    // 必须是 {
    if (CurrentChar() != '{') {
        state = ScannerState::NORMAL;
        return false;
    }
    
    Advance(); // 跳过 {
    
    // 扫描到对应的 }
    int braceCount = 1;
    buffer.clear();
    
    while (!IsEOF() && braceCount > 0) {
        if (CurrentChar() == '{') {
            braceCount++;
        } else if (CurrentChar() == '}') {
            braceCount--;
            if (braceCount == 0) {
                Advance(); // 跳过最后的 }
                break;
            }
        } else if (CurrentChar() == '"' || CurrentChar() == '\'') {
            // 处理字符串中的大括号
            char quote = CurrentChar();
            buffer += CurrentChar();
            Advance();
            ScanString(quote);
            continue;
        }
        
        buffer += CurrentChar();
        Advance();
    }
    
    state = ScannerState::NORMAL;
    return true;
}

bool Scanner::ScanScript() {
    // 类似ScanStyle的实现
    std::string scriptStart = "script";
    size_t startPos = source.find(scriptStart, position);
    
    if (startPos == std::string::npos) {
        state = ScannerState::NORMAL;
        return false;
    }
    
    // 移动到script后
    while (position < startPos + scriptStart.length()) {
        Advance();
    }
    
    // 跳过空白字符
    while (!IsEOF() && std::isspace(CurrentChar())) {
        Advance();
    }
    
    // 必须是 {
    if (CurrentChar() != '{') {
        state = ScannerState::NORMAL;
        return false;
    }
    
    Advance(); // 跳过 {
    
    // 扫描到对应的 }
    int braceCount = 1;
    buffer.clear();
    
    while (!IsEOF() && braceCount > 0) {
        if (CurrentChar() == '{') {
            braceCount++;
        } else if (CurrentChar() == '}') {
            braceCount--;
            if (braceCount == 0) {
                Advance(); // 跳过最后的 }
                break;
            }
        } else if (CurrentChar() == '"' || CurrentChar() == '\'') {
            // 处理字符串中的大括号
            char quote = CurrentChar();
            buffer += CurrentChar();
            Advance();
            ScanString(quote);
            continue;
        }
        
        buffer += CurrentChar();
        Advance();
    }
    
    state = ScannerState::NORMAL;
    return true;
}

bool Scanner::ScanComment() {
    if (CurrentChar() == '/' && PeekChar() == '/') {
        // 单行注释
        AdvanceN(2);
        while (!IsEOF() && CurrentChar() != '\n') {
            buffer += CurrentChar();
            Advance();
        }
        if (CurrentChar() == '\n') {
            Advance();
        }
    } else if (CurrentChar() == '/' && PeekChar() == '*') {
        // 多行注释
        AdvanceN(2);
        while (!IsEOF() && !(CurrentChar() == '*' && PeekChar() == '/')) {
            buffer += CurrentChar();
            Advance();
        }
        if (CurrentChar() == '*' && PeekChar() == '/') {
            AdvanceN(2);
        }
    } else if (CurrentChar() == '-' && PeekChar() == '-') {
        // 生成器注释
        AdvanceN(2);
        while (!IsEOF() && CurrentChar() != '\n') {
            buffer += CurrentChar();
            Advance();
        }
        if (CurrentChar() == '\n') {
            Advance();
        }
    }
    
    state = ScannerState::NORMAL;
    return true;
}

bool Scanner::ScanString(char quote) {
    while (!IsEOF() && CurrentChar() != quote) {
        if (CurrentChar() == '\\') {
            buffer += CurrentChar();
            Advance();
            if (!IsEOF()) {
                buffer += CurrentChar();
                Advance();
            }
        } else {
            buffer += CurrentChar();
            Advance();
        }
    }
    
    if (CurrentChar() == quote) {
        buffer += CurrentChar();
        Advance();
    }
    
    return true;
}

bool Scanner::ScanOrigin() {
    // 扫描原始嵌入内容
    // TODO: 实现原始嵌入扫描逻辑
    state = ScannerState::NORMAL;
    return true;
}

bool Scanner::IsStartOfStyle() const {
    // 检查是否是style块的开始
    if (position + 5 >= source.length()) return false;
    
    std::string ahead = source.substr(position, 5);
    if (ahead == "style") {
        // 检查前面是否是标识符字符
        if (position > 0 && (std::isalnum(source[position - 1]) || source[position - 1] == '_')) {
            return false;
        }
        
        // 检查后面是否跟着空白字符或{
        size_t checkPos = position + 5;
        while (checkPos < source.length() && std::isspace(source[checkPos])) {
            checkPos++;
        }
        
        return checkPos < source.length() && source[checkPos] == '{';
    }
    
    return false;
}

bool Scanner::IsStartOfScript() const {
    // 检查是否是script块的开始
    if (position + 6 >= source.length()) return false;
    
    std::string ahead = source.substr(position, 6);
    if (ahead == "script") {
        // 检查前面是否是标识符字符
        if (position > 0 && (std::isalnum(source[position - 1]) || source[position - 1] == '_')) {
            return false;
        }
        
        // 检查后面是否跟着空白字符或{
        size_t checkPos = position + 6;
        while (checkPos < source.length() && std::isspace(source[checkPos])) {
            checkPos++;
        }
        
        return checkPos < source.length() && source[checkPos] == '{';
    }
    
    return false;
}

bool Scanner::ContainsCHTLJSFeatures(const std::string& content) const {
    // 检查是否包含CHTL JS特征
    // 1. 增强选择器 {{}}
    if (content.find("{{") != std::string::npos && content.find("}}") != std::string::npos) {
        return true;
    }
    
    // 2. CHTL JS操作符 ->
    if (content.find("->") != std::string::npos) {
        // 需要确保不是在字符串中
        // TODO: 更精确的检测
        return true;
    }
    
    // 3. CHTL JS函数（listen, delegate, animate等）
    std::regex chtljsFunctions(R"(\b(listen|delegate|animate|vir)\s*\{)");
    if (std::regex_search(content, chtljsFunctions)) {
        return true;
    }
    
    // 4. 事件绑定操作符 &->
    if (content.find("&->") != std::string::npos) {
        return true;
    }
    
    return false;
}

CodeFragment Scanner::CreateFragment(FragmentType type, const std::string& content) {
    CodeFragment fragment;
    fragment.type = type;
    fragment.content = content;
    fragment.startLine = bufferStartLine;
    fragment.startColumn = bufferStartColumn;
    fragment.endLine = currentLine;
    fragment.endColumn = currentColumn;
    fragment.context = GetCurrentContext();
    
    if (debugMode) {
        std::cout << "创建片段: 类型=" << static_cast<int>(type)
                  << ", 行=" << fragment.startLine << "-" << fragment.endLine
                  << ", 内容长度=" << content.length() << "\n";
    }
    
    return fragment;
}

void Scanner::PushContext(const std::string& type, const std::string& name) {
    ContextItem item;
    item.type = type;
    item.name = name;
    item.startLine = currentLine;
    item.startColumn = currentColumn;
    contextStack.push_back(item);
}

void Scanner::PopContext() {
    if (!contextStack.empty()) {
        contextStack.pop_back();
    }
}

std::string Scanner::GetCurrentContext() const {
    if (contextStack.empty()) {
        return "";
    }
    
    // 返回最内层的上下文
    const auto& context = contextStack.back();
    
    // 如果是局部样式或脚本，添加local:前缀
    if ((context.type == "style" || context.type == "script") && contextStack.size() > 1) {
        return "local:" + context.type;
    }
    
    return context.type;
}

void Scanner::ReportError(const std::string& message) {
    std::stringstream ss;
    ss << "Scanner错误 [" << currentLine << ":" << currentColumn << "]: " << message;
    errors.push_back(ss.str());
}

// 双指针扫描机制实现
void Scanner::InitDoublePointer() {
    frontPointer = position;
    backPointer = position;
    collectMode = false;
}

bool Scanner::ScanWithDoublePointer(const std::string& keyword) {
    InitDoublePointer();
    
    while (frontPointer < source.length()) {
        // 检查是否找到关键字
        if (frontPointer + keyword.length() <= source.length()) {
            std::string potential = source.substr(frontPointer, keyword.length());
            if (potential == keyword) {
                // 找到关键字，开始收集
                collectMode = true;
                return true;
            }
        }
        
        // 移动前指针
        frontPointer++;
        
        // 如果没有在收集模式，同步移动后指针
        if (!collectMode && frontPointer - backPointer > sliceSize) {
            backPointer = frontPointer - sliceSize;
        }
    }
    
    return false;
}

std::string Scanner::CollectFragment(size_t start, size_t end) {
    if (start >= source.length() || end > source.length() || start >= end) {
        return "";
    }
    return source.substr(start, end - start);
}

// 前置截取机制实现
bool Scanner::NeedPreCapture(const std::string& content, size_t pos) const {
    // 检查是否需要前置截取
    // 例如：对于 "arg ** arg2"，当扫描到 ** 时需要前置截取 arg
    if (pos >= 2 && pos < content.length() - 1) {
        // 检查CHTL JS操作符
        if (content.substr(pos, 2) == "->") {
            return pos > 0 && content[pos - 1] != ' ';
        }
        
        // 检查其他需要前置截取的模式
        if (content.substr(pos, 2) == "**") {
            return pos > 0 && std::isalnum(content[pos - 1]);
        }
    }
    
    return false;
}

std::string Scanner::PreCaptureFragment(const std::string& content, size_t& pos) {
    // 向前截取直到遇到分隔符
    size_t start = pos;
    while (start > 0 && !std::isspace(content[start - 1]) && 
           content[start - 1] != '{' && content[start - 1] != '}' &&
           content[start - 1] != ';' && content[start - 1] != ',') {
        start--;
    }
    
    std::string captured = content.substr(start, pos - start);
    pos = start;
    return captured;
}

// CHTL JS精准切割实现
std::vector<CodeFragment> Scanner::SplitCHTLJSFragment(const std::string& content) {
    std::vector<CodeFragment> fragments;
    size_t pos = 0;
    size_t fragmentStart = 0;
    
    while (pos < content.length()) {
        // 检查是否是CHTL JS边界
        if (IsCHTLJSBoundary(content, pos)) {
            // 保存当前片段
            if (pos > fragmentStart) {
                CodeFragment fragment;
                fragment.type = FragmentType::CHTLJS;
                fragment.content = content.substr(fragmentStart, pos - fragmentStart);
                fragment.startLine = bufferStartLine;
                fragment.startColumn = bufferStartColumn;
                fragment.context = GetCurrentContext();
                fragments.push_back(fragment);
            }
            
            // 处理特殊边界（如 -> 操作符）
            if (pos + 2 <= content.length() && content.substr(pos, 2) == "->") {
                CodeFragment fragment;
                fragment.type = FragmentType::CHTLJS;
                fragment.content = "->";
                fragment.startLine = bufferStartLine;
                fragment.startColumn = bufferStartColumn + (pos - fragmentStart);
                fragment.context = GetCurrentContext();
                fragments.push_back(fragment);
                pos += 2;
                fragmentStart = pos;
            } else {
                pos++;
            }
        } else {
            pos++;
        }
    }
    
    // 保存最后的片段
    if (fragmentStart < content.length()) {
        CodeFragment fragment;
        fragment.type = FragmentType::CHTLJS;
        fragment.content = content.substr(fragmentStart);
        fragment.startLine = bufferStartLine;
        fragment.startColumn = bufferStartColumn + fragmentStart;
        fragment.context = GetCurrentContext();
        fragments.push_back(fragment);
    }
    
    return fragments;
}

bool Scanner::IsCHTLJSBoundary(const std::string& content, size_t pos) const {
    // 检查是否是CHTL JS的边界点
    
    // 1. -> 操作符前后
    if (pos + 2 <= content.length() && content.substr(pos, 2) == "->") {
        return true;
    }
    
    // 2. {{ 或 }} 边界
    if (pos + 2 <= content.length()) {
        std::string twoChars = content.substr(pos, 2);
        if (twoChars == "{{" || twoChars == "}}") {
            return true;
        }
    }
    
    // 3. &-> 操作符
    if (pos + 3 <= content.length() && content.substr(pos, 3) == "&->") {
        return true;
    }
    
    // 4. 在某些上下文中的特定分隔符
    if (pos < content.length()) {
        char c = content[pos];
        // 在CHTL JS函数调用中的边界
        if (GetCurrentContext().find("chtljs_function") != std::string::npos) {
            return c == '{' || c == '}' || c == ':' || c == ',';
        }
    }
    
    return false;
}

// 局部样式块处理实现
bool Scanner::IsLocalStyleBlock() const {
    // 检查是否在元素内部的style块
    return state == ScannerState::IN_STYLE && contextStack.size() > 1;
}

void Scanner::MarkAsLocalStyle() {
    if (!contextStack.empty()) {
        contextStack.back().type = "local:style";
    }
}

std::string Scanner::ExtractSelectorFromStyle(const std::string& content) const {
    // 提取样式内容中的选择器
    // 查找类选择器 .classname
    std::regex classRegex(R"(\.([a-zA-Z_][\w-]*))");
    std::smatch match;
    
    if (std::regex_search(content, match, classRegex)) {
        return match[1].str();  // 返回类名（不含点）
    }
    
    // 查找ID选择器 #id
    std::regex idRegex(R"(#([a-zA-Z_][\w-]*))");
    if (std::regex_search(content, match, idRegex)) {
        return match[1].str();  // 返回ID（不含#）
    }
    
    return "";
}

} // namespace CHTL