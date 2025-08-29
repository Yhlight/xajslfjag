#include "CJMODApi.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ========== Arg 实现 ==========

Arg::Arg(const std::string& name, const CJMODValue& defaultValue)
    : m_name(name), m_defaultValue(defaultValue), m_hasValue(false) {
}

Arg& Arg::bind(const CJMODValue& value) {
    m_value = value;
    m_hasValue = true;
    return *this;
}

Arg& Arg::fillValue(const CJMODValue& value) {
    if (!m_hasValue) {
        m_value = value;
        m_hasValue = true;
    }
    return *this;
}

template<typename T>
Arg& Arg::transform(std::function<T(const CJMODValue&)> transformer) {
    if (m_hasValue) {
        m_value = transformer(m_value);
    }
    return *this;
}

std::string Arg::asString() const {
    if (!m_hasValue) {
        return std::visit([](const auto& val) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, m_defaultValue);
    }
    
    return std::visit([](const auto& val) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            return val;
        } else {
            return std::to_string(val);
        }
    }, m_value);
}

int Arg::asInt() const {
    const auto& value = m_hasValue ? m_value : m_defaultValue;
    
    return std::visit([](const auto& val) -> int {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
            return val;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            try {
                return std::stoi(val);
            } catch (...) {
                return 0;
            }
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
            return static_cast<int>(val);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
            return val ? 1 : 0;
        }
        return 0;
    }, value);
}

double Arg::asDouble() const {
    const auto& value = m_hasValue ? m_value : m_defaultValue;
    
    return std::visit([](const auto& val) -> double {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
            return val;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            try {
                return std::stod(val);
            } catch (...) {
                return 0.0;
            }
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
            return static_cast<double>(val);
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
            return val ? 1.0 : 0.0;
        }
        return 0.0;
    }, value);
}

bool Arg::asBool() const {
    const auto& value = m_hasValue ? m_value : m_defaultValue;
    
    return std::visit([](const auto& val) -> bool {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, bool>) {
            return val;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            return !val.empty() && val != "false" && val != "0";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
            return val != 0;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
            return val != 0.0;
        }
        return false;
    }, value);
}

bool Arg::hasValue() const {
    return m_hasValue;
}

bool Arg::validate() const {
    return m_hasValue || !std::holds_alternative<std::string>(m_defaultValue) || 
           std::get<std::string>(m_defaultValue) != "";
}

// ========== AtomArg 实现 ==========

std::string AtomArg::processBasicPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args) {
    std::string result = template_;
    std::regex placeholder(R"(\$(\d+))");
    std::smatch match;
    
    auto it = result.cbegin();
    while (std::regex_search(it, result.cend(), match, placeholder)) {
        int index = std::stoi(match[1].str());
        
        if (index < static_cast<int>(args.size())) {
            std::string value = std::visit([](const auto& val) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                    return val;
                } else {
                    return std::to_string(val);
                }
            }, args[index]);
            
            result.replace(match.position(0), match.length(0), value);
        }
        
        it = result.cbegin() + match.position(0) + 1;
    }
    
    return result;
}

std::string AtomArg::processOptionalPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args) {
    std::string result = template_;
    std::regex placeholder(R"(\$\?(\d+))");
    std::smatch match;
    
    auto it = result.cbegin();
    while (std::regex_search(it, result.cend(), match, placeholder)) {
        int index = std::stoi(match[1].str());
        
        std::string value = "";
        if (index < static_cast<int>(args.size())) {
            value = std::visit([](const auto& val) -> std::string {
                if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                    return val;
                } else {
                    return std::to_string(val);
                }
            }, args[index]);
        }
        
        result.replace(match.position(0), match.length(0), value);
        it = result.cbegin() + match.position(0) + value.length();
    }
    
    return result;
}

std::string AtomArg::processRequiredPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args) {
    std::string result = template_;
    std::regex placeholder(R"(\$!(\d+))");
    std::smatch match;
    
    auto it = result.cbegin();
    while (std::regex_search(it, result.cend(), match, placeholder)) {
        int index = std::stoi(match[1].str());
        
        if (index >= static_cast<int>(args.size())) {
            throw std::runtime_error("Required argument $!" + std::to_string(index) + " not provided");
        }
        
        std::string value = std::visit([](const auto& val) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, args[index]);
        
        result.replace(match.position(0), match.length(0), value);
        it = result.cbegin() + match.position(0) + value.length();
    }
    
    return result;
}

