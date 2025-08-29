#include "Generator.h"
#include "../CHTLJSNode/ModuleNode.h"
#include <regex>
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace JS {

// CHTLJSGeneratorContext实现
CHTLJSGeneratorContext::CHTLJSGeneratorContext() : uniqueIdCounter(0) {
    enterScope("global");
}

void CHTLJSGeneratorContext::enterScope(const String& scopeName) {
    String actualScopeName = scopeName.empty() ? "scope_" + std::to_string(scopes.size()) : scopeName;
    scopes.push_back(actualScopeName);
}

void CHTLJSGeneratorContext::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

String CHTLJSGeneratorContext::getCurrentScope() const {
    return scopes.empty() ? "" : scopes.back();
}

size_t CHTLJSGeneratorContext::getScopeDepth() const {
    return scopes.size();
}

void CHTLJSGeneratorContext::declareVariable(const String& name, const String& type) {
    variables[name] = getCurrentScope() + "::" + type;
}

bool CHTLJSGeneratorContext::isVariableDeclared(const String& name) const {
    return variables.find(name) != variables.end();
}

String CHTLJSGeneratorContext::getVariableScope(const String& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        size_t pos = it->second.find("::");
        return pos != String::npos ? it->second.substr(0, pos) : "";
    }
    return "";
}

void CHTLJSGeneratorContext::declareFunction(const String& name, const StringVector& params) {
    functions[name] = params;
}

bool CHTLJSGeneratorContext::isFunctionDeclared(const String& name) const {
    return functions.find(name) != functions.end();
}

StringVector CHTLJSGeneratorContext::getFunctionParams(const String& name) const {
    auto it = functions.find(name);
    return it != functions.end() ? it->second : StringVector{};
}

void CHTLJSGeneratorContext::registerEventHandler(const String& selector, const String& event, const String& handler) {
    eventHandlers[selector][event] = handler;
}

StringVector CHTLJSGeneratorContext::getEventHandlers(const String& selector) const {
    StringVector handlers;
    auto it = eventHandlers.find(selector);
    if (it != eventHandlers.end()) {
        for (const auto& pair : it->second) {
            handlers.push_back(pair.first + ":" + pair.second);
        }
    }
    return handlers;
}

void CHTLJSGeneratorContext::registerModule(const String& name, const StringVector& exports) {
    modules[name] = exports;
}

StringVector CHTLJSGeneratorContext::getModuleExports(const String& name) const {
    auto it = modules.find(name);
    return it != modules.end() ? it->second : StringVector{};
}

bool CHTLJSGeneratorContext::isModuleRegistered(const String& name) const {
    return modules.find(name) != modules.end();
}

String CHTLJSGeneratorContext::generateUniqueId(const String& prefix) {
    return prefix + "_" + std::to_string(uniqueIdCounter++);
}

void CHTLJSGeneratorContext::addDependency(const String& dependency) {
    if (std::find(dependencies.begin(), dependencies.end(), dependency) == dependencies.end()) {
        dependencies.push_back(dependency);
    }
}

StringVector CHTLJSGeneratorContext::getDependencies() const {
    return dependencies;
}

// CHTLJSGenerator实现
CHTLJSGenerator::CHTLJSGenerator(const CHTLJSGeneratorConfig& config)
    : config(config), indentLevel(0) {
    initializeGenerators();
}

String CHTLJSGenerator::generate(const std::shared_ptr<BaseNode>& node) {
    if (!node) {
        addError("空节点传入生成器");
        return "";
    }
    
    try {
        String result = generateNode(node);
        
        if (config.enableOptimization) {
            result = optimizeCode(result);
        }
        
        if (config.minifyOutput) {
            result = minifyCode(result);
        }
        
        if (!config.preserveComments) {
            // 移除注释的逻辑
            result = std::regex_replace(result, std::regex(R"(//[^\n]*\n)"), "\n");
            result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
        }
        
        if (config.generateSourceMap) {
            result = addSourceMap(result);
        }
        
        return result;
        
    } catch (const std::exception& e) {
        addError("代码生成异常: " + String(e.what()));
        return "";
    }
}

String CHTLJSGenerator::generateFromTokens(const std::vector<CHTLJSToken>& tokens) {
    String result;
    
    for (const auto& token : tokens) {
        switch (token.type) {
            case CHTLJSTokenType::LISTEN:
                result += generateListenTemplate();
                break;
            case CHTLJSTokenType::DELEGATE:
                result += generateDelegateTemplate();
                break;
            case CHTLJSTokenType::ANIMATE:
                result += generateAnimateTemplate();
                break;
            case CHTLJSTokenType::VIR:
                result += generateVirTemplate();
                break;
            case CHTLJSTokenType::MODULE:
                result += generateModuleTemplate();
                break;
            case CHTLJSTokenType::SELECTOR_START:
                result += translateSelector(token.value);
                break;
            case CHTLJSTokenType::EVENT_BIND:
                result += " addEventListener(";
                break;
            default:
                result += token.value;
                break;
        }
    }
    
    return result;
}

