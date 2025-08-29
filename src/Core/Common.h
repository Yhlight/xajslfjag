#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <stack>
#include <queue>
#include <set>
#include <unordered_set>

namespace CHTL {
    using String = std::string;
    using StringVector = std::vector<String>;
    using StringMap = std::map<String, String>;
    using StringUnorderedMap = std::unordered_map<String, String>;
    
    // Forward declarations
    class Token;
    class AST;
    class Context;
    class Scanner;
    class Parser;
    class Generator;
    
    // 基础配置常量
    namespace Config {
        constexpr int INDEX_INITIAL_COUNT = 0;
        constexpr bool DISABLE_NAME_GROUP = true;
        constexpr bool DISABLE_CUSTOM_ORIGIN_TYPE = false;
        constexpr bool DEBUG_MODE = false;
        constexpr bool DISABLE_STYLE_AUTO_ADD_CLASS = false;
        constexpr bool DISABLE_STYLE_AUTO_ADD_ID = false;
        constexpr bool DISABLE_SCRIPT_AUTO_ADD_CLASS = true;
        constexpr bool DISABLE_SCRIPT_AUTO_ADD_ID = true;
        constexpr bool DISABLE_DEFAULT_NAMESPACE = false;
    }
    
    // 工具函数
    namespace Utils {
        String trim(const String& str);
        StringVector split(const String& str, const String& delimiter);
        String join(const StringVector& parts, const String& delimiter);
        bool startsWith(const String& str, const String& prefix);
        bool endsWith(const String& str, const suffix);
        String toLowerCase(const String& str);
        String toUpperCase(const String& str);
        bool isValidIdentifier(const String& str);
        bool fileExists(const String& path);
        String readFile(const String& path);
        void writeFile(const String& path, const String& content);
    }
}