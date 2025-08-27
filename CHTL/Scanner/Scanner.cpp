#include "Scanner.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

Scanner::Scanner() 
    : position(0), currentLine(1), currentColumn(1),
      debugMode(false), sliceSize(1024), state(ScannerState::NORMAL),
      bufferType(FragmentType::UNKNOWN), bufferStartLine(1), bufferStartColumn(1) {
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

} // namespace CHTL