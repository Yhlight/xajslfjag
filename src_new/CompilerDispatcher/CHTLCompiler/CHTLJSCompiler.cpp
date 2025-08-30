#include "CHTLJSCompiler.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ========== CHTLJSSyntaxAnalyzer 实现 ==========

CHTLJSSyntaxAnalyzer::ParsedFunction CHTLJSSyntaxAnalyzer::parseFunction(const std::string& content) {
    ParsedFunction func;
    
    // 检测虚拟对象声明
    std::regex virPattern(R"(vir\s+(\w+)\s*=\s*(\w+)\s*\{([^}]*)\})");
    std::smatch virMatch;
    
    if (std::regex_search(content, virMatch, virPattern)) {
        func.name = virMatch[1].str();
        func.type = virMatch[2].str();
        func.body = virMatch[3].str();
        func.isVirtual = true;
        return func;
    }
    
    // 检测普通CHTL JS函数
    std::regex funcPattern(R"((\w+)\s*\{([^}]*)\})");
    std::smatch funcMatch;
    
    if (std::regex_search(content, funcMatch, funcPattern)) {
        func.type = funcMatch[1].str();
        func.body = funcMatch[2].str();
        func.isVirtual = false;
        
        // 解析函数参数
        std::regex paramPattern(R"((\w+):\s*([^,}]+))");
        std::sregex_iterator paramIter(func.body.begin(), func.body.end(), paramPattern);
        std::sregex_iterator paramEnd;
        
        for (; paramIter != paramEnd; ++paramIter) {
            std::smatch paramMatch = *paramIter;
            func.parameters[paramMatch[1].str()] = paramMatch[2].str();
        }
    }
    
    return func;
}