std::string AtomArg::processDefaultPlaceholder(const std::string& template_, const CJMODValue& defaultValue) {
    std::string result = template_;
    std::regex placeholder(R"(\$_)");
    
    std::string value = std::visit([](const auto& val) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            return val;
        } else {
            return std::to_string(val);
        }
    }, defaultValue);
    
    return std::regex_replace(result, placeholder, value);
}

std::string AtomArg::processVariadicPlaceholder(const std::string& template_, const std::vector<CJMODValue>& args, size_t startIndex) {
    std::string result = template_;
    std::regex placeholder(R"(\.\.\.)");
    
    std::ostringstream variadicContent;
    for (size_t i = startIndex; i < args.size(); ++i) {
        if (i > startIndex) variadicContent << ", ";
        
        std::string value = std::visit([](const auto& val) -> std::string {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, args[i]);
        
        variadicContent << value;
    }
    
    return std::regex_replace(result, placeholder, variadicContent.str());
}

std::string AtomArg::processAllPlaceholders(const std::string& template_, const std::vector<CJMODValue>& args) {
    std::string result = template_;
    
    // 按顺序处理各种占位符
    result = processRequiredPlaceholder(result, args);
    result = processBasicPlaceholder(result, args);
    result = processOptionalPlaceholder(result, args);
    result = processVariadicPlaceholder(result, args, 0);
    
    return result;
}

// ========== Syntax namespace 实现 ==========

namespace Syntax {

SyntaxAnalysisResult analyze(const std::string& code) {
    SyntaxAnalysisResult result;
    
    // 检查基本语法结构
    if (code.find("function") != std::string::npos) {
        result = analyzeFunction(code);
    } else if (code.find("var") != std::string::npos || 
               code.find("let") != std::string::npos || 
               code.find("const") != std::string::npos) {
        result = analyzeVariable(code);
    } else if (code.find("import") != std::string::npos) {
        result = analyzeImport(code);
    } else if (code.find("type") != std::string::npos || 
               code.find("interface") != std::string::npos) {
        result = analyzeType(code);
    } else {
        result.type = "unknown";
        result.isValid = false;
        result.errorMessage = "Unrecognized syntax pattern";
    }
    
    return result;
}

SyntaxAnalysisResult analyzeFunction(const std::string& functionCode) {
    SyntaxAnalysisResult result;
    result.type = "function";
    
    // 解析函数名和参数
    std::regex funcPattern(R"(function\s+(\w+)\s*\(([^)]*)\)\s*\{)");
    std::smatch match;
    
    if (std::regex_search(functionCode, match, funcPattern)) {
        result.properties["name"] = match[1].str();
        result.properties["parameters"] = match[2].str();
        result.isValid = true;
    } else {
        result.isValid = false;
        result.errorMessage = "Invalid function syntax";
    }
    
    return result;
}

SyntaxAnalysisResult analyzeVariable(const std::string& variableCode) {
    SyntaxAnalysisResult result;
    result.type = "variable";
    
    // 解析变量声明
    std::regex varPattern(R"((var|let|const)\s+(\w+)\s*=\s*([^;]+))");
    std::smatch match;
    
    if (std::regex_search(variableCode, match, varPattern)) {
        result.properties["type"] = match[1].str();
        result.properties["name"] = match[2].str();
        result.properties["value"] = match[3].str();
        result.isValid = true;
    } else {
        result.isValid = false;
        result.errorMessage = "Invalid variable syntax";
    }
    
    return result;
}

SyntaxAnalysisResult analyzeImport(const std::string& importCode) {
    SyntaxAnalysisResult result;
    result.type = "import";
    
    // 解析导入语句
    std::regex importPattern(R"(import\s+(.+?)\s+from\s+['"]([^'"]+)['"])");
    std::smatch match;
    
    if (std::regex_search(importCode, match, importPattern)) {
        result.properties["imports"] = match[1].str();
        result.properties["module"] = match[2].str();
        result.dependencies.push_back(match[2].str());
        result.isValid = true;
    } else {
        result.isValid = false;
        result.errorMessage = "Invalid import syntax";
    }
    
    return result;
}

SyntaxAnalysisResult analyzeType(const std::string& typeCode) {
    SyntaxAnalysisResult result;
    result.type = "type";
    
    // 解析类型定义
    std::regex typePattern(R"((type|interface)\s+(\w+)\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(typeCode, match, typePattern)) {
        result.properties["kind"] = match[1].str();
        result.properties["name"] = match[2].str();
        result.properties["body"] = match[3].str();
        result.isValid = true;
    } else {
        result.isValid = false;
        result.errorMessage = "Invalid type syntax";
    }
    
    return result;
}

bool validateSyntax(const std::string& code, std::vector<std::string>& errors) {
    auto result = analyze(code);
    
    if (!result.isValid) {
        errors.push_back(result.errorMessage);
        return false;
    }
    
    // 额外的语法验证
    if (result.type == "function") {
        // 检查函数体匹配
        size_t openBraces = std::count(code.begin(), code.end(), '{');
        size_t closeBraces = std::count(code.begin(), code.end(), '}');
        
        if (openBraces != closeBraces) {
            errors.push_back("Mismatched braces in function");
            return false;
        }
    }
    
    return true;
}

} // namespace Syntax

// ========== CJMODScanner 实现 ==========

CJMODScanner::CJMODScanner() : m_frontPointer(0), m_backPointer(0) {
    initializePointers();
}

bool CJMODScanner::scan(const std::string& code) {
    cleanup();
    
    scanFunctions(code);
    scanVariables(code);
    scanTypes(code);
    scanImports(code);
    
    updateStats("total_scans");
    return true;
}

std::vector<std::string> CJMODScanner::getFunctions() const {
    return m_functions;
}

std::vector<std::string> CJMODScanner::getVariables() const {
    return m_variables;
}

std::vector<std::string> CJMODScanner::getTypes() const {
    return m_types;
}

std::vector<std::string> CJMODScanner::getImports() const {
    return m_imports;
}

std::string CJMODScanner::getScanStats() const {
    std::ostringstream ss;
    
    ss << "CJMOD Scanner Statistics:\n";
    ss << "========================\n";
    ss << "Functions: " << m_functions.size() << "\n";
    ss << "Variables: " << m_variables.size() << "\n";
    ss << "Types: " << m_types.size() << "\n";
    ss << "Imports: " << m_imports.size() << "\n";
    
    for (const auto& [category, count] : m_stats) {
        ss << category << ": " << count << "\n";
    }
    
    return ss.str();
}

void CJMODScanner::cleanup() {
    m_functions.clear();
    m_variables.clear();
    m_types.clear();
    m_imports.clear();
    m_stats.clear();
}

void CJMODScanner::scanFunctions(const std::string& code) {
    std::regex funcPattern(R"(function\s+(\w+))");
    std::sregex_iterator iter(code.begin(), code.end(), funcPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        m_functions.push_back(match[1].str());
        updateStats("functions_found");
    }
}

void CJMODScanner::scanVariables(const std::string& code) {
    std::regex varPattern(R"((var|let|const)\s+(\w+))");
    std::sregex_iterator iter(code.begin(), code.end(), varPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        m_variables.push_back(match[2].str());
        updateStats("variables_found");
    }
}

void CJMODScanner::scanTypes(const std::string& code) {
    std::regex typePattern(R"((type|interface)\s+(\w+))");
    std::sregex_iterator iter(code.begin(), code.end(), typePattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        m_types.push_back(match[2].str());
        updateStats("types_found");
    }
}

void CJMODScanner::scanImports(const std::string& code) {
    std::regex importPattern(R"(import\s+.+?\s+from\s+['"]([^'"]+)['"])");
    std::sregex_iterator iter(code.begin(), code.end(), importPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        m_imports.push_back(match[1].str());
        updateStats("imports_found");
    }
}

void CJMODScanner::updateStats(const std::string& category) {
    m_stats[category]++;
}

// ========== CJMODGenerator 实现 ==========

CJMODGenerator::CJMODGenerator() {
}

std::string CJMODGenerator::exportResult(const SyntaxAnalysisResult& analysis) {
    std::ostringstream ss;
    
    ss << "// Generated CJMOD Export\n";
    ss << "// Type: " << analysis.type << "\n";
    
    if (analysis.type == "function") {
        std::string name = std::get<std::string>(analysis.properties.at("name"));
        std::string params = std::get<std::string>(analysis.properties.at("parameters"));
        
        ss << "module.exports." << name << " = function(" << params << ") {\n";
        ss << "    // Implementation here\n";
        ss << "};\n";
    } else if (analysis.type == "variable") {
        std::string name = std::get<std::string>(analysis.properties.at("name"));
        std::string value = std::get<std::string>(analysis.properties.at("value"));
        
        ss << "module.exports." << name << " = " << value << ";\n";
    }
    
    return ss.str();
}

std::string CJMODGenerator::generateFunctionBinding(const std::string& functionName, const std::vector<Arg>& args) {
    std::ostringstream ss;
    
    ss << "// Function Binding: " << functionName << "\n";
    ss << "function " << functionName << "(";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << args[i].getName();
    }
    
    ss << ") {\n";
    ss << "    // Argument processing\n";
    
    for (const auto& arg : args) {
        ss << "    var " << arg.getName() << "_processed = " << arg.asString() << ";\n";
    }
    
    ss << "    // Function implementation\n";
    ss << "    return null;\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CJMODGenerator::generateVariableBinding(const std::string& variableName, const CJMODValue& value) {
    std::ostringstream ss;
    
    ss << "// Variable Binding: " << variableName << "\n";
    ss << "var " << variableName << " = ";
    
    std::visit([&ss](const auto& val) {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
            ss << "\"" << val << "\"";
        } else {
            ss << val;
        }
    }, value);
    
    ss << ";\n";
    
    return ss.str();
}

std::string CJMODGenerator::optimizeCode(const std::string& code) {
    std::string optimized = code;
    
    // 移除多余的空行
    optimized = std::regex_replace(optimized, std::regex(R"(\n\s*\n\s*\n)"), "\n\n");
    
    // 移除行尾空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+$)"), "");
    
    return optimized;
}

// ========== CHTLJSFunction namespace 实现 ==========

namespace CHTLJSFunction {

std::string CreateCHTLJSFunction(const std::string& functionName, 
                               const std::vector<Arg>& parameters,
                               const std::string& body) {
    std::ostringstream ss;
    
    ss << "// CHTL JS Function: " << functionName << "\n";
    ss << "function " << functionName << "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameters[i].getName();
    }
    
    ss << ") {\n";
    ss << body << "\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CreateEventListener(const std::string& eventName, 
                              const std::string& selector,
                              const std::string& handler) {
    std::ostringstream ss;
    
    ss << "// CHTL JS Event Listener\n";
    ss << "document.addEventListener('" << eventName << "', function(e) {\n";
    ss << "    if (e.target.matches('" << selector << "')) {\n";
    ss << "        " << handler << "\n";
    ss << "    }\n";
    ss << "});\n";
    
    return ss.str();
}

std::string CreateEventDelegate(const std::string& parentSelector,
                              const std::string& childSelector,
                              const std::string& eventName,
                              const std::string& handler) {
    std::ostringstream ss;
    
    ss << "// CHTL JS Event Delegate\n";
    ss << "document.querySelector('" << parentSelector << "').addEventListener('" << eventName << "', function(e) {\n";
    ss << "    if (e.target.matches('" << childSelector << "')) {\n";
    ss << "        " << handler << "\n";
    ss << "    }\n";
    ss << "});\n";
    
    return ss.str();
}

std::string CreateAnimation(const std::string& targetSelector,
                          const std::string& animationProperties,
                          int duration) {
    std::ostringstream ss;
    
    ss << "// CHTL JS Animation\n";
    ss << "document.querySelector('" << targetSelector << "').animate([\n";
    ss << "    " << animationProperties << "\n";
    ss << "], {\n";
    ss << "    duration: " << duration << ",\n";
    ss << "    fill: 'forwards'\n";
    ss << "});\n";
    
    return ss.str();
}

std::string CreateVirtualObject(const std::string& objectName,
                              const std::unordered_map<std::string, std::string>& methods) {
    std::ostringstream ss;
    
    ss << "// CHTL JS Virtual Object: " << objectName << "\n";
    ss << "var " << objectName << " = {\n";
    
    bool first = true;
    for (const auto& [methodName, methodBody] : methods) {
        if (!first) ss << ",\n";
        ss << "    " << methodName << ": function() {\n";
        ss << "        " << methodBody << "\n";
        ss << "    }";
        first = false;
    }
    
    ss << "\n};\n";
    
    return ss.str();
}

} // namespace CHTLJSFunction

