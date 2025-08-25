#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include "UnifiedScanner/PatternMatcher.h"
#include "CHTL/Core/Config.h"
#include "CJMOD/CHTLJSFunction.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source, const Config& config)
    : m_Source(source), m_Config(config) {
}

std::vector<CodeSlice> CHTLUnifiedScanner::Scan() {
    m_Slices.clear();
    m_Current = 0;
    m_Line = 1;
    m_State = ScanState::CHTL;
    
    while (!IsAtEnd()) {
        m_Start = m_Current;
        ScanToken();
    }
    
    // 如果还有未处理的内容，添加最后一个切片
    if (m_Start < m_Source.length()) {
        AddSlice(SliceType::CHTL, m_Start, m_Source.length());
    }
    
    // 对每个切片进行二次处理
    for (auto& slice : m_Slices) {
        PerformSecondarySlicing(slice);
    }
    
    return m_Slices;
}

void CHTLUnifiedScanner::ScanToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) return;
    
    // 检查是否进入特殊块
    if (m_State == ScanState::CHTL) {
        // 检查全局<style>标签
        if (Match("<style")) {
            while (!IsAtEnd() && Peek() != '>') Advance();
            if (!IsAtEnd()) Advance(); // 跳过 '>'
            m_State = ScanState::GlobalStyle;
            AddSlice(SliceType::CHTL, m_Start, m_Current);
            m_Start = m_Current;
            return;
        }
        
        // 检查全局<script>标签
        if (Match("<script")) {
            while (!IsAtEnd() && Peek() != '>') Advance();
            if (!IsAtEnd()) Advance(); // 跳过 '>'
            m_State = ScanState::GlobalScript;
            AddSlice(SliceType::CHTL, m_Start, m_Current);
            m_Start = m_Current;
            return;
        }
        
        // 检查局部style块
        if (Match("style") && Peek() == '{') {
            m_State = ScanState::LocalStyle;
            ScanLocalStyleBlock();
            return;
        }
        
        // 检查局部script块
        if (Match("script") && Peek() == '{') {
            m_State = ScanState::LocalScript;
            ScanLocalScriptBlock();
            return;
        }
    }
    
    // 处理全局样式块
    if (m_State == ScanState::GlobalStyle) {
        if (Match("</style>")) {
            AddSlice(SliceType::CSS, m_Start, m_Current - 8); // 不包含</style>
            m_State = ScanState::CHTL;
            m_Start = m_Current;
            return;
        }
    }
    
    // 处理全局脚本块
    if (m_State == ScanState::GlobalScript) {
        if (Match("</script>")) {
            AddSlice(SliceType::JS, m_Start, m_Current - 9); // 不包含</script>
            m_State = ScanState::CHTL;
            m_Start = m_Current;
            return;
        }
    }
    
    // 默认前进
    Advance();
}

void CHTLUnifiedScanner::ScanLocalStyleBlock() {
    // style { ... } 形式的局部样式块
    Advance(); // 跳过 '{'
    
    int braceCount = 1;
    size_t blockStart = m_Current;
    
    while (!IsAtEnd() && braceCount > 0) {
        if (Peek() == '{') {
            braceCount++;
        } else if (Peek() == '}') {
            braceCount--;
            if (braceCount == 0) {
                // 找到局部样式块的结束
                AddSlice(SliceType::CHTL, m_Start, m_Current + 1);
                Advance(); // 跳过 '}'
                m_State = ScanState::CHTL;
                m_Start = m_Current;
                return;
            }
        }
        Advance();
    }
}

void CHTLUnifiedScanner::ScanLocalScriptBlock() {
    // script { ... } 形式的局部脚本块
    Advance(); // 跳过 '{'
    
    int braceCount = 1;
    size_t blockStart = m_Current;
    std::string scriptContent;
    
    while (!IsAtEnd() && braceCount > 0) {
        if (Peek() == '{') {
            braceCount++;
        } else if (Peek() == '}') {
            braceCount--;
            if (braceCount == 0) {
                // 提取script内容
                scriptContent = m_Source.substr(blockStart, m_Current - blockStart);
                break;
            }
        }
        Advance();
    }
    
    // 先添加script块的开始标记
    CodeSlice scriptStart;
    scriptStart.Content = "script{";
    scriptStart.Type = SliceType::CHTL;
    scriptStart.StartPosition = m_Start;
    scriptStart.EndPosition = blockStart;
    scriptStart.LineNumber = m_Line;
    m_Slices.push_back(scriptStart);
    
    // 处理script块中的CHTL JS语法
    auto processedSlices = ProcessScriptBlock(scriptContent);
    
    // 将处理后的切片添加到结果中
    for (const auto& slice : processedSlices) {
        m_Slices.push_back(slice);
    }
    
    // 添加script块的结束标记
    if (!IsAtEnd()) {
        CodeSlice scriptEnd;
        scriptEnd.Content = "}";
        scriptEnd.Type = SliceType::CHTL;
        scriptEnd.StartPosition = m_Current;
        scriptEnd.EndPosition = m_Current + 1;
        scriptEnd.LineNumber = m_Line;
        m_Slices.push_back(scriptEnd);
        Advance(); // 跳过 '}'
    }
    
    m_State = ScanState::CHTL;
    m_Start = m_Current;
}