String CHTLJSGenerator::generateListen(const std::shared_ptr<CHTLJSListenNode>& node) {
    if (!node) return "";
    
    String result = generateIndent() + "// Listen block" + generateNewline();
    String target = translateSelector(node->getTarget());
    
    result += generateIndent() + "const target = " + target + ";" + generateNewline();
    
    auto events = node->getEvents();
    for (const auto& event : events) {
        String eventName = event.first;
        String handler = event.second;
        
        result += generateIndent() + "target.addEventListener('" + eventName + "', " + handler + ");" + generateNewline();
        context.registerEventHandler(node->getTarget(), eventName, handler);
    }
    
    return result;
}

String CHTLJSGenerator::generateDelegate(const std::shared_ptr<CHTLJSDelegateNode>& node) {
    if (!node) return "";
    
    String result = generateIndent() + "// Delegate block" + generateNewline();
    String parent = translateSelector(node->getParent());
    String target = node->getTarget();
    
    result += generateIndent() + "const parentElement = " + parent + ";" + generateNewline();
    
    auto events = node->getEvents();
    for (const auto& event : events) {
        String eventName = event.first;
        String handler = event.second;
        
        result += generateIndent() + "parentElement.addEventListener('" + eventName + "', function(e) {" + generateNewline();
        indentLevel++;
        result += generateIndent() + "if (e.target.matches('" + target + "')) {" + generateNewline();
        indentLevel++;
        result += generateIndent() + "(" + handler + ").call(e.target, e);" + generateNewline();
        indentLevel--;
        result += generateIndent() + "}" + generateNewline();
        indentLevel--;
        result += generateIndent() + "});" + generateNewline();
    }
    
    return result;
}

String CHTLJSGenerator::generateAnimate(const std::shared_ptr<CHTLJSAnimateNode>& node) {
    if (!node) return "";
    
    String result = generateIndent() + "// Animate block" + generateNewline();
    String target = translateSelector(node->getTarget());
    
    result += generateIndent() + "const animationTarget = " + target + ";" + generateNewline();
    result += generateIndent() + "const animation = {" + generateNewline();
    indentLevel++;
    
    auto properties = node->getProperties();
    for (const auto& prop : properties) {
        result += generateIndent() + prop.first + ": " + prop.second + "," + generateNewline();
    }
    
    indentLevel--;
    result += generateIndent() + "};" + generateNewline();
    
    // 生成动画执行代码
    result += generateIndent() + "animationTarget.animate([" + generateNewline();
    indentLevel++;
    result += generateIndent() + "animation.begin || {}," + generateNewline();
    result += generateIndent() + "animation.end || {}" + generateNewline();
    indentLevel--;
    result += generateIndent() + "], {" + generateNewline();
    indentLevel++;
    result += generateIndent() + "duration: animation.duration || 1000," + generateNewline();
    result += generateIndent() + "easing: animation.easing || 'ease'" + generateNewline();
    indentLevel--;
    result += generateIndent() + "});" + generateNewline();
    
    return result;
}

String CHTLJSGenerator::generateVir(const std::shared_ptr<CHTLJSVirNode>& node) {
    if (!node) return "";
    
    String result = generateIndent() + "// Virtual object: " + node->getName() + generateNewline();
    String name = node->getName();
    
    result += generateIndent() + "const " + name + " = {" + generateNewline();
    indentLevel++;
    
    auto properties = node->getProperties();
    for (const auto& prop : properties) {
        result += generateIndent() + prop.first + ": " + prop.second + "," + generateNewline();
    }
    
    indentLevel--;
    result += generateIndent() + "};" + generateNewline();
    
    // 注册虚对象到上下文
    StringVector props;
    for (const auto& prop : properties) {
        props.push_back(prop.first);
    }
    context.registerModule(name, props);
    
    return result;
}