// ========== CJMODAPIManager 实现 ==========

CJMODAPIManager::CJMODAPIManager()
    : m_initialized(false), m_debugMode(false) {
}

bool CJMODAPIManager::initialize() {
    if (m_initialized) return true;
    
    try {
        m_scanner = std::make_unique<CJMODScanner>();
        m_generator = std::make_unique<CJMODGenerator>();
        m_virtualBinder = std::make_unique<VirtualObjectBinder>();
        
        m_initialized = true;
        updateStats("initialization_success");
        
        return true;
    } catch (const std::exception& e) {
        reportError("Failed to initialize CJMOD API: " + std::string(e.what()));
        updateStats("initialization_failure");
        return false;
    }
}

SyntaxAnalysisResult CJMODAPIManager::analyzeSyntax(const std::string& code) {
    updateStats("syntax_analysis");
    return Syntax::analyze(code);
}

bool CJMODAPIManager::scanCode(const std::string& code) {
    if (!m_initialized) return false;
    
    updateStats("code_scans");
    return m_scanner->scan(code);
}

std::string CJMODAPIManager::generateBindings(const SyntaxAnalysisResult& analysis) {
    if (!m_initialized) return "";
    
    updateStats("binding_generations");
    return m_generator->exportResult(analysis);
}

std::string CJMODAPIManager::getAPIStats() const {
    std::ostringstream ss;
    
    ss << "CJMOD API Manager Statistics:\n";
    ss << "============================\n";
    ss << "Initialized: " << (m_initialized ? "Yes" : "No") << "\n";
    ss << "Debug Mode: " << (m_debugMode ? "Yes" : "No") << "\n\n";
    
    for (const auto& [category, count] : m_stats) {
        ss << category << ": " << count << "\n";
    }
    
    return ss.str();
}