std::vector<CHTLJSSyntaxAnalyzer::EnhancedSelector> CHTLJSSyntaxAnalyzer::parseSelectors(const std::string& content) {
    std::vector<EnhancedSelector> selectors;
    
    // 解析 {{selector}} 格式
    std::regex selectorPattern(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(content.begin(), content.end(), selectorPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        EnhancedSelector selector;
        selector.original = match[0].str();
        
        std::string selectorContent = match[1].str();
        
        // 解析索引访问 selector[index]
        std::regex indexPattern(R"((.+)\[(\d+)\])");
        std::smatch indexMatch;
        
        if (std::regex_match(selectorContent, indexMatch, indexPattern)) {
            selectorContent = indexMatch[1].str();
            selector.index = std::stoi(indexMatch[2].str());
        } else {
            selector.index = -1;
        }
        
        // 确定选择器类型
        if (selectorContent[0] == '.') {
            selector.type = "class";
            selector.selector = selectorContent;
            if (selector.index >= 0) {
                selector.jsExpression = "document.querySelectorAll('" + selector.selector + "')[" + std::to_string(selector.index) + "]";
            } else {
                selector.jsExpression = "document.querySelector('" + selector.selector + "')";
            }
        } else if (selectorContent[0] == '#') {
            selector.type = "id";
            selector.selector = selectorContent;
            selector.jsExpression = "document.getElementById('" + selectorContent.substr(1) + "')";
        } else if (selectorContent.find(' ') != std::string::npos) {
            selector.type = "complex";
            selector.selector = selectorContent;
            if (selector.index >= 0) {
                selector.jsExpression = "document.querySelectorAll('" + selector.selector + "')[" + std::to_string(selector.index) + "]";
            } else {
                selector.jsExpression = "document.querySelector('" + selector.selector + "')";
            }
        } else {
            selector.type = "tag";
            selector.selector = selectorContent;
            if (selector.index >= 0) {
                selector.jsExpression = "document.getElementsByTagName('" + selector.selector + "')[" + std::to_string(selector.index) + "]";
            } else {
                selector.jsExpression = "document.querySelector('" + selector.selector + "')";
            }
        }
        
        selectors.push_back(selector);
    }
    
    return selectors;
}

bool CHTLJSSyntaxAnalyzer::parseEventBinding(const std::string& content, 
                                           std::string& element, 
                                           std::string& event, 
                                           std::string& handler) {
    std::regex bindingPattern(R"((.+?)\s*&->\s*(\w+)\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, bindingPattern)) {
        element = match[1].str();
        event = match[2].str();
        handler = match[3].str();
        return true;
    }
    
    return false;
}

bool CHTLJSSyntaxAnalyzer::isVirtualObjectDeclaration(const std::string& content) {
    std::regex virPattern(R"(vir\s+\w+\s*=)");
    return std::regex_search(content, virPattern);
}

CHTLJSSyntaxAnalyzer::ParsedFunction CHTLJSSyntaxAnalyzer::parseVirtualObject(const std::string& content) {
    return parseFunction(content);
}

// ========== CHTLJSCodeGenerator 实现 ==========

std::string CHTLJSCodeGenerator::generateEventListener(const CHTLJSSyntaxAnalyzer::ParsedFunction& func) {
    std::stringstream ss;
    
    // 获取目标元素
    std::string target = func.parameters.count("target") ? func.parameters.at("target") : "this";
    
    ss << "// Generated CHTL JS Event Listener\n";
    ss << "(function() {\n";
    ss << "    const target = " << target << ";\n";
    ss << "    if (!target) return;\n\n";
    
    // 解析事件处理器
    std::regex eventPattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(func.body.begin(), func.body.end(), eventPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string eventName = match[1].str();
        std::string handler = match[2].str();
        
        ss << "    target.addEventListener('" << eventName << "', " << handler << ");\n";
    }
    
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::generateEventDelegate(const CHTLJSSyntaxAnalyzer::ParsedFunction& func) {
    std::stringstream ss;
    
    std::string parent = func.parameters.count("parent") ? func.parameters.at("parent") : "document";
    std::string target = func.parameters.count("target") ? func.parameters.at("target") : "";
    
    ss << "// Generated CHTL JS Event Delegate\n";
    ss << "(function() {\n";
    ss << "    const parent = " << parent << ";\n";
    ss << "    if (!parent) return;\n\n";
    
    std::regex eventPattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(func.body.begin(), func.body.end(), eventPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string eventName = match[1].str();
        std::string handler = match[2].str();
        
        ss << "    parent.addEventListener('" << eventName << "', function(e) {\n";
        if (!target.empty()) {
            ss << "        if (e.target.matches('" << target << "')) {\n";
            ss << "            (" << handler << ").call(e.target, e);\n";
            ss << "        }\n";
        } else {
            ss << "        (" << handler << ").call(e.target, e);\n";
        }
        ss << "    });\n";
    }
    
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::generateAnimation(const CHTLJSSyntaxAnalyzer::ParsedFunction& func) {
    std::stringstream ss;
    
    std::string target = func.parameters.count("target") ? func.parameters.at("target") : "this";
    std::string duration = func.parameters.count("duration") ? func.parameters.at("duration") : "300";
    std::string easing = func.parameters.count("easing") ? func.parameters.at("easing") : "ease";
    
    ss << "// Generated CHTL JS Animation\n";
    ss << "(function() {\n";
    ss << "    const target = " << target << ";\n";
    ss << "    if (!target) return;\n\n";
    
    ss << "    const animation = target.animate([\n";
    
    // 解析关键帧
    if (func.parameters.count("begin")) {
        ss << "        " << func.parameters.at("begin") << ",\n";
    }
    
    if (func.parameters.count("end")) {
        ss << "        " << func.parameters.at("end") << "\n";
    }
    
    ss << "    ], {\n";
    ss << "        duration: " << duration << ",\n";
    ss << "        easing: '" << easing << "',\n";
    ss << "        fill: 'forwards'\n";
    ss << "    });\n\n";
    
    if (func.parameters.count("callback")) {
        ss << "    animation.addEventListener('finish', " << func.parameters.at("callback") << ");\n";
    }
    
    ss << "    return animation;\n";
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::generateVirtualObject(const CHTLJSSyntaxAnalyzer::ParsedFunction& func) {
    std::stringstream ss;
    
    ss << "// Generated Virtual Object: " << func.name << "\n";
    ss << "const " << func.name << "_impl = {\n";
    
    // 解析虚拟对象的方法和属性
    std::regex memberPattern(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(func.body.begin(), func.body.end(), memberPattern);
    std::sregex_iterator end;
    
    bool first = true;
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string memberName = match[1].str();
        std::string memberValue = match[2].str();
        
        if (!first) ss << ",\n";
        ss << "    " << memberName << ": " << memberValue;
        first = false;
    }
    
    ss << "\n};\n\n";
    
    ss << "const " << func.name << " = {\n";
    ss << "    get: function(key) { return " << func.name << "_impl[key]; },\n";
    ss << "    call: function(key, ...args) {\n";
    ss << "        const method = " << func.name << "_impl[key];\n";
    ss << "        return typeof method === 'function' ? method(...args) : method;\n";
    ss << "    }\n";
    ss << "};\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::transformSelector(const CHTLJSSyntaxAnalyzer::EnhancedSelector& selector) {
    return selector.jsExpression;
}

std::string CHTLJSCodeGenerator::generateEventBinding(const std::string& element, 
                                                    const std::string& event, 
                                                    const std::string& handler) {
    std::stringstream ss;
    
    ss << "// Generated Event Binding\n";
    ss << "(function() {\n";
    ss << "    const element = " << element << ";\n";
    ss << "    if (element) {\n";
    ss << "        element.addEventListener('" << event << "', function() {\n";
    ss << "            " << handler << "\n";
    ss << "        });\n";
    ss << "    }\n";
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::generateModuleImport(const std::string& moduleSpec) {
    std::stringstream ss;
    
    ss << "// Generated Module Import\n";
    ss << "(function() {\n";
    ss << "    // AMD-style module loader implementation\n";
    ss << "    const modules = " << moduleSpec << ";\n";
    ss << "    if (Array.isArray(modules)) {\n";
    ss << "        modules.forEach(function(module) {\n";
    ss << "            if (typeof module === 'string') {\n";
    ss << "                const script = document.createElement('script');\n";
    ss << "                script.src = module;\n";
    ss << "                script.async = false;\n";
    ss << "                document.head.appendChild(script);\n";
    ss << "            }\n";
    ss << "        });\n";
    ss << "    }\n";
    ss << "})();\n";
    
    return ss.str();
}

std::string CHTLJSCodeGenerator::optimizeGeneratedCode(const std::string& code) {
    std::string optimized = code;
    
    // 移除多余的空行
    optimized = std::regex_replace(optimized, std::regex(R"(\n\s*\n\s*\n)"), "\n\n");
    
    // 移除行尾空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+$)"), "");
    
    return optimized;
}

// ========== VirtualObjectManager 实现 ==========

void VirtualObjectManager::registerVirtualObject(const std::string& name, const VirtualObject& obj) {
    m_virtualObjects[name] = obj;
}

VirtualObjectManager::VirtualObject* VirtualObjectManager::getVirtualObject(const std::string& name) {
    auto it = m_virtualObjects.find(name);
    return (it != m_virtualObjects.end()) ? &it->second : nullptr;
}

std::string VirtualObjectManager::resolveVirtualCall(const std::string& call) {
    std::regex callPattern(R"((\w+)->(\w+)(?:\((.*)\))?)");
    std::smatch match;
    
    if (std::regex_match(call, match, callPattern)) {
        std::string objName = match[1].str();
        std::string methodName = match[2].str();
        std::string args = match[3].str();
        
        auto* obj = getVirtualObject(objName);
        if (obj) {
            if (args.empty()) {
                return objName + ".get('" + methodName + "')";
            } else {
                return objName + ".call('" + methodName + "', " + args + ")";
            }
        }
    }
    
    return call;
}

std::string VirtualObjectManager::generateImplementation(const VirtualObject& obj) {
    std::stringstream ss;
    
    ss << "// Virtual Object Implementation: " << obj.name << "\n";
    ss << "const " << obj.name << "_virtual = {\n";
    
    bool first = true;
    for (const auto& [key, value] : obj.methods) {
        if (!first) ss << ",\n";
        ss << "    " << key << ": " << value;
        first = false;
    }
    
    for (const auto& [key, value] : obj.properties) {
        if (!first) ss << ",\n";
        ss << "    " << key << ": " << value;
        first = false;
    }
    
    ss << "\n};\n";
    
    return ss.str();
}

// ========== CHTLJSEnhancedCompiler 实现 ==========

CHTLJSEnhancedCompiler::CHTLJSEnhancedCompiler() 
    : m_debugMode(false) {
    m_analyzer = std::make_unique<CHTLJSSyntaxAnalyzer>();
    m_generator = std::make_unique<CHTLJSCodeGenerator>();
    m_virtualManager = std::make_unique<VirtualObjectManager>();
}

std::string CHTLJSEnhancedCompiler::compile(const std::string& content) {
    updateStats("total_compilations");
    
    std::string processed = preprocessCode(content);
    std::stringstream result;
    
    result << "// CHTL JS Compiled Code\n";
    result << "(function() {\n";
    result << "'use strict';\n\n";
    
    // 编译各种CHTL JS特性
    std::string functions = compileFunction(processed);
    std::string selectors = compileSelectors(processed);
    std::string bindings = compileEventBindings(processed);
    std::string virtuals = compileVirtualObjects(processed);
    std::string modules = compileModuleImports(processed);
    
    if (!functions.empty()) {
        result << functions << "\n";
    }
    
    if (!selectors.empty()) {
        result << selectors << "\n";
    }
    
    if (!bindings.empty()) {
        result << bindings << "\n";
    }
    
    if (!virtuals.empty()) {
        result << virtuals << "\n";
    }
    
    if (!modules.empty()) {
        result << modules << "\n";
    }
    
    result << "})();\n";
    
    return postprocessCode(result.str());
}

std::string CHTLJSEnhancedCompiler::compileFunction(const std::string& content) {
    std::stringstream result;
    
    // 查找所有CHTL JS函数
    std::regex funcPattern(R"((listen|delegate|animate)\s*\{[^}]*\})");
    std::sregex_iterator iter(content.begin(), content.end(), funcPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string funcContent = match[0].str();
        
        auto parsedFunc = m_analyzer->parseFunction(funcContent);
        
        if (parsedFunc.type == "listen") {
            result << m_generator->generateEventListener(parsedFunc) << "\n";
            updateStats("listen_functions");
        } else if (parsedFunc.type == "delegate") {
            result << m_generator->generateEventDelegate(parsedFunc) << "\n";
            updateStats("delegate_functions");
        } else if (parsedFunc.type == "animate") {
            result << m_generator->generateAnimation(parsedFunc) << "\n";
            updateStats("animate_functions");
        }
    }
    
    return result.str();
}

std::string CHTLJSEnhancedCompiler::compileSelectors(const std::string& content) {
    std::stringstream result;
    
    auto selectors = m_analyzer->parseSelectors(content);
    
    if (!selectors.empty()) {
        result << "// Enhanced Selector Transformations\n";
        
        for (const auto& selector : selectors) {
            std::string transformed = m_generator->transformSelector(selector);
            // 这里可以添加选择器缓存或其他优化
            updateStats("selectors_transformed");
        }
    }
    
    return result.str();
}

std::string CHTLJSEnhancedCompiler::compileEventBindings(const std::string& content) {
    std::stringstream result;
    
    std::string element, event, handler;
    if (m_analyzer->parseEventBinding(content, element, event, handler)) {
        result << m_generator->generateEventBinding(element, event, handler) << "\n";
        updateStats("event_bindings");
    }
    
    return result.str();
}

std::string CHTLJSEnhancedCompiler::compileVirtualObjects(const std::string& content) {
    std::stringstream result;
    
    if (m_analyzer->isVirtualObjectDeclaration(content)) {
        auto virtualObj = m_analyzer->parseVirtualObject(content);
        result << m_generator->generateVirtualObject(virtualObj) << "\n";
        updateStats("virtual_objects");
    }
    
    return result.str();
}

std::string CHTLJSEnhancedCompiler::compileModuleImports(const std::string& content) {
    std::stringstream result;
    
    std::regex modulePattern(R"(module\s*\{([^}]*)\})");
    std::smatch match;
    
    if (std::regex_search(content, match, modulePattern)) {
        std::string moduleSpec = match[1].str();
        result << m_generator->generateModuleImport(moduleSpec) << "\n";
        updateStats("module_imports");
    }
    
    return result.str();
}

void CHTLJSEnhancedCompiler::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

std::string CHTLJSEnhancedCompiler::getCompilationStats() const {
    std::stringstream ss;
    
    ss << "CHTL JS Compilation Statistics:\n";
    ss << "==============================\n";
    
    for (const auto& [category, count] : m_stats) {
        ss << category << ": " << count << "\n";
    }
    
    return ss.str();
}

void CHTLJSEnhancedCompiler::cleanup() {
    m_stats.clear();
}

std::string CHTLJSEnhancedCompiler::preprocessCode(const std::string& content) {
    std::string processed = content;
    
    // 移除多余的空白
    processed = std::regex_replace(processed, std::regex(R"(\s+)"), " ");
    
    // 标准化换行
    processed = std::regex_replace(processed, std::regex(R"(\r\n|\r)"), "\n");
    
    return processed;
}

std::string CHTLJSEnhancedCompiler::postprocessCode(const std::string& code) {
    return m_generator->optimizeGeneratedCode(code);
}

void CHTLJSEnhancedCompiler::updateStats(const std::string& category) {
    m_stats[category]++;
}

void CHTLJSEnhancedCompiler::reportError(const std::string& message) {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, "CHTL JS Compiler: " + message);
}

} // namespace CHTL