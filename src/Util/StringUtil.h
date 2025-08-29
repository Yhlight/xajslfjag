#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <memory>
#include <cstdio>

namespace CHTL {

class StringUtil {
public:
    // 去除空白
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    
    // 分割字符串
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    
    // 连接字符串
    static std::string join(const std::vector<std::string>& parts, const std::string& delimiter);
    
    // 替换字符串
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // 前缀后缀检查
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // 大小写转换
    static std::string toUpper(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // 判断是否为数字
    static bool isNumber(const std::string& str);
    
    // 判断是否为标识符
    static bool isIdentifier(const std::string& str);
    
    // 转义/反转义
    static std::string escape(const std::string& str);
    static std::string unescape(const std::string& str);
    
    // 格式化
    template<typename... Args>
    static std::string format(const std::string& fmt, Args... args);
    
    // 编码转换
    static std::string toUtf8(const std::string& str);
    static std::string fromUtf8(const std::string& str);
};

// StringUtil implementation
inline std::string StringUtil::trim(const std::string& str) {
    return ltrim(rtrim(str));
}

inline std::string StringUtil::ltrim(const std::string& str) {
    auto it = std::find_if(str.begin(), str.end(), [](int ch) {
        return !std::isspace(ch);
    });
    return std::string(it, str.end());
}

inline std::string StringUtil::rtrim(const std::string& str) {
    auto it = std::find_if(str.rbegin(), str.rend(), [](int ch) {
        return !std::isspace(ch);
    });
    return std::string(str.begin(), it.base());
}

inline std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    if (delimiter.empty()) {
        result.push_back(str);
        return result;
    }
    
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

inline std::string StringUtil::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    if (parts.empty()) return "";
    
    std::string result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter + parts[i];
    }
    return result;
}

inline std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    
    size_t pos = str.find(from);
    if (pos == std::string::npos) return str;
    
    std::string result = str;
    result.replace(pos, from.length(), to);
    return result;
}

inline std::string StringUtil::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return str;
    
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

inline bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    if (prefix.length() > str.length()) return false;
    return str.compare(0, prefix.length(), prefix) == 0;
}

inline bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    if (suffix.length() > str.length()) return false;
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

inline std::string StringUtil::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

inline std::string StringUtil::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

inline bool StringUtil::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    auto it = str.begin();
    if (*it == '-' || *it == '+') ++it;
    
    bool hasDigit = false;
    bool hasDot = false;
    
    for (; it != str.end(); ++it) {
        if (std::isdigit(*it)) {
            hasDigit = true;
        } else if (*it == '.' && !hasDot) {
            hasDot = true;
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

inline bool StringUtil::isIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    if (!std::isalpha(str[0]) && str[0] != '_') return false;
    
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_') return false;
    }
    
    return true;
}

inline std::string StringUtil::escape(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default: result += c;
        }
    }
    return result;
}

inline std::string StringUtil::unescape(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n': result += '\n'; ++i; break;
                case 'r': result += '\r'; ++i; break;
                case 't': result += '\t'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case '"': result += '"'; ++i; break;
                case '\'': result += '\''; ++i; break;
                default: result += str[i];
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

// 使用snprintf实现简单的格式化
template<typename... Args>
inline std::string StringUtil::format(const std::string& fmt, Args... args) {
    size_t size = snprintf(nullptr, 0, fmt.c_str(), args...) + 1;
    if (size <= 0) { 
        return "";
    }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, fmt.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

inline std::string StringUtil::toUtf8(const std::string& str) {
    // 简单实现：假设输入已经是UTF-8
    // 在实际应用中，应该使用iconv或ICU库进行转换
    return str;
}

inline std::string StringUtil::fromUtf8(const std::string& str) {
    // 简单实现：假设输出需要UTF-8
    // 在实际应用中，应该使用iconv或ICU库进行转换
    return str;
}

} // namespace CHTL

#endif // STRING_UTIL_H