#include "VariableNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <stack>

namespace CHTL {

// ========== VariableTemplateNode 实现 ==========

VariableTemplateNode::VariableTemplateNode(const String& name, const Position& pos)
    : BaseNode(NodeType::TEMPLATE, name, pos), templateName(name), 
      scope(VariableScope::TEMPLATE), isCompileTime(true) {
}

void VariableTemplateNode::addVariable(const String& name, const String& value, VariableType type) {
    if (isValidVariableName(name) && isValidVariableValue(value)) {
        variables[name] = value;
        variableTypes[name] = type;
        
        // 自动检测变量类型
        if (type == VariableType::SIMPLE) {
            variableTypes[name] = parseVariableType(value);
        }
    }
}

void VariableTemplateNode::setVariableType(const String& name, VariableType type) {
    if (hasVariable(name)) {
        variableTypes[name] = type;
    }
}

void VariableTemplateNode::setDefaultValue(const String& name, const String& defaultVal) {
    defaultValues[name] = defaultVal;
}

void VariableTemplateNode::setDescription(const String& name, const String& desc) {
    descriptions[name] = desc;
}

void VariableTemplateNode::addDependency(const String& varName, const String& dependency) {
    if (hasVariable(varName)) {
        dependencies[varName].push_back(dependency);
    }
}

bool VariableTemplateNode::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

String VariableTemplateNode::getVariable(const String& name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    
    // 返回默认值
    auto defaultIt = defaultValues.find(name);
    return defaultIt != defaultValues.end() ? defaultIt->second : "";
}

VariableType VariableTemplateNode::getVariableType(const String& name) const {
    auto it = variableTypes.find(name);
    return it != variableTypes.end() ? it->second : VariableType::SIMPLE;
}

String VariableTemplateNode::getDefaultValue(const String& name) const {
    auto it = defaultValues.find(name);
    return it != defaultValues.end() ? it->second : "";
}

StringVector VariableTemplateNode::getDependencies(const String& name) const {
    auto it = dependencies.find(name);
    return it != dependencies.end() ? it->second : StringVector{};
}

StringVector VariableTemplateNode::getAllVariableNames() const {
    StringVector names;
    for (const auto& [name, value] : variables) {
        names.push_back(name);
    }
    return names;
}

String VariableTemplateNode::resolveVariable(const String& name, const std::unordered_map<String, String>& context) const {
    if (!hasVariable(name)) {
        return getDefaultValue(name);
    }
    
    VariableType type = getVariableType(name);
    
    switch (type) {
        case VariableType::SIMPLE:
            return resolveSimpleVariable(name, context);
        case VariableType::FUNCTION:
            return resolveFunctionVariable(name, context);
        case VariableType::REFERENCE:
            return resolveReferenceVariable(name, context);
        case VariableType::CONDITIONAL:
            return resolveConditionalVariable(name, context);
        case VariableType::COMPUTED:
            return resolveComputedVariable(name, context);
        default:
            return getVariable(name);
    }
}

String VariableTemplateNode::substituteVariables(const String& expression, const std::unordered_map<String, String>& context) const {
    String result = expression;
    
    // 替换所有变量引用 {{varName}}
    std::regex varRegex(R"(\{\{(\w+)\}\})");
    std::smatch match;
    
    while (std::regex_search(result, match, varRegex)) {
        String varName = match[1].str();
        String resolvedValue = resolveVariable(varName, context);
        result = std::regex_replace(result, std::regex("\\{\\{" + varName + "\\}\\}"), resolvedValue);
    }
    
    return result;
}

std::unordered_map<String, String> VariableTemplateNode::getAllResolvedVariables(const std::unordered_map<String, String>& context) const {
    std::unordered_map<String, String> resolved;
    
    for (const auto& [name, value] : variables) {
        resolved[name] = resolveVariable(name, context);
    }
    
    return resolved;
}

bool VariableTemplateNode::validateVariables() const {
    // 检查循环依赖
    if (hasCircularDependencies()) {
        return false;
    }
    
    // 检查变量名和值的有效性
    for (const auto& [name, value] : variables) {
        if (!isValidVariableName(name) || !isValidVariableValue(value)) {
            return false;
        }
    }
    
    return true;
}

StringVector VariableTemplateNode::getValidationErrors() const {
    StringVector errors;
    
    // 检查循环依赖
    if (hasCircularDependencies()) {
        errors.push_back("检测到变量循环依赖");
    }
    
    // 检查无效的变量名
    for (const auto& [name, value] : variables) {
        if (!isValidVariableName(name)) {
            errors.push_back("无效的变量名: " + name);
        }
        if (!isValidVariableValue(value)) {
            errors.push_back("无效的变量值: " + name + " = " + value);
        }
    }
    
    return errors;
}

void VariableTemplateNode::optimizeVariableReferences() {
    inlineSimpleReferences();
    eliminateUnusedVariables();
}

bool VariableTemplateNode::hasCircularDependencies() const {
    StringUnorderedSet visited;
    StringUnorderedSet recursionStack;
    
    for (const auto& [varName, deps] : dependencies) {
        if (hasCyclicDependency(varName, visited, recursionStack)) {
            return true;
        }
    }
    
    return false;
}

String VariableTemplateNode::getValue() const {
    std::ostringstream oss;
    oss << "[Template] @Var " << templateName << " {\n";
    
    for (const auto& [name, value] : variables) {
        oss << "    " << name << ": \"" << value << "\";\n";
    }
    
    oss << "}";
    return oss.str();
}

std::unique_ptr<BaseNode> VariableTemplateNode::clone() const {
    auto cloned = std::make_unique<VariableTemplateNode>(templateName, position);
    cloned->variables = variables;
    cloned->variableTypes = variableTypes;
    cloned->defaultValues = defaultValues;
    cloned->descriptions = descriptions;
    cloned->dependencies = dependencies;
    cloned->scope = scope;
    cloned->isCompileTime = isCompileTime;
    return cloned;
}

std::unique_ptr<VariableTemplateNode> VariableTemplateNode::createVariableTemplate(const String& name, const Position& pos) {
    return std::make_unique<VariableTemplateNode>(name, pos);
}

VariableType VariableTemplateNode::parseVariableType(const String& value) {
    if (value.find("calc(") == 0 || value.find("@function(") == 0) {
        return VariableType::FUNCTION;
    }
    if (value.find("@") == 0) {
        return VariableType::REFERENCE;
    }
    if (value.find("@if(") == 0) {
        return VariableType::CONDITIONAL;
    }
    if (value.find("@computed(") == 0) {
        return VariableType::COMPUTED;
    }
    return VariableType::SIMPLE;
}

std::pair<String, StringVector> VariableTemplateNode::parseVariableFunction(const String& functionCall) {
    std::regex funcRegex(R"((\w+)\s*\(([^)]*)\))");
    std::smatch match;
    
    if (std::regex_match(functionCall, match, funcRegex)) {
        String funcName = match[1].str();
        String argsStr = match[2].str();
        
        StringVector args;
        std::istringstream iss(argsStr);
        String arg;
        
        while (std::getline(iss, arg, ',')) {
            arg.erase(0, arg.find_first_not_of(" \t"));
            arg.erase(arg.find_last_not_of(" \t") + 1);
            if (!arg.empty()) {
                args.push_back(arg);
            }
        }
        
        return {funcName, args};
    }
    
    return {"", {}};
}

bool VariableTemplateNode::isVariableReference(const String& value) {
    return value.find("@") == 0 && value.find("(") == String::npos;
}

bool VariableTemplateNode::isComputedVariable(const String& value) {
    return value.find("@computed(") == 0;
}

// 内部辅助方法实现
String VariableTemplateNode::resolveSimpleVariable(const String& name, const std::unordered_map<String, String>& context) const {
    String value = getVariable(name);
    
    // 检查上下文覆盖
    auto contextIt = context.find(name);
    if (contextIt != context.end()) {
        return contextIt->second;
    }
    
    return value;
}

String VariableTemplateNode::resolveFunctionVariable(const String& name, const std::unordered_map<String, String>& context) const {
    String value = getVariable(name);
    
    // 解析函数调用
    auto [funcName, args] = parseVariableFunction(value);
    
    if (funcName == "calc") {
        // CSS calc函数
        return "calc(" + args[0] + ")";
    } else if (funcName == "rgb") {
        // RGB颜色函数
        if (args.size() >= 3) {
            return "rgb(" + args[0] + ", " + args[1] + ", " + args[2] + ")";
        }
    }
    
    return value;
}

String VariableTemplateNode::resolveReferenceVariable(const String& name, const std::unordered_map<String, String>& context) const {
    String value = getVariable(name);
    
    if (value.find("@") == 0) {
        String refName = value.substr(1); // 移除@前缀
        
        // 检查上下文中的引用
        auto contextIt = context.find(refName);
        if (contextIt != context.end()) {
            return contextIt->second;
        }
        
        // 检查本模板中的其他变量
        if (hasVariable(refName)) {
            return resolveVariable(refName, context);
        }
    }
    
    return value;
}

String VariableTemplateNode::resolveConditionalVariable(const String& name, const std::unordered_map<String, String>& context) const {
    String value = getVariable(name);
    
    // 解析 @if(condition, trueValue, falseValue)
    std::regex ifRegex(R"(@if\s*\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\))");
    std::smatch match;
    
    if (std::regex_match(value, match, ifRegex)) {
        String condition = match[1].str();
        String trueValue = match[2].str();
        String falseValue = match[3].str();
        
        // 简单条件评估（可以扩展）
        bool conditionResult = (condition == "true" || condition == "1");
        
        return conditionResult ? trueValue : falseValue;
    }
    
    return value;
}

String VariableTemplateNode::resolveComputedVariable(const String& name, const std::unordered_map<String, String>& context) const {
    String value = getVariable(name);
    
    // 解析 @computed(expression)
    std::regex computedRegex(R"(@computed\s*\(\s*([^)]+)\s*\))");
    std::smatch match;
    
    if (std::regex_match(value, match, computedRegex)) {
        String expression = match[1].str();
        
        // 替换表达式中的变量
        return substituteVariables(expression, context);
    }
    
    return value;
}

bool VariableTemplateNode::hasCyclicDependency(const String& varName, StringUnorderedSet& visited, StringUnorderedSet& recursionStack) const {
    visited.insert(varName);
    recursionStack.insert(varName);
    
    auto it = dependencies.find(varName);
    if (it != dependencies.end()) {
        for (const String& dep : it->second) {
            if (recursionStack.find(dep) != recursionStack.end()) {
                return true; // 找到循环
            }
            if (visited.find(dep) == visited.end()) {
                if (hasCyclicDependency(dep, visited, recursionStack)) {
                    return true;
                }
            }
        }
    }
    
    recursionStack.erase(varName);
    return false;
}

void VariableTemplateNode::inlineSimpleReferences() {
    // 内联简单的变量引用
    for (auto& [name, value] : variables) {
        if (getVariableType(name) == VariableType::REFERENCE) {
            String refName = value.substr(1); // 移除@前缀
            if (hasVariable(refName) && getVariableType(refName) == VariableType::SIMPLE) {
                value = getVariable(refName);
                variableTypes[name] = VariableType::SIMPLE;
            }
        }
    }
}

void VariableTemplateNode::eliminateUnusedVariables() {
    // 标记被使用的变量
    StringUnorderedSet used;
    
    // 检查变量依赖
    for (const auto& [varName, deps] : dependencies) {
        for (const String& dep : deps) {
            used.insert(dep);
        }
    }
    
    // 移除未使用的变量（保留至少一个）
    if (used.size() < variables.size() && variables.size() > 1) {
        auto it = variables.begin();
        while (it != variables.end()) {
            if (used.find(it->first) == used.end()) {
                variableTypes.erase(it->first);
                defaultValues.erase(it->first);
                descriptions.erase(it->first);
                dependencies.erase(it->first);
                it = variables.erase(it);
                break; // 一次只移除一个
            } else {
                ++it;
            }
        }
    }
}

bool VariableTemplateNode::isValidVariableName(const String& name) const {
    return !name.empty() && 
           name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") == String::npos;
}

bool VariableTemplateNode::isValidVariableValue(const String& value) const {
    return !value.empty();
}

// ========== VariableSpecializationCallNode 实现 ==========

VariableSpecializationCallNode::VariableSpecializationCallNode(const String& templateName, const String& varName, const Position& pos)
    : BaseNode(NodeType::TEMPLATE, templateName + "(" + varName + ")", pos),
      templateName(templateName), variableName(varName), isGlobalCall(false), isChainedCall(false) {
}

void VariableSpecializationCallNode::addSpecialization(const String& paramName, const String& value) {
    if (isValidParameterName(paramName) && isValidParameterValue(value)) {
        specializations[paramName] = value;
    }
}

void VariableSpecializationCallNode::setSpecializations(const std::unordered_map<String, String>& specs) {
    specializations.clear();
    for (const auto& [name, value] : specs) {
        addSpecialization(name, value);
    }
}

void VariableSpecializationCallNode::addContextOverride(const String& contextVar, const String& value) {
    contextOverrides[contextVar] = value;
}

bool VariableSpecializationCallNode::hasSpecialization(const String& paramName) const {
    return specializations.find(paramName) != specializations.end();
}

String VariableSpecializationCallNode::getSpecialization(const String& paramName) const {
    auto it = specializations.find(paramName);
    return it != specializations.end() ? it->second : "";
}

String VariableSpecializationCallNode::resolveCall(const VariableTemplateNode* templateNode) const {
    if (!templateNode || !templateNode->hasVariable(variableName)) {
        return "";
    }
    
    VariableType varType = templateNode->getVariableType(variableName);
    
    switch (varType) {
        case VariableType::SIMPLE:
            return resolveSimpleCall(templateNode);
        case VariableType::CONDITIONAL:
            return resolveConditionalCall(templateNode);
        case VariableType::COMPUTED:
            return resolveComputedCall(templateNode);
        default:
            return templateNode->resolveVariable(variableName, specializations);
    }
}

String VariableSpecializationCallNode::generateCallExpression() const {
    std::ostringstream oss;
    oss << templateName << "(" << variableName;
    
    if (!specializations.empty()) {
        for (const auto& [name, value] : specializations) {
            oss << " " << name << " = " << value;
        }
    }
    
    oss << ")";
    return oss.str();
}

String VariableSpecializationCallNode::generateCSSVariableCall() const {
    if (specializations.empty()) {
        return "var(--" + templateName + "-" + variableName + ")";
    } else {
        auto it = specializations.begin();
        return "var(--" + templateName + "-" + variableName + ", " + it->second + ")";
    }
}

String VariableSpecializationCallNode::generateSassVariableCall() const {
    return "$" + templateName + "-" + variableName;
}

bool VariableSpecializationCallNode::validateCall(const VariableTemplateNode* templateNode) const {
    if (!templateNode) {
        return false;
    }
    
    // 检查变量是否存在
    if (!templateNode->hasVariable(variableName)) {
        return false;
    }
    
    // 检查参数有效性
    for (const auto& [name, value] : specializations) {
        if (!isValidParameterName(name) || !isValidParameterValue(value)) {
            return false;
        }
    }
    
    return true;
}

StringVector VariableSpecializationCallNode::getCallValidationErrors(const VariableTemplateNode* templateNode) const {
    StringVector errors;
    
    if (!templateNode) {
        errors.push_back("模板节点为空");
        return errors;
    }
    
    if (!templateNode->hasVariable(variableName)) {
        errors.push_back("变量不存在: " + variableName);
    }
    
    for (const auto& [name, value] : specializations) {
        if (!isValidParameterName(name)) {
            errors.push_back("无效的参数名: " + name);
        }
        if (!isValidParameterValue(value)) {
            errors.push_back("无效的参数值: " + name + " = " + value);
        }
    }
    
    return errors;
}

String VariableSpecializationCallNode::getValue() const {
    return generateCallExpression();
}

std::unique_ptr<BaseNode> VariableSpecializationCallNode::clone() const {
    auto cloned = std::make_unique<VariableSpecializationCallNode>(templateName, variableName, position);
    cloned->specializations = specializations;
    cloned->contextOverrides = contextOverrides;
    cloned->isGlobalCall = isGlobalCall;
    cloned->isChainedCall = isChainedCall;
    return cloned;
}

std::unique_ptr<VariableSpecializationCallNode> VariableSpecializationCallNode::createCall(
    const String& templateName, const String& varName, const Position& pos) {
    return std::make_unique<VariableSpecializationCallNode>(templateName, varName, pos);
}

std::tuple<String, String, std::unordered_map<String, String>> VariableSpecializationCallNode::parseSpecializationCall(const String& callExpression) {
    // 解析 ThemeColor(tableColor = rgb(145, 155, 200)) 格式
    std::regex callRegex(R"((\w+)\s*\(\s*(\w+)(?:\s+(.+))?\s*\))");
    std::smatch match;
    
    if (std::regex_match(callExpression, match, callRegex)) {
        String templateName = match[1].str();
        String varName = match[2].str();
        String paramString = match[3].str();
        
        std::unordered_map<String, String> params;
        if (!paramString.empty()) {
            params = parseParameterList(paramString);
        }
        
        return std::make_tuple(templateName, varName, params);
    }
    
    return std::make_tuple("", "", {});
}

bool VariableSpecializationCallNode::isSpecializationCall(const String& expression) {
    std::regex callRegex(R"(\w+\s*\(\s*\w+.*\))");
    return std::regex_match(expression, callRegex);
}

std::unordered_map<String, String> VariableSpecializationCallNode::parseParameterList(const String& paramString) {
    std::unordered_map<String, String> params;
    
    // 解析 name = value 格式的参数
    std::regex paramRegex(R"((\w+)\s*=\s*([^,)]+))");
    std::sregex_iterator iter(paramString.begin(), paramString.end(), paramRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        String name = match[1].str();
        String value = match[2].str();
        
        // 去除前后空白
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        params[name] = value;
    }
    
    return params;
}

// 内部解析方法实现
String VariableSpecializationCallNode::resolveSimpleCall(const VariableTemplateNode* templateNode) const {
    // 检查是否有特例化参数
    if (hasSpecialization(variableName)) {
        return getSpecialization(variableName);
    }
    
    // 返回默认值
    return templateNode->getVariable(variableName);
}

String VariableSpecializationCallNode::resolveConditionalCall(const VariableTemplateNode* templateNode) const {
    // 处理条件变量的特例化调用
    String baseValue = templateNode->getVariable(variableName);
    
    // 应用上下文覆盖
    std::unordered_map<String, String> context = specializations;
    for (const auto& [key, value] : contextOverrides) {
        context[key] = value;
    }
    
    return templateNode->resolveVariable(variableName, context);
}

String VariableSpecializationCallNode::resolveComputedCall(const VariableTemplateNode* templateNode) const {
    // 处理计算变量的特例化调用
    std::unordered_map<String, String> context = specializations;
    for (const auto& [key, value] : contextOverrides) {
        context[key] = value;
    }
    
    return templateNode->resolveVariable(variableName, context);
}

String VariableSpecializationCallNode::processParameterValue(const String& value, const std::unordered_map<String, String>& context) const {
    if (isParameterExpression(value)) {
        return evaluateParameterExpression(value, context);
    }
    return value;
}

bool VariableSpecializationCallNode::isParameterExpression(const String& value) const {
    return value.find("calc(") == 0 || value.find("rgb(") == 0 || value.find("@") == 0;
}

String VariableSpecializationCallNode::evaluateParameterExpression(const String& expression, const std::unordered_map<String, String>& context) const {
    // 简单的表达式求值
    if (expression.find("rgb(") == 0) {
        return expression; // RGB表达式直接返回
    }
    if (expression.find("calc(") == 0) {
        return expression; // calc表达式直接返回
    }
    
    return expression;
}

bool VariableSpecializationCallNode::isValidParameterName(const String& name) const {
    return !name.empty() && 
           name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") == String::npos;
}

bool VariableSpecializationCallNode::isValidParameterValue(const String& value) const {
    return !value.empty();
}

// ========== VariableContextManager 实现 ==========

void VariableContextManager::pushContext(const String& contextName) {
    Context newContext;
    newContext.name = contextName.empty() ? "context_" + std::to_string(contextStack.size()) : contextName;
    newContext.parentScope = getCurrentScope();
    contextStack.push_back(newContext);
}

void VariableContextManager::popContext() {
    if (!contextStack.empty()) {
        contextStack.pop_back();
    }
}

void VariableContextManager::setContextVariable(const String& name, const String& value) {
    if (Context* ctx = getCurrentContext()) {
        ctx->variables[name] = value;
    }
}

String VariableContextManager::getContextVariable(const String& name) const {
    // 从当前上下文向上查找
    for (auto it = contextStack.rbegin(); it != contextStack.rend(); ++it) {
        auto varIt = it->variables.find(name);
        if (varIt != it->variables.end()) {
            return varIt->second;
        }
    }
    
    // 检查全局变量
    auto globalIt = globalVariables.find(name);
    return globalIt != globalVariables.end() ? globalIt->second : "";
}

bool VariableContextManager::hasContextVariable(const String& name) const {
    return !getContextVariable(name).empty();
}

void VariableContextManager::clearContext() {
    if (Context* ctx = getCurrentContext()) {
        ctx->variables.clear();
    }
}

void VariableContextManager::registerVariableTemplate(const String& name, std::shared_ptr<VariableTemplateNode> templateNode) {
    variableTemplates[name] = templateNode;
}

void VariableContextManager::unregisterVariableTemplate(const String& name) {
    variableTemplates.erase(name);
}

std::shared_ptr<VariableTemplateNode> VariableContextManager::getVariableTemplate(const String& name) const {
    auto it = variableTemplates.find(name);
    return it != variableTemplates.end() ? it->second : nullptr;
}

bool VariableContextManager::hasVariableTemplate(const String& name) const {
    return variableTemplates.find(name) != variableTemplates.end();
}

String VariableContextManager::resolveVariableCall(const String& templateName, const String& varName, 
                                                  const std::unordered_map<String, String>& specializations) const {
    // 检查缓存
    if (cachingEnabled) {
        String cacheKey = generateCacheKey(templateName, varName, specializations);
        auto cacheIt = resolvedCache.find(cacheKey);
        if (cacheIt != resolvedCache.end()) {
            return cacheIt->second;
        }
    }
    
    // 获取模板
    auto templateNode = getVariableTemplate(templateName);
    if (!templateNode) {
        return "";
    }
    
    // 创建上下文
    std::unordered_map<String, String> context = specializations;
    
    // 添加当前上下文变量
    for (const auto& contextVar : getCurrentContextVariables()) {
        if (context.find(contextVar.first) == context.end()) {
            context[contextVar.first] = contextVar.second;
        }
    }
    
    // 解析变量
    String result = templateNode->resolveVariable(varName, context);
    
    // 缓存结果
    if (cachingEnabled) {
        String cacheKey = generateCacheKey(templateName, varName, specializations);
        const_cast<VariableContextManager*>(this)->resolvedCache[cacheKey] = result;
    }
    
    return result;
}

String VariableContextManager::resolveVariableExpression(const String& expression) const {
    // 解析包含变量调用的表达式
    String result = expression;
    
    // 匹配 TemplateName(varName = value) 格式
    std::regex callRegex(R"((\w+)\s*\(\s*(\w+)(?:\s*=\s*([^)]+))?\s*\))");
    std::smatch match;
    
    while (std::regex_search(result, match, callRegex)) {
        String templateName = match[1].str();
        String varName = match[2].str();
        String specialization = match[3].str();
        
        std::unordered_map<String, String> specs;
        if (!specialization.empty()) {
            specs[varName] = specialization;
        }
        
        String resolvedValue = resolveVariableCall(templateName, varName, specs);
        result = std::regex_replace(result, std::regex(match[0].str()), resolvedValue);
    }
    
    return result;
}

void VariableContextManager::enterScope(const String& scopeName) {
    pushContext(scopeName);
}

void VariableContextManager::exitScope() {
    popContext();
}

String VariableContextManager::getCurrentScope() const {
    return contextStack.empty() ? "global" : contextStack.back().name;
}

void VariableContextManager::setScopeVariable(const String& name, const String& value) {
    setContextVariable(name, value);
}

void VariableContextManager::clearCache() {
    resolvedCache.clear();
}

void VariableContextManager::optimizeVariableResolution() {
    // 预解析常用的变量组合
    for (const auto& [templateName, templateNode] : variableTemplates) {
        if (templateNode) {
            auto varNames = templateNode->getAllVariableNames();
            for (const String& varName : varNames) {
                // 预缓存无特例化的调用
                String result = templateNode->resolveVariable(varName);
                String cacheKey = generateCacheKey(templateName, varName, {});
                resolvedCache[cacheKey] = result;
            }
        }
    }
}

StringVector VariableContextManager::getAllContexts() const {
    StringVector contexts;
    for (const auto& context : contextStack) {
        contexts.push_back(context.name);
    }
    return contexts;
}

StringVector VariableContextManager::getAllTemplates() const {
    StringVector templates;
    for (const auto& [name, templateNode] : variableTemplates) {
        templates.push_back(name);
    }
    return templates;
}

std::unordered_map<String, String> VariableContextManager::getCurrentContextVariables() const {
    if (const Context* ctx = getCurrentContext()) {
        return ctx->variables;
    }
    return {};
}

void VariableContextManager::dumpContextState() const {
    std::cout << "=== Variable Context State ===\n";
    std::cout << "Context Stack Size: " << contextStack.size() << "\n";
    std::cout << "Templates: " << variableTemplates.size() << "\n";
    std::cout << "Cache Entries: " << resolvedCache.size() << "\n";
    
    for (size_t i = 0; i < contextStack.size(); ++i) {
        const auto& context = contextStack[i];
        std::cout << "Context[" << i << "]: " << context.name 
                  << " (vars: " << context.variables.size() << ")\n";
    }
}

// 内部方法实现
String VariableContextManager::generateCacheKey(const String& templateName, const String& varName, 
                                               const std::unordered_map<String, String>& specializations) const {
    std::ostringstream oss;
    oss << templateName << "::" << varName;
    
    if (!specializations.empty()) {
        oss << "::";
        for (const auto& [key, value] : specializations) {
            oss << key << "=" << value << ",";
        }
    }
    
    return oss.str();
}

VariableContextManager::Context* VariableContextManager::getCurrentContext() {
    return contextStack.empty() ? nullptr : &contextStack.back();
}

const VariableContextManager::Context* VariableContextManager::getCurrentContext() const {
    return contextStack.empty() ? nullptr : &contextStack.back();
}

} // namespace CHTL