void CJMODAPIManager::updateStats(const std::string& category) {
    m_stats[category]++;
}

void CJMODAPIManager::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "CJMOD API: " + message);
}

// ========== VirtualObjectBinder 实现 ==========

bool VirtualObjectBinder::bindVirtualObject(const std::string& objectName, 
                                          const std::unordered_map<std::string, std::function<CJMODValue(const std::vector<CJMODValue>&)>>& methods) {
    m_virtualObjects[objectName] = methods;
    return true;
}

CJMODValue VirtualObjectBinder::callVirtualMethod(const std::string& objectName, 
                                           const std::string& methodName,
                                           const std::vector<CJMODValue>& args) {
    auto objIt = m_virtualObjects.find(objectName);
    if (objIt == m_virtualObjects.end()) {
        return std::string("Object not found");
    }
    
    auto methodIt = objIt->second.find(methodName);
    if (methodIt == objIt->second.end()) {
        return std::string("Method not found");
    }
    
    return methodIt->second(args);
}

bool VirtualObjectBinder::hasVirtualObject(const std::string& objectName) const {
    return m_virtualObjects.find(objectName) != m_virtualObjects.end();
}

std::vector<std::string> VirtualObjectBinder::getVirtualObjectMethods(const std::string& objectName) const {
    std::vector<std::string> methods;
    auto it = m_virtualObjects.find(objectName);
    if (it != m_virtualObjects.end()) {
        for (const auto& [methodName, method] : it->second) {
            methods.push_back(methodName);
        }
    }
    return methods;
}

