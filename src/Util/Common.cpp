#include "Common.h"

namespace CHTL {
namespace Utils {

String trim(const String& str) {
    size_t first = str.find_first_not_of(' ');
    if (String::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

StringVector split(const String& str, const String& delimiter) {
    StringVector tokens;
    size_t start = 0;
    size_t end = 0;
    
    while ((end = str.find(delimiter, start)) != String::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(str.substr(start));
    
    return tokens;
}

String join(const StringVector& parts, const String& delimiter) {
    if (parts.empty()) return "";
    
    String result = parts[0];
    for (size_t i = 1; i < parts.size(); ++i) {
        result += delimiter + parts[i];
    }
    return result;
}

bool startsWith(const String& str, const String& prefix) {
    return str.length() >= prefix.length() && 
           str.compare(0, prefix.length(), prefix) == 0;
}

bool endsWith(const String& str, const String& suffix) {
    return str.length() >= suffix.length() && 
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

String toLowerCase(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

String toUpperCase(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool isValidIdentifier(const String& str) {
    if (str.empty()) return false;
    if (!std::isalpha(str[0]) && str[0] != '_') return false;
    
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_') return false;
    }
    return true;
}

bool fileExists(const String& path) {
    std::ifstream file(path);
    return file.good();
}

String readFile(const String& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const String& path, const String& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot write to file: " + path);
    }
    file << content;
}

} // namespace Utils
} // namespace CHTL