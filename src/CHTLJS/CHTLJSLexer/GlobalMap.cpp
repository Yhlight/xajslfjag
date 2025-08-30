#include "GlobalMap.h"
#include <regex>
#include <algorithm>

namespace CHTL {
namespace JS {

// CHTLJSKeywordMap实现
std::unordered_map<String, String> CHTLJSKeywordMap::keywordMap;

const std::unordered_map<String, String>& CHTLJSKeywordMap::getKeywordMap() {
    if (keywordMap.empty()) {
        initializeKeywordMap();
    }
    return keywordMap;
}

void CHTLJSKeywordMap::initializeKeywordMap() {
    keywordMap = {
        // JavaScript关键字
        {"const", "CONST"},
        {"let", "LET"},
        {"var", "VAR"},
        {"function", "FUNCTION"},
        {"return", "RETURN"},
        {"if", "IF"},
        {"else", "ELSE"},
        {"for", "FOR"},
        {"while", "WHILE"},
        {"do", "DO"},
        {"break", "BREAK"},
        {"continue", "CONTINUE"},
        {"class", "CLASS"},
        {"extends", "EXTENDS"},
        {"new", "NEW"},
        {"this", "THIS"},
        {"try", "TRY"},
        {"catch", "CATCH"},
        {"finally", "FINALLY"},
        {"throw", "THROW"},
        {"true", "BOOLEAN"},
        {"false", "BOOLEAN"},
        {"null", "NULL"},
        {"undefined", "UNDEFINED"},
        
        // CHTL JS特有关键字
        {"listen", "LISTEN"},
        {"delegate", "DELEGATE"},
        {"animate", "ANIMATE"},
        {"vir", "VIR"},
        {"module", "MODULE"},
        {"load", "LOAD"}
    };
}

bool CHTLJSKeywordMap::isKeyword(const String& word) {
    const auto& map = getKeywordMap();
    return map.find(word) != map.end();
}

CHTLJSTokenType CHTLJSKeywordMap::getKeywordType(const String& word) {
    if (word == "const") return CHTLJSTokenType::CONST;
    if (word == "let") return CHTLJSTokenType::LET;
    if (word == "var") return CHTLJSTokenType::VAR;
    if (word == "function") return CHTLJSTokenType::FUNCTION;
    if (word == "return") return CHTLJSTokenType::RETURN;
    if (word == "if") return CHTLJSTokenType::IF;
    if (word == "else") return CHTLJSTokenType::ELSE;
    if (word == "for") return CHTLJSTokenType::FOR;
    if (word == "while") return CHTLJSTokenType::WHILE;
    if (word == "do") return CHTLJSTokenType::DO;
    if (word == "break") return CHTLJSTokenType::BREAK;
    if (word == "continue") return CHTLJSTokenType::CONTINUE;
    if (word == "class") return CHTLJSTokenType::CLASS;
    if (word == "extends") return CHTLJSTokenType::EXTENDS;
    if (word == "new") return CHTLJSTokenType::NEW;
    if (word == "this") return CHTLJSTokenType::THIS;
    if (word == "try") return CHTLJSTokenType::TRY;
    if (word == "catch") return CHTLJSTokenType::CATCH;
    if (word == "finally") return CHTLJSTokenType::FINALLY;
    if (word == "throw") return CHTLJSTokenType::THROW;
    if (word == "true" || word == "false") return CHTLJSTokenType::BOOLEAN;
    if (word == "null") return CHTLJSTokenType::NULL_LITERAL;
    if (word == "undefined") return CHTLJSTokenType::UNDEFINED;
    
    // CHTL JS关键字
    if (word == "listen") return CHTLJSTokenType::LISTEN;
    if (word == "delegate") return CHTLJSTokenType::DELEGATE;
    if (word == "animate") return CHTLJSTokenType::ANIMATE;
    if (word == "vir") return CHTLJSTokenType::VIR;
    if (word == "module") return CHTLJSTokenType::MODULE;
    if (word == "load") return CHTLJSTokenType::LOAD;
    
    return CHTLJSTokenType::IDENTIFIER;
}

StringVector CHTLJSKeywordMap::getAllKeywords() {
    const auto& map = getKeywordMap();
    StringVector keywords;
    for (const auto& pair : map) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

bool CHTLJSKeywordMap::isCHTLJSKeyword(const String& word) {
    return word == "listen" || word == "delegate" || word == "animate" || 
           word == "vir" || word == "module" || word == "load";
}

bool CHTLJSKeywordMap::isJavaScriptKeyword(const String& word) {
    return isKeyword(word) && !isCHTLJSKeyword(word);
}

bool CHTLJSKeywordMap::isCJMODKeyword(const String& word) {
    return word == "printMylove" || word == "iNeverAway";
}

// CHTLJSOperatorMap实现
std::unordered_map<String, String> CHTLJSOperatorMap::operatorMap;
std::unordered_map<String, int> CHTLJSOperatorMap::precedenceMap;
std::unordered_map<String, bool> CHTLJSOperatorMap::associativityMap;

const std::unordered_map<String, String>& CHTLJSOperatorMap::getOperatorMap() {
    if (operatorMap.empty()) {
        initializeOperatorMap();
    }
    return operatorMap;
}

void CHTLJSOperatorMap::initializeOperatorMap() {
    operatorMap = {
        // CHTL JS特殊操作符
        {"->", "ARROW_OP"},
        {"=>", "DOUBLE_ARROW"},
        {"&->", "EVENT_BIND"},
        {"{{", "SELECTOR_START"},
        {"}}", "SELECTOR_END"},
        
        // 标准操作符
        {"+", "PLUS"},
        {"-", "MINUS"},
        {"*", "MULTIPLY"},
        {"/", "DIVIDE"},
        {"%", "MODULO"},
        {"=", "ASSIGN"},
        {"+=", "PLUS_ASSIGN"},
        {"-=", "MINUS_ASSIGN"},
        {"*=", "MULT_ASSIGN"},
        {"/=", "DIV_ASSIGN"},
        {"%=", "MOD_ASSIGN"},
        {"**", "POWER"},
        {"++", "INCREMENT"},
        {"--", "DECREMENT"},
        
        // 比较操作符
        {"==", "EQUAL"},
        {"!=", "NOT_EQUAL"},
        {"===", "STRICT_EQUAL"},
        {"!==", "STRICT_NOT_EQUAL"},
        {"<", "LESS_THAN"},
        {">", "GREATER_THAN"},
        {"<=", "LESS_EQUAL"},
        {">=", "GREATER_EQUAL"},
        
        // 逻辑操作符
        {"&&", "LOGICAL_AND"},
        {"||", "LOGICAL_OR"},
        {"!", "LOGICAL_NOT"},
        
        // 位操作符
        {"&", "BITWISE_AND"},
        {"|", "BITWISE_OR"},
        {"^", "BITWISE_XOR"},
        {"~", "BITWISE_NOT"},
        {"<<", "LEFT_SHIFT"},
        {">>", "RIGHT_SHIFT"},
        {">>>", "UNSIGNED_RIGHT_SHIFT"}
    };
    
    initializePrecedenceMap();
}

void CHTLJSOperatorMap::initializePrecedenceMap() {
    precedenceMap = {
        {"(", 19}, {")", 19}, {"[", 19}, {"]", 19}, {".", 19}, {"->", 19},
        {"++", 18}, {"--", 18}, {"!", 18}, {"~", 18},
        {"**", 17},
        {"*", 16}, {"/", 16}, {"%", 16},
        {"+", 15}, {"-", 15},
        {"<<", 14}, {">>", 14}, {">>>", 14},
        {"<", 13}, {"<=", 13}, {">", 13}, {">=", 13},
        {"==", 12}, {"!=", 12}, {"===", 12}, {"!==", 12},
        {"&", 11},
        {"^", 10},
        {"|", 9},
        {"&&", 8},
        {"||", 7},
        {"=>", 6},
        {"=", 5}, {"+=", 5}, {"-=", 5}, {"*=", 5}, {"/=", 5}, {"%=", 5},
        {"&->", 4},  // CHTL JS事件绑定
        {",", 3}
    };
    
    // 左结合性（true）或右结合性（false）
    associativityMap = {
        {"=", false}, {"+=", false}, {"-=", false}, {"*=", false}, 
        {"/=", false}, {"%=", false}, {"**", false}, {"=>", false},
        {"&->", false}  // CHTL JS事件绑定是右结合
    };
}

bool CHTLJSOperatorMap::isOperator(const String& op) {
    const auto& map = getOperatorMap();
    return map.find(op) != map.end();
}

CHTLJSTokenType CHTLJSOperatorMap::getOperatorType(const String& op) {
    if (op == "->") return CHTLJSTokenType::ARROW_OP;
    if (op == "=>") return CHTLJSTokenType::DOUBLE_ARROW;
    if (op == "&->") return CHTLJSTokenType::EVENT_BIND;
    if (op == "{{") return CHTLJSTokenType::SELECTOR_START;
    if (op == "}}") return CHTLJSTokenType::SELECTOR_END;
    if (op == "+") return CHTLJSTokenType::PLUS;
    if (op == "-") return CHTLJSTokenType::MINUS;
    if (op == "*") return CHTLJSTokenType::MULTIPLY;
    if (op == "/") return CHTLJSTokenType::DIVIDE;
    if (op == "%") return CHTLJSTokenType::MODULO;
    if (op == "=") return CHTLJSTokenType::ASSIGN;
    if (op == "+=") return CHTLJSTokenType::PLUS_ASSIGN;
    if (op == "-=") return CHTLJSTokenType::MINUS_ASSIGN;
    if (op == "*=") return CHTLJSTokenType::MULT_ASSIGN;
    if (op == "/=") return CHTLJSTokenType::DIV_ASSIGN;
    if (op == "%=") return CHTLJSTokenType::MOD_ASSIGN;
    if (op == "**") return CHTLJSTokenType::POWER;
    if (op == "++") return CHTLJSTokenType::INCREMENT;
    if (op == "--") return CHTLJSTokenType::DECREMENT;
    
    return CHTLJSTokenType::INVALID;
}

StringVector CHTLJSOperatorMap::getAllOperators() {
    const auto& map = getOperatorMap();
    StringVector operators;
    for (const auto& pair : map) {
        operators.push_back(pair.first);
    }
    return operators;
}

int CHTLJSOperatorMap::getOperatorPrecedence(const String& op) {
    auto it = precedenceMap.find(op);
    return it != precedenceMap.end() ? it->second : 0;
}

bool CHTLJSOperatorMap::isLeftAssociative(const String& op) {
    auto it = associativityMap.find(op);
    return it == associativityMap.end() || it->second;  // 默认左结合
}

bool CHTLJSOperatorMap::isRightAssociative(const String& op) {
    return !isLeftAssociative(op);
}

bool CHTLJSOperatorMap::isCHTLJSOperator(const String& op) {
    return op == "->" || op == "&->" || op == "{{" || op == "}}";
}

bool CHTLJSOperatorMap::isEventBindingOperator(const String& op) {
    return op == "&->";
}

bool CHTLJSOperatorMap::isSelectorOperator(const String& op) {
    return op == "{{" || op == "}}";
}

// CHTLJSSelectorMap实现
std::unordered_map<String, CHTLJSSelectorMap::SelectorInfo> CHTLJSSelectorMap::selectorMap;

const std::unordered_map<String, CHTLJSSelectorMap::SelectorInfo>& CHTLJSSelectorMap::getSelectorMap() {
    if (selectorMap.empty()) {
        initializeSelectorMap();
    }
    return selectorMap;
}

void CHTLJSSelectorMap::initializeSelectorMap() {
    selectorMap = {
        {"*", {"*", "document.querySelectorAll('*')", false, {}}},
        {"body", {"body", "document.body", false, {}}},
        {"html", {"html", "document.documentElement", false, {}}},
        {"head", {"head", "document.head", false, {}}},
        
        // 类选择器模式
        {".class", {"\\.[a-zA-Z][a-zA-Z0-9_-]*", "document.querySelectorAll('$&')", false, {}}},
        
        // ID选择器模式
        {"#id", {"#[a-zA-Z][a-zA-Z0-9_-]*", "document.getElementById('$1')", false, {}}},
        
        // 标签选择器模式
        {"tag", {"[a-zA-Z][a-zA-Z0-9]*", "document.querySelectorAll('$&')", false, {}}},
        
        // 属性选择器模式
        {"[attr]", {"\\[[^\\]]+\\]", "document.querySelectorAll('$&')", false, {}}},
        
        // 伪类选择器模式
        {":pseudo", {":[a-zA-Z][a-zA-Z0-9-]*", "document.querySelectorAll('$&')", false, {}}},
        
        // 复合选择器模式
        {"descendant", {".+ .+", "document.querySelectorAll('$&')", false, {}}},
        {"child", {".+ > .+", "document.querySelectorAll('$&')", false, {}}},
        {"sibling", {".+ \\+ .+", "document.querySelectorAll('$&')", false, {}}},
        {"general", {".+ ~ .+", "document.querySelectorAll('$&')", false, {}}}
    };
}

bool CHTLJSSelectorMap::isValidSelector(const String& selector) {
    if (selector.empty()) return false;
    
    // 基本验证：检查是否为有效的CSS选择器
    return isValidCSSSelector(selector);
}

String CHTLJSSelectorMap::translateToJS(const String& selector, const String& context) {
    if (selector.empty()) return "";
    
    // 特殊情况处理
    if (selector == "body") return "document.body";
    if (selector == "html") return "document.documentElement";
    if (selector == "head") return "document.head";
    
    // ID选择器优化
    if (selector.size() > 1 && selector[0] == '#') {
        String id = selector.substr(1);
        return "document.getElementById('" + id + "')";
    }
    
    // 通用查询选择器
    String contextPrefix = context.empty() ? "document" : context;
    return contextPrefix + ".querySelectorAll('" + selector + "')";
}

StringVector CHTLJSSelectorMap::extractSelectors(const String& source) {
    StringVector selectors;
    std::regex selectorPattern(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(source.begin(), source.end(), selectorPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        String selector = (*iter)[1].str();
        // 清理空白字符
        selector.erase(std::remove_if(selector.begin(), selector.end(), ::isspace), selector.end());
        if (!selector.empty()) {
            selectors.push_back(selector);
        }
    }
    
    return selectors;
}

bool CHTLJSSelectorMap::isValidCSSSelector(const String& selector) {
    if (selector.empty()) return false;
    
    // 基础CSS选择器验证
    // 这里简化验证，实际应该更严格
    std::regex validSelectorPattern(R"(^[.#]?[a-zA-Z][a-zA-Z0-9_-]*$|^[.#]?[a-zA-Z][a-zA-Z0-9_-]*(\s*[>+~]\s*[.#]?[a-zA-Z][a-zA-Z0-9_-]*)*$)");
    return std::regex_match(selector, validSelectorPattern);
}

String CHTLJSSelectorMap::optimizeSelector(const String& selector) {
    String optimized = selector;
    
    // 移除多余空白
    std::regex multiSpace(R"(\s+)");
    optimized = std::regex_replace(optimized, multiSpace, " ");
    
    // 移除前后空白
    optimized.erase(0, optimized.find_first_not_of(" \t"));
    optimized.erase(optimized.find_last_not_of(" \t") + 1);
    
    return optimized;
}

// CHTLJSGlobalMap实现
bool CHTLJSGlobalMap::initialized = false;

void CHTLJSGlobalMap::initialize() {
    if (!initialized) {
        initializeAll();
        initialized = true;
    }
}

bool CHTLJSGlobalMap::isInitialized() {
    return initialized;
}

void CHTLJSGlobalMap::initializeAll() {
    // 初始化所有映射表
    CHTLJSKeywordMap::getKeywordMap();
    CHTLJSOperatorMap::getOperatorMap();
    CHTLJSSelectorMap::getSelectorMap();
}

bool CHTLJSGlobalMap::isReservedWord(const String& word) {
    return CHTLJSKeywordMap::isKeyword(word);
}

bool CHTLJSGlobalMap::isBuiltinFunction(const String& name) {
    return CHTLJSKeywordMap::isCHTLJSKeyword(name) || CHTLJSKeywordMap::isCJMODKeyword(name);
}

bool CHTLJSGlobalMap::isValidOperator(const String& op) {
    return CHTLJSOperatorMap::isOperator(op);
}

bool CHTLJSGlobalMap::isValidSelector(const String& selector) {
    return CHTLJSSelectorMap::isValidSelector(selector);
}

String CHTLJSGlobalMap::translateCHTLJSToJS(const String& source) {
    String result = source;
    
    // 翻译选择器
    StringVector selectors = CHTLJSSelectorMap::extractSelectors(source);
    for (const String& selector : selectors) {
        String jsCode = CHTLJSSelectorMap::translateToJS(selector);
        String pattern = "{{" + selector + "}}";
        size_t pos = result.find(pattern);
        if (pos != String::npos) {
            result.replace(pos, pattern.length(), jsCode);
        }
    }
    
    return result;
}

StringVector CHTLJSGlobalMap::extractDependencies(const String& source) {
    StringVector dependencies;
    
    // 提取模块依赖
    std::regex modulePattern(R"(module\s*\{\s*([^}]+)\s*\})");
    std::sregex_iterator iter(source.begin(), source.end(), modulePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        String moduleContent = (*iter)[1].str();
        // 解析模块内容以提取依赖
        std::regex loadPattern(R"(load:\s*['"]\s*([^'"]+)\s*['"])");
        std::sregex_iterator loadIter(moduleContent.begin(), moduleContent.end(), loadPattern);
        std::sregex_iterator loadEnd;
        
        for (; loadIter != loadEnd; ++loadIter) {
            dependencies.push_back((*loadIter)[1].str());
        }
    }
    
    return dependencies;
}

String CHTLJSGlobalMap::optimizeGeneratedCode(const String& jsCode) {
    String optimized = jsCode;
    
    // 移除多余的空白行
    std::regex multiNewline(R"(\n\s*\n)");
    optimized = std::regex_replace(optimized, multiNewline, "\n");
    
    // 移除行尾空白
    std::regex trailingSpace(R"(\s+$)", std::regex_constants::multiline);
    optimized = std::regex_replace(optimized, trailingSpace, "");
    
    return optimized;
}

StringVector CHTLJSGlobalMap::validateCHTLJSSyntax(const String& source) {
    StringVector errors;
    
    // 验证选择器语法
    StringVector selectors = CHTLJSSelectorMap::extractSelectors(source);
    for (const String& selector : selectors) {
        if (!CHTLJSSelectorMap::isValidCSSSelector(selector)) {
            errors.push_back("无效的CSS选择器: " + selector);
        }
    }
    
    // 验证事件绑定语法
    std::regex eventBindPattern(R"(\{\{[^}]+\}\}\s*&->\s*\w+)");
    if (!std::regex_search(source, eventBindPattern)) {
        // 可能没有事件绑定，这不是错误
    }
    
    return errors;
}

bool CHTLJSGlobalMap::hasConflicts(const String& source) {
    // 检查语法冲突
    StringVector errors = validateCHTLJSSyntax(source);
    return !errors.empty();
}

} // namespace JS
} // namespace CHTL