String CHTLJSGenerator::generateModule(const std::shared_ptr<ModuleNode>& node) {
    if (!node) return "";
    
    String result = generateIndent() + "// Module: " + node->getModuleName() + generateNewline();
    
    // 生成AMD模块包装器
    StringVector dependencies = node->getDependencies();
    String dependencyList = "[";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        dependencyList += "'" + dependencies[i] + "'";
        if (i < dependencies.size() - 1) dependencyList += ", ";
    }
    dependencyList += "]";
    
    result += generateIndent() + "define(" + dependencyList + ", function(";
    for (size_t i = 0; i < dependencies.size(); ++i) {
        result += "dep" + std::to_string(i);
        if (i < dependencies.size() - 1) result += ", ";
    }
    result += ") {" + generateNewline();
    
    indentLevel++;
    
    // 生成模块内容
    String content = node->getContent();
    std::istringstream iss(content);
    String line;
    while (std::getline(iss, line)) {
        result += generateIndent() + line + generateNewline();
    }
    
    // 生成导出
    auto exports = node->getExports();
    if (!exports.empty()) {
        result += generateNewline() + generateIndent() + "return {" + generateNewline();
        indentLevel++;
        for (size_t i = 0; i < exports.size(); ++i) {
            result += generateIndent() + exports[i] + ": " + exports[i];
            if (i < exports.size() - 1) result += ",";
            result += generateNewline();
        }
        indentLevel--;
        result += generateIndent() + "};" + generateNewline();
    }
    
    indentLevel--;
    result += generateIndent() + "});" + generateNewline();
    
    return result;
}

String CHTLJSGenerator::generateEventBinding(const std::shared_ptr<CHTLJSEventBindingNode>& node) {
    if (!node) return "";
    
    String result;
    String selector = translateSelector(node->getSelector());
    String event = node->getEvent();
    String handler = node->getHandler();
    
    result += generateIndent() + selector + ".addEventListener('" + event + "', " + handler + ");" + generateNewline();
    
    return result;
}

String CHTLJSGenerator::generateSelector(const std::shared_ptr<CHTLJSSelectorNode>& node) {
    if (!node) return "";
    
    return translateSelector(node->getSelector());
}

String CHTLJSGenerator::translateSelector(const String& selector) {
    if (selector.empty()) return "document";
    
    // 移除 {{ }} 包装
    String cleanSelector = selector;
    if (cleanSelector.substr(0, 2) == "{{" && cleanSelector.substr(cleanSelector.length() - 2) == "}}") {
        cleanSelector = cleanSelector.substr(2, cleanSelector.length() - 4);
    }
    
    // 特殊选择器处理
    if (cleanSelector == "body") return "document.body";
    if (cleanSelector == "html") return "document.documentElement";
    if (cleanSelector == "head") return "document.head";
    
    // ID选择器优化
    if (cleanSelector.length() > 1 && cleanSelector[0] == '#') {
        String id = cleanSelector.substr(1);
        return "document.getElementById('" + id + "')";
    }
    
    // 通用查询选择器
    return "document.querySelector('" + cleanSelector + "')";
}

String CHTLJSGenerator::generateIndent() const {
    String indent;
    for (size_t i = 0; i < indentLevel; ++i) {
        indent += config.indentString;
    }
    return indent;
}

String CHTLJSGenerator::generateNewline() const {
    return config.newlineString;
}

String CHTLJSGenerator::optimizeCode(const String& code) {
    String optimized = code;
    
    // 移除空行
    optimized = std::regex_replace(optimized, std::regex(R"(\n\s*\n)"), "\n");
    
    // 优化变量声明
    optimized = optimizeVariableDeclarations(optimized);
    
    return optimized;
}

String CHTLJSGenerator::minifyCode(const String& code) {
    String minified = code;
    
    // 移除多余空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除分号前的空格
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    
    // 移除逗号后多余空格
    minified = std::regex_replace(minified, std::regex(R"(\s*,\s*)"), ",");
    
    return minified;
}

String CHTLJSGenerator::addSourceMap(const String& code) {
    // 简化的源映射添加
    return code + "\n//# sourceMappingURL=generated.js.map";
}

String CHTLJSGenerator::optimizeVariableDeclarations(const String& code) {
    // 简化的变量声明优化
    String optimized = std::regex_replace(code, std::regex(R"(var\s+(\w+);\s*\1\s*=)"), "var $1 =");
    return optimized;
}

void CHTLJSGenerator::addError(const String& message) {
    errors.push_back(message);
}

void CHTLJSGenerator::addWarning(const String& message) {
    warnings.push_back(message);
}

const StringVector& CHTLJSGenerator::getErrors() const {
    return errors;
}

const StringVector& CHTLJSGenerator::getWarnings() const {
    return warnings;
}

bool CHTLJSGenerator::hasErrors() const {
    return !errors.empty();
}

String CHTLJSGenerator::getErrorSummary() const {
    if (errors.empty()) return "没有错误";
    
    String summary = "代码生成错误 (" + std::to_string(errors.size()) + " 个):\n";
    for (size_t i = 0; i < errors.size(); ++i) {
        summary += std::to_string(i + 1) + ". " + errors[i] + "\n";
    }
    return summary;
}

