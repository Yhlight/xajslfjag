#include "CJMODScanner.h"
#include <regex>

namespace CHTL {
namespace CJMOD {

Arg CJMODScanner::scan(const Arg& pattern, const std::string& code) {
    Arg result;
    size_t codePos = 0;
    
    for (const auto& item : pattern.getItems()) {
        std::string matchedValue;
        
        if (matchPatternItem(item.pattern, code, codePos, matchedValue)) {
            ArgItem resultItem(item.pattern, item.position);
            resultItem.value = matchedValue;
            result.addItem(resultItem);
        } else {
            // 匹配失败
            return Arg();
        }
    }
    
    return result;
}

bool CJMODScanner::scanAndValidate(const Arg& pattern, const std::string& code) {
    Arg result = scan(pattern, code);
    return result.size() == pattern.size();
}

bool CJMODScanner::matchPatternItem(const std::string& patternItem, 
                                   const std::string& code, 
                                   size_t& pos,
                                   std::string& matchedValue) {
    // 跳过空白
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    
    if (patternItem == "$") {
        // 匹配一个值（数字、标识符、字符串等）
        size_t start = pos;
        
        if (code[pos] == '"' || code[pos] == '\'') {
            // 字符串
            char quote = code[pos];
            pos++;
            while (pos < code.length() && code[pos] != quote) {
                if (code[pos] == '\\') pos++; // 跳过转义字符
                pos++;
            }
            if (pos < code.length()) pos++; // 跳过结束引号
        } else if (std::isdigit(code[pos]) || code[pos] == '-') {
            // 数字
            if (code[pos] == '-') pos++;
            while (pos < code.length() && (std::isdigit(code[pos]) || code[pos] == '.')) {
                pos++;
            }
        } else if (std::isalpha(code[pos]) || code[pos] == '_') {
            // 标识符
            while (pos < code.length() && (std::isalnum(code[pos]) || code[pos] == '_')) {
                pos++;
            }
        } else {
            return false;
        }
        
        matchedValue = extractValue(code, start, pos);
        return true;
    } else {
        // 匹配字面量
        size_t patternLen = patternItem.length();
        if (pos + patternLen <= code.length() && 
            code.substr(pos, patternLen) == patternItem) {
            matchedValue = patternItem;
            pos += patternLen;
            return true;
        }
    }
    
    return false;
}

std::string CJMODScanner::extractValue(const std::string& code, 
                                      size_t start, 
                                      size_t end) {
    return code.substr(start, end - start);
}

} // namespace CJMOD
} // namespace CHTL