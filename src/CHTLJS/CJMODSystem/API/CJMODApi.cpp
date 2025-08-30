#include "CJMODApi.h"
#include <iostream>
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace CHTL {
namespace CJMOD {

// AtomArg::Impl
class AtomArg::Impl {
public:
    std::string value;
    AtomArgType type;
    
    Impl() : type(AtomArgType::Literal) {}
    Impl(const std::string& v, AtomArgType t) : value(v), type(t) {}
};

AtomArg::AtomArg() : pImpl(std::make_unique<Impl>()) {}

AtomArg::AtomArg(const std::string& value, AtomArgType type) 
    : pImpl(std::make_unique<Impl>(value, type)) {}

AtomArg::AtomArg(const AtomArg& other) 
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {}

AtomArg::AtomArg(AtomArg&& other) noexcept = default;

AtomArg& AtomArg::operator=(const AtomArg& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

AtomArg& AtomArg::operator=(AtomArg&& other) noexcept = default;

AtomArg::~AtomArg() = default;

const std::string& AtomArg::getValue() const {
    return pImpl->value;
}

void AtomArg::setValue(const std::string& value) {
    pImpl->value = value;
}

AtomArgType AtomArg::getType() const {
    return pImpl->type;
}

void AtomArg::setType(AtomArgType type) {
    pImpl->type = type;
}

bool AtomArg::isPlaceholder() const {
    return pImpl->type != AtomArgType::Literal && pImpl->type != AtomArgType::Keyword;
}

std::string AtomArg::toString() const {
    switch (pImpl->type) {
        case AtomArgType::Placeholder:
            return "$";
        case AtomArgType::OptionalPlaceholder:
            return "$?";
        case AtomArgType::VoidPlaceholder:
            return "$!";
        case AtomArgType::NegatePlaceholder:
            return "$^";
        case AtomArgType::VarPlaceholder:
            return "$_";
        case AtomArgType::SpecialPlaceholder:
            return "$#";
        default:
            return pImpl->value;
    }
}

// Arg::Impl
class Arg::Impl {
public:
    std::vector<AtomArg> atoms;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> bindings;
    std::string transformResult;
};

Arg::Arg() : pImpl(std::make_unique<Impl>()) {}

Arg::Arg(const std::vector<std::string>& values) : pImpl(std::make_unique<Impl>()) {
    for (const auto& value : values) {
        pImpl->atoms.emplace_back(value, AtomArgType::Literal);
    }
}

Arg::Arg(const std::vector<AtomArg>& atoms) : pImpl(std::make_unique<Impl>()) {
    pImpl->atoms = atoms;
}

Arg::Arg(Arg&& other) noexcept = default;

Arg& Arg::operator=(Arg&& other) noexcept = default;

Arg::~Arg() = default;

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < pImpl->atoms.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << pImpl->atoms[i].toString() << "\"";
    }
    std::cout << "]" << std::endl;
}

size_t Arg::size() const {
    return pImpl->atoms.size();
}

AtomArg& Arg::operator[](size_t index) {
    return pImpl->atoms[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return pImpl->atoms[index];
}

void Arg::push(const AtomArg& atom) {
    pImpl->atoms.push_back(atom);
}

void Arg::push(const std::string& value, AtomArgType type) {
    pImpl->atoms.emplace_back(value, type);
}

void Arg::bind(const std::string& placeholder, std::function<std::string(const std::string&)> func) {
    pImpl->bindings[placeholder] = func;
}

void Arg::fillValue(const Arg& values) {
    size_t valueIndex = 0;
    for (auto& atom : pImpl->atoms) {
        if (atom.isPlaceholder() && valueIndex < values.size()) {
            atom.setValue(values[valueIndex].getValue());
            atom.setType(AtomArgType::Literal);
            ++valueIndex;
        }
    }
}

void Arg::fillValue(const std::vector<std::string>& values) {
    size_t valueIndex = 0;
    for (auto& atom : pImpl->atoms) {
        if (atom.isPlaceholder() && valueIndex < values.size()) {
            atom.setValue(values[valueIndex]);
            atom.setType(AtomArgType::Literal);
            ++valueIndex;
        }
    }
}

void Arg::transform(const std::string& jsCode) {
    pImpl->transformResult = jsCode;
}

const std::string& Arg::getTransformResult() const {
    return pImpl->transformResult;
}

std::vector<AtomArg>::iterator Arg::begin() {
    return pImpl->atoms.begin();
}

std::vector<AtomArg>::iterator Arg::end() {
    return pImpl->atoms.end();
}

std::vector<AtomArg>::const_iterator Arg::begin() const {
    return pImpl->atoms.begin();
}

std::vector<AtomArg>::const_iterator Arg::end() const {
    return pImpl->atoms.end();
}

// Syntax实现
Arg Syntax::analyze(const std::string& syntax) {
    Arg result;
    std::istringstream stream(syntax);
    std::string token;
    
    while (stream >> token) {
        AtomArgType type = AtomArgType::Literal;
        
        // 识别占位符类型
        if (token == "$") {
            type = AtomArgType::Placeholder;
        } else if (token == "$?") {
            type = AtomArgType::OptionalPlaceholder;
        } else if (token == "$!") {
            type = AtomArgType::VoidPlaceholder;
        } else if (token == "$^") {
            type = AtomArgType::NegatePlaceholder;
        } else if (token == "$_") {
            type = AtomArgType::VarPlaceholder;
        } else if (token == "$#") {
            type = AtomArgType::SpecialPlaceholder;
        } else {
            // 检查是否是关键字
            if (token.size() > 0 && std::isalpha(token[0])) {
                type = AtomArgType::Keyword;
            }
        }
        
        result.push(token, type);
    }
    
    return result;
}

bool Syntax::isObject(const std::string& code) {
    std::string trimmed = Util::trim(code);
    return !trimmed.empty() && trimmed[0] == '{' && trimmed.back() == '}';
}

bool Syntax::isFunction(const std::string& code) {
    std::regex funcRegex(R"(^\s*function\s+\w+\s*\(.*\)\s*\{.*\}\s*$)");
    std::regex arrowRegex(R"(^\s*\(.*\)\s*=>\s*.*$)");
    
    return std::regex_match(code, funcRegex) || std::regex_match(code, arrowRegex);
}

bool Syntax::isArray(const std::string& code) {
    std::string trimmed = Util::trim(code);
    return !trimmed.empty() && trimmed[0] == '[' && trimmed.back() == ']';
}

bool Syntax::isCHTLJSFunction(const std::string& code) {
    // CHTL JS函数格式: functionName {param1: value1, param2: value2}
    std::regex chtljsRegex(R"(^\s*\w+\s*\{.*\}\s*;?\s*$)");
    return std::regex_match(code, chtljsRegex);
}

std::string Syntax::getFunctionName(const std::string& code) {
    std::regex funcRegex(R"(function\s+(\w+))");
    std::regex chtljsRegex(R"(^\s*(\w+)\s*\{)");
    std::smatch match;
    
    if (std::regex_search(code, match, funcRegex)) {
        return match[1];
    }
    
    if (std::regex_search(code, match, chtljsRegex)) {
        return match[1];
    }
    
    return "";
}

std::vector<std::pair<std::string, std::string>> Syntax::getObjectProperties(const std::string& code) {
    std::vector<std::pair<std::string, std::string>> properties;
    
    if (!isObject(code)) {
        return properties;
    }
    
    // 简单的属性解析（实际实现需要更复杂的解析器）
    std::string content = code.substr(1, code.length() - 2); // 去掉花括号
    std::regex propRegex(R"((\w+)\s*:\s*([^,}]+))");
    
    auto begin = std::sregex_iterator(content.begin(), content.end(), propRegex);
    auto end = std::sregex_iterator();
    
    for (auto it = begin; it != end; ++it) {
        std::smatch match = *it;
        properties.push_back({match[1], Util::trim(match[2])});
    }
    
    return properties;
}

std::vector<std::string> Syntax::getArrayElements(const std::string& code) {
    std::vector<std::string> elements;
    
    if (!isArray(code)) {
        return elements;
    }
    
    // 简单的数组解析（实际实现需要更复杂的解析器）
    std::string content = code.substr(1, code.length() - 2); // 去掉方括号
    return Util::split(content, ",");
}

// CJMODScanner实现
static std::string scanContext;
static size_t scanPosition = 0;

Arg CJMODScanner::scan(const Arg& /*pattern*/, const std::string& keyword) {
    Arg result;
    
    // 在扫描上下文中查找匹配的模式
    size_t pos = scanPosition;
    
    if (!keyword.empty()) {
        // 查找关键字
        pos = scanContext.find(keyword, scanPosition);
        if (pos != std::string::npos) {
            scanPosition = pos + keyword.length();
            
            // 提取匹配的值
            // 这里需要根据pattern的结构来提取值
            // result = std::move(pattern); // TODO: Implement proper pattern matching
        }
    } else {
        // 扫描整个模式
        // result = std::move(pattern); // TODO: Implement proper pattern matching
    }
    
    return result;
}

Arg CJMODScanner::dualPointerScan(const std::string& code, const std::string& startPattern, const std::string& endPattern) {
    Arg result;
    
    size_t start = code.find(startPattern);
    if (start == std::string::npos) {
        return result;
    }
    
    start += startPattern.length();
    size_t end = code.find(endPattern, start);
    
    if (end != std::string::npos) {
        std::string content = code.substr(start, end - start);
        result.push(content);
    }
    
    return result;
}

Arg CJMODScanner::preTruncateScan(const std::string& code, const std::string& delimiter) {
    Arg result;
    
    size_t pos = code.find(delimiter);
    if (pos != std::string::npos) {
        result.push(code.substr(0, pos));
        result.push(delimiter);
        result.push(code.substr(pos + delimiter.length()));
    } else {
        result.push(code);
    }
    
    return result;
}

void CJMODScanner::setContext(const std::string& code) {
    scanContext = code;
    scanPosition = 0;
}

size_t CJMODScanner::getCurrentPosition() {
    return scanPosition;
}

void CJMODScanner::setCurrentPosition(size_t pos) {
    scanPosition = pos;
}

void CJMODScanner::reset() {
    scanContext.clear();
    scanPosition = 0;
}

// CJMODGenerator实现
static std::string generatedCode;
static std::string outputMode = "javascript";
static std::vector<std::tuple<size_t, size_t, size_t, size_t>> sourceMappings;

void CJMODGenerator::exportResult(const Arg& args) {
    generatedCode += args.getTransformResult();
}

void CJMODGenerator::exportJavaScript(const std::string& jsCode) {
    if (outputMode == "javascript") {
        generatedCode += jsCode;
    }
}

void CJMODGenerator::exportCSS(const std::string& cssCode) {
    if (outputMode == "css") {
        generatedCode += cssCode;
    }
}

void CJMODGenerator::exportHTML(const std::string& htmlCode) {
    if (outputMode == "html") {
        generatedCode += htmlCode;
    }
}

void CJMODGenerator::setOutputMode(const std::string& mode) {
    outputMode = mode;
}

std::string CJMODGenerator::getGeneratedCode() {
    return generatedCode;
}

void CJMODGenerator::clearGeneratedCode() {
    generatedCode.clear();
    sourceMappings.clear();
}

void CJMODGenerator::addSourceMapping(size_t srcLine, size_t srcCol, size_t dstLine, size_t dstCol) {
    sourceMappings.push_back(std::make_tuple(srcLine, srcCol, dstLine, dstCol));
}

// CHTLJSFunction实现
static std::unordered_map<std::string, std::tuple<std::string, std::string>> functionRegistry;
static std::unordered_map<std::string, std::string> virtualObjectBindings;

void CHTLJSFunction::create(const std::string& name, const std::string& params, const std::string& body) {
    functionRegistry[name] = std::make_tuple(params, body);
}

void CHTLJSFunction::bindVirtualObject(const std::string& functionName) {
    virtualObjectBindings[functionName] = "vir";
}

std::string CHTLJSFunction::getVirtualObject(const std::string& functionName) {
    auto it = virtualObjectBindings.find(functionName);
    return it != virtualObjectBindings.end() ? it->second : "";
}

bool CHTLJSFunction::exists(const std::string& functionName) {
    return functionRegistry.find(functionName) != functionRegistry.end();
}

std::string CHTLJSFunction::call(const std::string& functionName, const std::vector<std::string>& args) {
    if (!exists(functionName)) {
        return "";
    }
    
    std::stringstream ss;
    ss << functionName << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << args[i];
    }
    ss << ")";
    
    return ss.str();
}

std::vector<std::string> CHTLJSFunction::getParameters(const std::string& functionName) {
    auto it = functionRegistry.find(functionName);
    if (it == functionRegistry.end()) {
        return {};
    }
    
    std::string params = std::get<0>(it->second);
    return Util::split(params, ",");
}

std::string CHTLJSFunction::getBody(const std::string& functionName) {
    auto it = functionRegistry.find(functionName);
    if (it == functionRegistry.end()) {
        return "";
    }
    
    return std::get<1>(it->second);
}

// Extension实现
static std::unordered_map<std::string, std::function<void(const Arg&)>> syntaxHandlers;
static std::unordered_map<std::string, std::function<std::string(const std::string&)>> selectorHandlers;
static std::unordered_map<std::string, std::function<std::string(const std::string&, const std::string&)>> operatorHandlers;
static std::unordered_map<std::string, std::function<std::string()>> virtualObjectGetters;
static std::unordered_map<std::string, std::function<std::string(const std::vector<std::string>&)>> functionHandlers;

void Extension::registerSyntax(const std::string& pattern, std::function<void(const Arg&)> handler) {
    syntaxHandlers[pattern] = handler;
}

void Extension::registerSelector(const std::string& selector, std::function<std::string(const std::string&)> handler) {
    selectorHandlers[selector] = handler;
}

void Extension::registerOperator(const std::string& op, std::function<std::string(const std::string&, const std::string&)> handler) {
    operatorHandlers[op] = handler;
}

void Extension::registerVirtualObject(const std::string& name, std::function<std::string()> getter) {
    virtualObjectGetters[name] = getter;
}

void Extension::registerFunction(const std::string& name, std::function<std::string(const std::vector<std::string>&)> func) {
    functionHandlers[name] = func;
}

void Extension::unregisterSyntax(const std::string& pattern) {
    syntaxHandlers.erase(pattern);
}

void Extension::unregisterSelector(const std::string& selector) {
    selectorHandlers.erase(selector);
}

void Extension::unregisterOperator(const std::string& op) {
    operatorHandlers.erase(op);
}

void Extension::unregisterVirtualObject(const std::string& name) {
    virtualObjectGetters.erase(name);
}

void Extension::unregisterFunction(const std::string& name) {
    functionHandlers.erase(name);
}

// Util实现
std::string Util::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c;
        }
    }
    return result;
}

std::string Util::unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '\"': result += '\"'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case 'n': result += '\n'; ++i; break;
                case 'r': result += '\r'; ++i; break;
                case 't': result += '\t'; ++i; break;
                default: result += str[i];
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

std::vector<std::string> Util::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
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

std::string Util::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) result += delimiter;
        result += parts[i];
    }
    return result;
}

std::string Util::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

std::string Util::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

} // namespace CJMOD
} // namespace CHTL