std::vector<CodeSlice> CHTLUnifiedScanner::ProcessScriptBlock(const std::string& content) {
    std::vector<CodeSlice> slices;
    size_t pos = 0;
    size_t lastPos = 0;
    
    // 扫描CHTL JS特有语法，如 {{selector}}
    while (pos < content.length()) {
        // 查找增强选择器 {{...}}
        size_t selectorStart = content.find("{{", pos);
        if (selectorStart != std::string::npos) {
            size_t selectorEnd = content.find("}}", selectorStart + 2);
            if (selectorEnd != std::string::npos) {
                // 添加{{之前的JS代码
                if (selectorStart > lastPos) {
                    CodeSlice jsSlice;
                    jsSlice.Content = content.substr(lastPos, selectorStart - lastPos);
                    jsSlice.Type = SliceType::JS;
                    jsSlice.StartPosition = lastPos;
                    jsSlice.EndPosition = selectorStart;
                    jsSlice.LineNumber = m_Line;
                    slices.push_back(jsSlice);
                }
                
                // 添加CHTL JS片段
                CodeSlice chtlJsSlice;
                chtlJsSlice.Content = content.substr(selectorStart, selectorEnd + 2 - selectorStart);
                chtlJsSlice.Type = SliceType::CHTL_JS;
                chtlJsSlice.StartPosition = selectorStart;
                chtlJsSlice.EndPosition = selectorEnd + 2;
                chtlJsSlice.LineNumber = m_Line;
                slices.push_back(chtlJsSlice);
                
                pos = selectorEnd + 2;
                lastPos = pos;
                continue;
            }
        }
        
        // 查找CHTL JS的 -> 操作符
        size_t arrowPos = content.find("->", pos);
        if (arrowPos != std::string::npos && arrowPos > lastPos) {
            // 检查是否是CHTL JS语法上下文
            bool isChtlJsContext = false;
            
            // 向前查找，看是否紧跟在}}之后
            size_t checkPos = arrowPos;
            while (checkPos > 0 && std::isspace(content[checkPos - 1])) {
                checkPos--;
            }
            if (checkPos >= 2 && content.substr(checkPos - 2, 2) == "}}") {
                isChtlJsContext = true;
            }
            
            if (isChtlJsContext) {
                // 找到方法调用的结束位置
                size_t methodEnd = arrowPos + 2;
                while (methodEnd < content.length() && 
                       (std::isalnum(content[methodEnd]) || content[methodEnd] == '_')) {
                    methodEnd++;
                }
                
                // 查找方法参数
                if (methodEnd < content.length() && content[methodEnd] == '(') {
                    int parenCount = 1;
                    methodEnd++;
                    while (methodEnd < content.length() && parenCount > 0) {
                        if (content[methodEnd] == '(') parenCount++;
                        else if (content[methodEnd] == ')') parenCount--;
                        methodEnd++;
                    }
                }
                
                // 添加->之前的内容为JS
                if (arrowPos > lastPos) {
                    CodeSlice jsSlice;
                    jsSlice.Content = content.substr(lastPos, arrowPos - lastPos);
                    jsSlice.Type = SliceType::JS;
                    jsSlice.StartPosition = lastPos;
                    jsSlice.EndPosition = arrowPos;
                    jsSlice.LineNumber = m_Line;
                    slices.push_back(jsSlice);
                }
                
                // 添加CHTL JS方法调用
                CodeSlice chtlJsSlice;
                chtlJsSlice.Content = content.substr(arrowPos, methodEnd - arrowPos);
                chtlJsSlice.Type = SliceType::CHTL_JS;
                chtlJsSlice.StartPosition = arrowPos;
                chtlJsSlice.EndPosition = methodEnd;
                chtlJsSlice.LineNumber = m_Line;
                slices.push_back(chtlJsSlice);
                
                pos = methodEnd;
                lastPos = pos;
                continue;
            }
        }
        
        // 查找 vir 关键字
        if (pos == 0 || !std::isalnum(content[pos - 1])) {
            if (content.substr(pos, 4) == "vir " && pos + 4 < content.length()) {
                // 添加vir之前的内容
                if (pos > lastPos) {
                    CodeSlice jsSlice;
                    jsSlice.Content = content.substr(lastPos, pos - lastPos);
                    jsSlice.Type = SliceType::JS;
                    jsSlice.StartPosition = lastPos;
                    jsSlice.EndPosition = pos;
                    jsSlice.LineNumber = m_Line;
                    slices.push_back(jsSlice);
                }
                
                // 找到vir声明的结束（通常是分号或换行）
                size_t virEnd = pos + 4;
                while (virEnd < content.length() && content[virEnd] != ';' && content[virEnd] != '\n') {
                    virEnd++;
                }
                if (virEnd < content.length() && content[virEnd] == ';') {
                    virEnd++; // 包含分号
                }
                
                // 添加vir声明
                CodeSlice chtlJsSlice;
                chtlJsSlice.Content = content.substr(pos, virEnd - pos);
                chtlJsSlice.Type = SliceType::CHTL_JS;
                chtlJsSlice.StartPosition = pos;
                chtlJsSlice.EndPosition = virEnd;
                chtlJsSlice.LineNumber = m_Line;
                slices.push_back(chtlJsSlice);
                
                pos = virEnd;
                lastPos = pos;
                continue;
            }
        }
        
        pos++;
    }
    
    // 添加剩余的JS代码
    if (lastPos < content.length()) {
        CodeSlice jsSlice;
        jsSlice.Content = content.substr(lastPos);
        jsSlice.Type = SliceType::JS;
        jsSlice.StartPosition = lastPos;
        jsSlice.EndPosition = content.length();
        jsSlice.LineNumber = m_Line;
        slices.push_back(jsSlice);
    }
    
    return slices;
}