std::string VirtualObjectBinder::generateVirtualObjectJS(const std::string& objectName) {
    return "// Virtual object: " + objectName;
}

void VirtualObjectBinder::cleanup() {
    m_virtualObjects.clear();
}

bool VirtualObjectBinder::validateMethodCall(const std::string& objectName, const std::string& methodName) const {
    auto objIt = m_virtualObjects.find(objectName);
    if (objIt == m_virtualObjects.end()) return false;
    
    return objIt->second.find(methodName) != objIt->second.end();
}

// ========== CJMODScanner 双指针扫描和前置截取实现 ==========

bool CJMODScanner::dualPointerScan(const std::string& code) {
    initializePointers();
    m_fragmentBuffer.clear();
    
    // 双指针扫描算法
    while (m_backPointer < code.length()) {
        // 预先扫描一个片段检查是否存在关键字
        std::string currentWindow = getCurrentWindow(code);
        bool hasKeyword = false;
        
        for (const auto& keyword : getCJMODKeywords()) {
            if (hasKeywordInWindow(currentWindow, keyword)) {
                hasKeyword = true;
                // 通知后指针准备收集
                m_keywordBuffer.push_back(keyword);
                break;
            }
        }
        
        if (hasKeyword) {
            // 类似滑动窗口算法同步向前
            if (advancePointers(code)) {
                std::string fragment = code.substr(m_frontPointer, m_backPointer - m_frontPointer);
                if (isValidCJMODFragment(fragment)) {
                    m_fragmentBuffer.push_back(fragment);
                }
            }
        } else {
            // 移动前指针到合适的位置
            m_frontPointer = m_backPointer;
            m_backPointer++;
        }
    }
    
    return !m_fragmentBuffer.empty();
}