void CHTLJSGenerator::clearErrors() {
    errors.clear();
    warnings.clear();
}

String CHTLJSGenerator::generateNode(const std::shared_ptr<BaseNode>& node) {
    if (!node) return "";
    
    // 这里需要根据节点类型调用相应的生成方法
    // 由于我们还没有完整的节点系统，这里提供基础实现
    return "// Generated from node of type: " + node->getNodeTypeName() + "\n";
}

void CHTLJSGenerator::initializeGenerators() {
    // 初始化各种节点生成器
    setupDefaultGenerators();
}

void CHTLJSGenerator::setupDefaultGenerators() {
    // 设置默认的节点生成器
    // 这里可以注册各种节点类型的生成函数
}

// 生成模板方法（简化实现）
String CHTLJSGenerator::generateListenTemplate() {
    return "/* Listen template */";
}

String CHTLJSGenerator::generateDelegateTemplate() {
    return "/* Delegate template */";
}

String CHTLJSGenerator::generateAnimateTemplate() {
    return "/* Animate template */";
}

String CHTLJSGenerator::generateVirTemplate() {
    return "/* Vir template */";
}

String CHTLJSGenerator::generateModuleTemplate() {
    return "/* Module template */";
}

// CHTLJSGeneratorFactory实现
std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createGenerator(const CHTLJSGeneratorConfig& config) {
    return std::make_unique<CHTLJSGenerator>(config);
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createMinifyingGenerator() {
    return createGenerator(getMinifyConfig());
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createDebugGenerator() {
    return createGenerator(getDebugConfig());
}

std::unique_ptr<CHTLJSGenerator> CHTLJSGeneratorFactory::createProductionGenerator() {
    return createGenerator(getProductionConfig());
}

CHTLJSGeneratorConfig CHTLJSGeneratorFactory::getDefaultConfig() {
    return CHTLJSGeneratorConfig{};
}

CHTLJSGeneratorConfig CHTLJSGeneratorFactory::getMinifyConfig() {
    CHTLJSGeneratorConfig config;
    config.minifyOutput = true;
    config.preserveComments = false;
    config.enableOptimization = true;
    return config;
}

CHTLJSGeneratorConfig CHTLJSGeneratorFactory::getDebugConfig() {
    CHTLJSGeneratorConfig config;
    config.enableDebugInfo = true;
    config.preserveComments = true;
    config.generateSourceMap = true;
    config.minifyOutput = false;
    return config;
}

CHTLJSGeneratorConfig CHTLJSGeneratorFactory::getProductionConfig() {
    CHTLJSGeneratorConfig config;
    config.minifyOutput = true;
    config.enableOptimization = true;
    config.preserveComments = false;
    config.strictMode = true;
    return config;
}

// CHTLJSTemplateManager实现
std::unordered_map<String, String> CHTLJSTemplateManager::templates;

void CHTLJSTemplateManager::registerTemplate(const String& name, const String& template_) {
    templates[name] = template_;
}

String CHTLJSTemplateManager::getTemplate(const String& name) {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : "";
}

bool CHTLJSTemplateManager::hasTemplate(const String& name) {
    return templates.find(name) != templates.end();
}

String CHTLJSTemplateManager::fillTemplate(const String& template_, const std::unordered_map<String, String>& placeholders) {
    String result = template_;
    
    for (const auto& placeholder : placeholders) {
        String pattern = "${" + placeholder.first + "}";
        size_t pos = 0;
        while ((pos = result.find(pattern, pos)) != String::npos) {
            result.replace(pos, pattern.length(), placeholder.second);
            pos += placeholder.second.length();
        }
    }
    
    return result;
}

String CHTLJSTemplateManager::fillTemplate(const String& name, const std::unordered_map<String, String>& placeholders) {
    String template_ = getTemplate(name);
    return template_.empty() ? "" : fillTemplate(template_, placeholders);
}

void CHTLJSTemplateManager::initializeDefaultTemplates() {
    // 注册默认模板
    registerTemplate("listen", R"(
${target}.addEventListener('${event}', ${handler});
)");
    
    registerTemplate("delegate", R"(
${parent}.addEventListener('${event}', function(e) {
    if (e.target.matches('${target}')) {
        (${handler}).call(e.target, e);
    }
});
)");
    
    registerTemplate("animate", R"(
${target}.animate([
    ${begin},
    ${end}
], {
    duration: ${duration},
    easing: '${easing}'
});
)");
    
    registerTemplate("module", R"(
define(${dependencies}, function(${params}) {
    ${content}
    return ${exports};
});
)");
}

} // namespace JS
} // namespace CHTL