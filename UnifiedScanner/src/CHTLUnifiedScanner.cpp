#include "UnifiedScanner/CHTLUnifiedScanner.h"
#include <algorithm>
#include <cctype>

namespace CHTL {

CHTLUnifiedScanner::CHTLUnifiedScanner(const std::string& source, const Config& config)
    : m_Source(source), m_Config(config) {
}

std::vector<CodeSlice> CHTLUnifiedScanner::Scan() {
    std::vector<CodeSlice> slices;
    
    while (!IsAtEnd()) {
        SkipWhitespace();
        if (IsAtEnd()) break;
        
        size_t startLine = m_Line;
        size_t startColumn = m_Column;
        size_t startPos = m_Current;
        
        SliceType type = DetectSliceType();
        CodeSlice slice;
        
        switch (type) {
            case SliceType::CHTL:
                slice = ScanCHTLSlice();
                break;
            case SliceType::CSS:
                slice = ScanStyleSlice();
                break;
            case SliceType::JS:
            case SliceType::CHTL_JS:
                slice = ScanScriptSlice();
                break;
            default:
                slice = ScanTextSlice();
                break;
        }
        
        slice.StartLine = startLine;
        slice.StartColumn = startColumn;
        
        // Apply registered processors if any
        if (m_Processors.find(slice.Type) != m_Processors.end()) {
            auto processedSlices = m_Processors[slice.Type]->Process(slice);
            slices.insert(slices.end(), processedSlices.begin(), processedSlices.end());
        } else {
            slices.push_back(slice);
        }
    }
    
    return slices;
}

void CHTLUnifiedScanner::RegisterProcessor(SliceType type, std::shared_ptr<ISliceProcessor> processor) {
    m_Processors[type] = processor;
}

bool CHTLUnifiedScanner::IsAtEnd() const {
    return m_Current >= m_Source.length();
}

char CHTLUnifiedScanner::Advance() {
    char c = m_Source[m_Current++];
    UpdatePosition(c);
    return c;
}

char CHTLUnifiedScanner::Peek() const {
    if (IsAtEnd()) return '\0';
    return m_Source[m_Current];
}

char CHTLUnifiedScanner::PeekNext() const {
    if (m_Current + 1 >= m_Source.length()) return '\0';
    return m_Source[m_Current + 1];
}

bool CHTLUnifiedScanner::Match(const std::string& expected) {
    if (m_Current + expected.length() > m_Source.length()) return false;
    
    for (size_t i = 0; i < expected.length(); i++) {
        if (m_Source[m_Current + i] != expected[i]) return false;
    }
    
    // Advance past the matched string
    for (size_t i = 0; i < expected.length(); i++) {
        Advance();
    }
    
    return true;
}

void CHTLUnifiedScanner::SkipWhitespace() {
    while (!IsAtEnd()) {
        char c = Peek();
        if (c == ' ' || c == '\r' || c == '\t' || c == '\n') {
            Advance();
        } else if (c == '/' && PeekNext() == '/') {
            // Skip line comment
            while (Peek() != '\n' && !IsAtEnd()) Advance();
        } else if (c == '/' && PeekNext() == '*') {
            // Skip block comment
            Advance(); // /
            Advance(); // *
            while (!IsAtEnd() && !(Peek() == '*' && PeekNext() == '/')) {
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

SliceType CHTLUnifiedScanner::DetectSliceType() {
    size_t savedPos = m_Current;
    size_t savedLine = m_Line;
    size_t savedCol = m_Column;
    
    // Look for CHTL constructs
    if (Match("style")) {
        SkipWhitespace();
        if (Match("{")) {
            // Restore position
            m_Current = savedPos;
            m_Line = savedLine;
            m_Column = savedCol;
            return SliceType::CSS;
        }
    }
    
    // Restore position for other checks
    m_Current = savedPos;
    m_Line = savedLine;
    m_Column = savedCol;
    
    if (Match("script")) {
        SkipWhitespace();
        if (Match("{")) {
            // Check if it's local script (inside element) or global
            // For now, assume it contains CHTL JS if local
            m_Current = savedPos;
            m_Line = savedLine;
            m_Column = savedCol;
            return SliceType::CHTL_JS;
        }
    }
    
    // Restore position
    m_Current = savedPos;
    m_Line = savedLine;
    m_Column = savedCol;
    
    // Default to CHTL
    return SliceType::CHTL;
}

CodeSlice CHTLUnifiedScanner::ScanCHTLSlice() {
    CodeSlice slice;
    slice.Type = SliceType::CHTL;
    
    size_t start = m_Current;
    
    // Scan until we find a style or script block
    while (!IsAtEnd()) {
        if (Match("style")) {
            size_t savedPos = m_Current;
            SkipWhitespace();
            if (Match("{")) {
                // Back up to before "style"
                m_Current = savedPos - 5; // Length of "style"
                break;
            }
        } else if (Match("script")) {
            size_t savedPos = m_Current;
            SkipWhitespace();
            if (Match("{")) {
                // Back up to before "script"
                m_Current = savedPos - 6; // Length of "script"
                break;
            }
        } else {
            Advance();
        }
    }
    
    slice.Content = m_Source.substr(start, m_Current - start);
    return slice;
}

CodeSlice CHTLUnifiedScanner::ScanStyleSlice() {
    CodeSlice slice;
    slice.Type = SliceType::CSS;
    
    // Skip "style"
    Match("style");
    SkipWhitespace();
    
    // Skip opening brace
    if (!Match("{")) {
        // Error: expected {
        slice.Content = "";
        return slice;
    }
    
    size_t start = m_Current;
    int braceCount = 1;
    
    // Scan until matching closing brace
    while (!IsAtEnd() && braceCount > 0) {
        if (Peek() == '{' && !IsInsideString()) {
            braceCount++;
        } else if (Peek() == '}' && !IsInsideString()) {
            braceCount--;
            if (braceCount == 0) {
                break;
            }
        }
        Advance();
    }
    
    slice.Content = m_Source.substr(start, m_Current - start);
    
    // Skip closing brace
    if (Peek() == '}') Advance();
    
    return slice;
}

CodeSlice CHTLUnifiedScanner::ScanScriptSlice() {
    CodeSlice slice;
    
    // Skip "script"
    Match("script");
    SkipWhitespace();
    
    // Skip opening brace
    if (!Match("{")) {
        // Error: expected {
        slice.Content = "";
        slice.Type = SliceType::JS;
        return slice;
    }
    
    // Determine if this is CHTL JS or regular JS
    // For now, check if it contains CHTL JS syntax like {{
    size_t start = m_Current;
    bool isCHTLJS = false;
    
    // Quick scan to detect CHTL JS syntax
    size_t scanPos = m_Current;
    while (scanPos < m_Source.length() - 1) {
        if (m_Source[scanPos] == '{' && m_Source[scanPos + 1] == '{') {
            isCHTLJS = true;
            break;
        }
        if (m_Source[scanPos] == '}' && !IsInsideString()) {
            break;
        }
        scanPos++;
    }
    
    slice.Type = isCHTLJS ? SliceType::CHTL_JS : SliceType::JS;
    
    int braceCount = 1;
    
    // Scan until matching closing brace
    while (!IsAtEnd() && braceCount > 0) {
        if (Peek() == '{' && !IsInsideString()) {
            braceCount++;
        } else if (Peek() == '}' && !IsInsideString()) {
            braceCount--;
            if (braceCount == 0) {
                break;
            }
        }
        Advance();
    }
    
    slice.Content = m_Source.substr(start, m_Current - start);
    
    // Skip closing brace
    if (Peek() == '}') Advance();
    
    return slice;
}

CodeSlice CHTLUnifiedScanner::ScanTextSlice() {
    CodeSlice slice;
    slice.Type = SliceType::CHTL;
    
    size_t start = m_Current;
    
    // For now, just advance one character
    // In a real implementation, this would scan text content
    Advance();
    
    slice.Content = m_Source.substr(start, m_Current - start);
    return slice;
}

std::string CHTLUnifiedScanner::ScanUntil(const std::string& delimiter) {
    size_t start = m_Current;
    
    while (!IsAtEnd()) {
        if (Match(delimiter)) {
            return m_Source.substr(start, m_Current - start - delimiter.length());
        }
        Advance();
    }
    
    return m_Source.substr(start);
}

bool CHTLUnifiedScanner::IsInsideString() const {
    // Simple check - in a real implementation, this would track string state
    // For now, just return false
    return false;
}

void CHTLUnifiedScanner::UpdatePosition(char c) {
    if (c == '\n') {
        m_Line++;
        m_Column = 1;
    } else {
        m_Column++;
    }
}

}