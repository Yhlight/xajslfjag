#pragma once

#include "Common.h"

namespace CHTL::Util {

// C++17兼容的字符串工具函数
class StringUtils {
public:
    // ends_with implementation for C++17
    static bool ends_with(const String& str, const String& suffix) {
        if (str.length() >= suffix.length()) {
            return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
        }
        return false;
    }
    
    // starts_with implementation for C++17
    static bool starts_with(const String& str, const String& prefix) {
        if (str.length() >= prefix.length()) {
            return str.compare(0, prefix.length(), prefix) == 0;
        }
        return false;
    }
    
    // 字符串分割
    static StringVector split(const String& str, const String& delimiter) {
        StringVector result;
        size_t start = 0;
        size_t end = str.find(delimiter);
        
        while (end != String::npos) {
            result.push_back(str.substr(start, end - start));
            start = end + delimiter.length();
            end = str.find(delimiter, start);
        }
        
        result.push_back(str.substr(start));
        return result;
    }
    
    // 字符串修剪空白
    static String trim(const String& str) {
        size_t start = str.find_first_not_of(" \t\n\r\f\v");
        if (start == String::npos) return "";
        
        size_t end = str.find_last_not_of(" \t\n\r\f\v");
        return str.substr(start, end - start + 1);
    }
    
    // 字符串替换
    static String replace_all(String str, const String& from, const String& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != String::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
    
    // 检查字符串是否为数字
    static bool is_numeric(const String& str) {
        if (str.empty()) return false;
        
        size_t start = 0;
        if (str[0] == '-' || str[0] == '+') start = 1;
        
        bool has_dot = false;
        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '.') {
                if (has_dot) return false;
                has_dot = true;
            } else if (!std::isdigit(str[i])) {
                return false;
            }
        }
        
        return start < str.length();
    }
};

} // namespace CHTL::Util