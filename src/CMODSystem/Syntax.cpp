#include "Syntax.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CJMOD {

// Syntax类实现
Arg Syntax::analyze(const CHTL::String& pattern) {
    CHTL::StringVector tokens = tokenizePattern(pattern);
    std::vector<AtomArg> atoms;
    
    for (const auto& token : tokens) {
        if (token.starts_with("$") || token == "...") {
            AtomArg::PlaceholderType type = AtomArg::parsePlaceholderType(token);
            atoms.emplace_back(token, type);
        } else {
            // 非占位符token作为普通原子
            atoms.emplace_back(token, AtomArg::PlaceholderType::NORMAL);
        }
    }
    
    return Arg(atoms);
}

bool Syntax::isObject(const CHTL::String& content) {
    return isValidJSObject(content);
}

bool Syntax::isFunction(const CHTL::String& content) {
    return isValidJSFunction(content);
}

bool Syntax::isArray(const CHTL::String& content) {
    return isValidJSArray(content);
}

bool Syntax::isCHTLJSFunction(const CHTL::String& content) {
    return isValidCHTLJSFunction(content);
}

CHTL::StringVector Syntax::tokenizePattern(const CHTL::String& pattern) {
    CHTL::StringVector tokens;
    std::istringstream iss(pattern);
    CHTL::String token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool Syntax::isValidJSObject(const CHTL::String& content) {
    std::regex objectPattern = getObjectPattern();
    return std::regex_match(content, objectPattern);
}

bool Syntax::isValidJSFunction(const CHTL::String& content) {
    std::regex functionPattern = getFunctionPattern();
    return std::regex_match(content, functionPattern);
}

bool Syntax::isValidJSArray(const CHTL::String& content) {
    std::regex arrayPattern = getArrayPattern();
    return std::regex_match(content, arrayPattern);
}

bool Syntax::isValidCHTLJSFunction(const CHTL::String& content) {
    std::regex chtljsPattern = getCHTLJSFunctionPattern();
    return std::regex_match(content, chtljsPattern);
}

std::regex Syntax::getObjectPattern() {
    return std::regex(R"(\s*\{[^}]*\}\s*)");
}

std::regex Syntax::getFunctionPattern() {
    return std::regex(R"(\s*function\s*[a-zA-Z_$][a-zA-Z0-9_$]*\s*\([^)]*\)\s*\{[^}]*\}\s*)");
}

std::regex Syntax::getArrayPattern() {
    return std::regex(R"(\s*\[[^\]]*\]\s*)");
}

std::regex Syntax::getCHTLJSFunctionPattern() {
    return std::regex(R"(\s*[a-zA-Z_$][a-zA-Z0-9_$]*\s*\{[^}]*\}\s*;?\s*)");
}

// AtomArg类实现
AtomArg::AtomArg(const CHTL::String& ph, PlaceholderType t)
    : placeholder(ph), type(t), isBound(false), isFilled(false) {}

void AtomArg::bind(std::function<CHTL::String(const CHTL::String&)> func) {
    bindFunction = func;
    isBound = true;
}

void AtomArg::fillValue(const CHTL::String& val) {
    if (isBound && bindFunction) {
        value = bindFunction(val);
    } else {
        value = val;
    }
    isFilled = true;
}

void AtomArg::fillValue(int val) {
    fillValue(std::to_string(val));
}

void AtomArg::fillValue(double val) {
    fillValue(std::to_string(val));
}

void AtomArg::fillValue(bool val) {
    fillValue(val ? "true" : "false");
}

CHTL::String AtomArg::getValue() const {
    return isFilled ? value : placeholder;
}

CHTL::String AtomArg::toString() const {
    return getValue();
}

AtomArg::PlaceholderType AtomArg::parsePlaceholderType(const CHTL::String& placeholder) {
    if (placeholder == "...") return PlaceholderType::VARIADIC;
    if (placeholder.ends_with("!_") || placeholder.ends_with("_!")) return PlaceholderType::REQUIRED;
    if (placeholder.ends_with("?_") || placeholder.ends_with("_?")) return PlaceholderType::OPTIONAL;
    if (placeholder.ends_with("!")) return PlaceholderType::REQUIRED;
    if (placeholder.ends_with("?")) return PlaceholderType::OPTIONAL;
    if (placeholder.ends_with("_")) return PlaceholderType::UNORDERED;
    return PlaceholderType::NORMAL;
}

CHTL::String AtomArg::placeholderTypeToString(PlaceholderType type) {
    switch (type) {
        case PlaceholderType::NORMAL: return "NORMAL";
        case PlaceholderType::OPTIONAL: return "OPTIONAL";
        case PlaceholderType::REQUIRED: return "REQUIRED";
        case PlaceholderType::UNORDERED: return "UNORDERED";
        case PlaceholderType::VARIADIC: return "VARIADIC";
        default: return "UNKNOWN";
    }
}

// Arg类实现
Arg::Arg(const CHTL::StringVector& placeholders) : isTransformed(false) {
    for (const auto& ph : placeholders) {
        AtomArg::PlaceholderType type = AtomArg::parsePlaceholderType(ph);
        atoms.emplace_back(ph, type);
    }
}

Arg::Arg(const std::vector<AtomArg>& atomList) : atoms(atomList), isTransformed(false) {}

AtomArg& Arg::operator[](size_t index) {
    return atoms[index];
}

const AtomArg& Arg::operator[](size_t index) const {
    return atoms[index];
}

AtomArg& Arg::at(size_t index) {
    if (index >= atoms.size()) {
        throw std::out_of_range("AtomArg index out of range");
    }
    return atoms[index];
}

const AtomArg& Arg::at(size_t index) const {
    if (index >= atoms.size()) {
        throw std::out_of_range("AtomArg index out of range");
    }
    return atoms[index];
}

void Arg::bind(const CHTL::String& placeholder, std::function<CHTL::String(const CHTL::String&)> func) {
    for (auto& atom : atoms) {
        if (atom.placeholder == placeholder) {
            atom.bind(func);
        }
    }
}

void Arg::bindAll(std::function<CHTL::String(const CHTL::String&)> func) {
    for (auto& atom : atoms) {
        atom.bind(func);
    }
}

void Arg::bindByIndex(size_t index, std::function<CHTL::String(const CHTL::String&)> func) {
    if (index < atoms.size()) {
        atoms[index].bind(func);
    }
}

void Arg::fillValue(const Arg& other) {
    for (size_t i = 0; i < std::min(atoms.size(), other.atoms.size()); ++i) {
        if (other.atoms[i].isFilled) {
            atoms[i].fillValue(other.atoms[i].value);
        }
    }
}

void Arg::fillValue(const CHTL::StringVector& values) {
    for (size_t i = 0; i < std::min(atoms.size(), values.size()); ++i) {
        atoms[i].fillValue(values[i]);
    }
    
    // 处理可变参数
    if (values.size() > atoms.size()) {
        expandVariadicArgs(values);
    }
}

void Arg::fillValueByPlaceholder(const CHTL::String& placeholder, const CHTL::String& value) {
    for (auto& atom : atoms) {
        if (atom.placeholder == placeholder) {
            atom.fillValue(value);
        }
    }
}

void Arg::fillValueByIndex(size_t index, const CHTL::String& value) {
    if (index < atoms.size()) {
        atoms[index].fillValue(value);
    }
}

void Arg::transform(const CHTL::String& pattern) {
    transformPattern = pattern;
    isTransformed = true;
}

CHTL::String Arg::getTransformedResult() const {
    if (!isTransformed) {
        return toString();
    }
    return applyTransformPattern();
}

bool Arg::isComplete() const {
    return !hasUnboundPlaceholders() && !hasUnfilledRequired();
}

bool Arg::hasUnboundPlaceholders() const {
    return std::any_of(atoms.begin(), atoms.end(),
                      [](const AtomArg& atom) {
                          return !atom.isBound && atom.placeholder.starts_with("$");
                      });
}

bool Arg::hasUnfilledRequired() const {
    return std::any_of(atoms.begin(), atoms.end(),
                      [](const AtomArg& atom) {
                          return !atom.isFilled && 
                                (atom.type == AtomArg::PlaceholderType::REQUIRED);
                      });
}

CHTL::StringVector Arg::getUnboundPlaceholders() const {
    CHTL::StringVector unbound;
    for (const auto& atom : atoms) {
        if (!atom.isBound && atom.placeholder.starts_with("$")) {
            unbound.push_back(atom.placeholder);
        }
    }
    return unbound;
}

CHTL::StringVector Arg::getUnfilledRequired() const {
    CHTL::StringVector unfilled;
    for (const auto& atom : atoms) {
        if (!atom.isFilled && atom.type == AtomArg::PlaceholderType::REQUIRED) {
            unfilled.push_back(atom.placeholder);
        }
    }
    return unfilled;
}

void Arg::print() const {
    std::cout << "[";
    for (size_t i = 0; i < atoms.size(); ++i) {
        std::cout << "\"" << atoms[i].getValue() << "\"";
        if (i < atoms.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

CHTL::String Arg::toString() const {
    std::ostringstream oss;
    for (size_t i = 0; i < atoms.size(); ++i) {
        oss << atoms[i].getValue();
        if (i < atoms.size() - 1) oss << " ";
    }
    return oss.str();
}

CHTL::String Arg::toDebugString() const {
    std::ostringstream oss;
    oss << "Arg[size=" << atoms.size() << ", transformed=" << isTransformed << "] {\n";
    for (size_t i = 0; i < atoms.size(); ++i) {
        oss << "  [" << i << "] placeholder='" << atoms[i].placeholder 
            << "' value='" << atoms[i].getValue() 
            << "' type=" << AtomArg::placeholderTypeToString(atoms[i].type)
            << " bound=" << atoms[i].isBound 
            << " filled=" << atoms[i].isFilled << "\n";
    }
    oss << "}";
    return oss.str();
}

AtomArg* Arg::findByPlaceholder(const CHTL::String& placeholder) {
    auto it = std::find_if(atoms.begin(), atoms.end(),
                          [&placeholder](const AtomArg& atom) {
                              return atom.placeholder == placeholder;
                          });
    return (it != atoms.end()) ? &(*it) : nullptr;
}

const AtomArg* Arg::findByPlaceholder(const CHTL::String& placeholder) const {
    auto it = std::find_if(atoms.begin(), atoms.end(),
                          [&placeholder](const AtomArg& atom) {
                              return atom.placeholder == placeholder;
                          });
    return (it != atoms.end()) ? &(*it) : nullptr;
}

bool Arg::hasPlaceholder(const CHTL::String& placeholder) const {
    return findByPlaceholder(placeholder) != nullptr;
}

void Arg::removePlaceholder(const CHTL::String& placeholder) {
    atoms.erase(std::remove_if(atoms.begin(), atoms.end(),
                              [&placeholder](const AtomArg& atom) {
                                  return atom.placeholder == placeholder;
                              }),
                atoms.end());
}

void Arg::expandVariadicArgs(const CHTL::StringVector& values) {
    // 找到可变参数
    auto variadicIt = std::find_if(atoms.begin(), atoms.end(),
                                  [](const AtomArg& atom) {
                                      return atom.type == AtomArg::PlaceholderType::VARIADIC;
                                  });
    
    if (variadicIt != atoms.end()) {
        size_t variadicIndex = std::distance(atoms.begin(), variadicIt);
        size_t extraValues = values.size() - atoms.size() + 1;
        
        // 移除原始可变参数
        atoms.erase(variadicIt);
        
        // 添加扩展的参数
        for (size_t i = 0; i < extraValues; ++i) {
            AtomArg varArg("variadic_" + std::to_string(i), AtomArg::PlaceholderType::NORMAL);
            varArg.fillValue(values[variadicIndex + i]);
            atoms.insert(atoms.begin() + variadicIndex + i, varArg);
        }
    }
}

CHTL::String Arg::applyTransformPattern() const {
    CHTL::String result = transformPattern;
    
    // 简单的占位符替换
    for (size_t i = 0; i < atoms.size(); ++i) {
        CHTL::String placeholder = "arg[" + std::to_string(i) + "]";
        size_t pos = result.find(placeholder);
        while (pos != CHTL::String::npos) {
            result.replace(pos, placeholder.length(), atoms[i].getValue());
            pos = result.find(placeholder, pos + atoms[i].getValue().length());
        }
    }
    
    return result;
}

void Arg::validatePlaceholders() const {
    // 验证占位符的有效性
    for (const auto& atom : atoms) {
        if (atom.type == AtomArg::PlaceholderType::REQUIRED && !atom.isFilled) {
            throw std::runtime_error("Required placeholder '" + atom.placeholder + "' is not filled");
        }
    }
}

} // namespace CJMOD