std::vector<std::string> CJMODScanner::prefixCutScan(const std::string& code, const std::string& keyword) {
    std::vector<std::string> cutFragments;
    
    // 前置截取机制 - 处理 arg ** arg2 这样的语法
    size_t keywordPos = findKeywordPosition(code, keyword);
    
    while (keywordPos != std::string::npos) {
        // 截取关键字前的片段
        std::string prefixFragment = extractPrefixFragment(code, keywordPos);
        
        if (!prefixFragment.empty() && isValidCJMODFragment(prefixFragment)) {
            cutFragments.push_back(prefixFragment);
        }
        
        // 继续查找下一个关键字
        keywordPos = findKeywordPosition(code.substr(keywordPos + keyword.length()), keyword);
        if (keywordPos != std::string::npos) {
            keywordPos += (keywordPos + keyword.length());
        }
    }
    
    return cutFragments;
}

bool CJMODScanner::hasKeywordInWindow(const std::string& fragment, const std::string& keyword) {
    return fragment.find(keyword) != std::string::npos;
}

std::vector<std::string> CJMODScanner::slidingWindowScan(const std::string& code, size_t windowSize) {
    std::vector<std::string> windows;
    
    // 滑动窗口算法扫描
    for (size_t i = 0; i <= code.length(); i += windowSize / 2) {
        if (i + windowSize <= code.length()) {
            std::string window = code.substr(i, windowSize);
            
            // 检查窗口是否包含CJMOD关键字
            for (const auto& keyword : getCJMODKeywords()) {
                if (hasKeywordInWindow(window, keyword)) {
                    windows.push_back(window);
                    break;
                }
            }
        }
    }
    
    return windows;
}

void CJMODScanner::initializePointers() {
    m_frontPointer = 0;
    m_backPointer = 0;
    m_keywordBuffer.clear();
    m_fragmentBuffer.clear();
}

bool CJMODScanner::advancePointers(const std::string& code) {
    if (m_backPointer >= code.length()) return false;
    
    // 同步向前移动双指针
    m_backPointer += 50; // 滑动窗口大小
    if (m_backPointer > code.length()) {
        m_backPointer = code.length();
    }
    
    return m_frontPointer < m_backPointer;
}

std::string CJMODScanner::getCurrentWindow(const std::string& code) {
    if (m_frontPointer >= code.length()) return "";
    
    size_t windowEnd = std::min(m_frontPointer + 100, code.length());
    return code.substr(m_frontPointer, windowEnd - m_frontPointer);
}

std::vector<std::string> CJMODScanner::getCJMODKeywords() {
    return {
        "**", "function", "var", "let", "const", "import", "export", 
        "module", "interface", "type", "class", "extends", "implements",
        "arg", "bind", "fillValue", "transform", "$", "$?", "$!", "$_", "..."
    };
}

size_t CJMODScanner::findKeywordPosition(const std::string& code, const std::string& keyword) {
    return code.find(keyword);
}

std::string CJMODScanner::extractPrefixFragment(const std::string& code, size_t keywordPos) {
    if (keywordPos == 0 || keywordPos == std::string::npos) return "";
    
    // 从关键字位置向前查找合适的分割点
    size_t startPos = 0;
    for (size_t i = keywordPos; i > 0; --i) {
        char c = code[i - 1];
        if (c == ';' || c == '\n' || c == '{' || c == '}') {
            startPos = i;
            break;
        }
    }
    
    return code.substr(startPos, keywordPos - startPos);
}

bool CJMODScanner::isValidCJMODFragment(const std::string& fragment) {
    if (fragment.empty()) return false;
    
    // 简单验证：检查是否包含有效的CJMOD语法元素
    for (const auto& keyword : getCJMODKeywords()) {
        if (fragment.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

} // namespace CHTL