void CHTLUnifiedScanner::AddSlice(SliceType type, size_t start, size_t end) {
    if (start >= end) return;
    
    CodeSlice slice;
    slice.Content = m_Source.substr(start, end - start);
    slice.Type = type;
    slice.StartPosition = start;
    slice.EndPosition = end;
    slice.LineNumber = m_Line;
    
    m_Slices.push_back(slice);
}

void CHTLUnifiedScanner::PerformSecondarySlicing(CodeSlice& slice) {
    // 对CHTL和CHTL JS切片进行二次切割，确保最小语法单元的完整性
    if (slice.Type == SliceType::CHTL) {
        // 检查是否包含CHTL特有语法
        // 例如：模板定义、自定义定义、导入语句等
        // 这里暂时保持原样，后续根据需要细化
    } else if (slice.Type == SliceType::CHTL_JS) {
        // CHTL JS已经是最小单元，不需要进一步切割
    }
}

bool CHTLUnifiedScanner::IsContextualBreakpoint(size_t position) {
    // 检查当前位置是否适合作为切割点
    if (position >= m_Source.length()) return true;
    
    // 不在字符串中间切割
    int quoteCount = 0;
    for (size_t i = 0; i < position; i++) {
        if (m_Source[i] == '"' && (i == 0 || m_Source[i-1] != '\\')) {
            quoteCount++;
        }
    }
    if (quoteCount % 2 != 0) return false;
    
    // 不在注释中间切割
    if (position > 1) {
        if (m_Source.substr(position - 2, 2) == "/*") return false;
        if (position > 0 && position < m_Source.length() - 1) {
            if (m_Source.substr(position - 1, 2) == "//") return false;
        }
    }
    
    return true;
}

bool CHTLUnifiedScanner::IsCHTLPattern(std::string_view content) {
    // 检查是否包含CHTL特有模式
    static const std::vector<std::string> chtlPatterns = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", "[Configuration]",
        "@Element", "@Style", "@Var", "@Html", "@JavaScript", "@Chtl", "@CJmod",
        "inherit", "delete", "insert", "from", "as"
    };
    
    for (const auto& pattern : chtlPatterns) {
        if (content.find(pattern) != std::string_view::npos) {
            return true;
        }
    }
    
    return false;
}

bool CHTLUnifiedScanner::IsCHTLJSPattern(std::string_view content) {
    // 检查是否包含CHTL JS特有模式
    return content.find("{{") != std::string_view::npos ||
           content.find("->") != std::string_view::npos ||
           content.find("vir ") != std::string_view::npos;
}

// 辅助方法实现
bool CHTLUnifiedScanner::IsAtEnd() const {
    return m_Current >= m_Source.length();
}

char CHTLUnifiedScanner::Peek(size_t offset) const {
    if (m_Current + offset >= m_Source.length()) return '\0';
    return m_Source[m_Current + offset];
}

char CHTLUnifiedScanner::Advance() {
    if (IsAtEnd()) return '\0';
    char c = m_Source[m_Current++];
    if (c == '\n') m_Line++;
    return c;
}

bool CHTLUnifiedScanner::Match(const std::string& expected) {
    if (m_Current + expected.length() > m_Source.length()) return false;
    
    if (m_Source.substr(m_Current, expected.length()) != expected) return false;
    
    m_Current += expected.length();
    return true;
}

void CHTLUnifiedScanner::SkipWhitespace() {
    while (!IsAtEnd()) {
        char c = Peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            Advance();
        } else if (c == '\n') {
            Advance();
        } else if (c == '/' && Peek(1) == '/') {
            // 跳过单行注释
            while (!IsAtEnd() && Peek() != '\n') Advance();
        } else if (c == '/' && Peek(1) == '*') {
            // 跳过多行注释
            Advance(); // /
            Advance(); // *
            while (!IsAtEnd() && !(Peek() == '*' && Peek(1) == '/')) {
                Advance();
            }
            if (!IsAtEnd()) {
                Advance(); // *
                Advance(); // /
            }
        } else {
            break;
        }
    }
}

void CHTLUnifiedScanner::RegisterFunction(std::shared_ptr<CHTLJSFunction> func) {
    m_RegisteredFunctions.push_back(func);
}

}