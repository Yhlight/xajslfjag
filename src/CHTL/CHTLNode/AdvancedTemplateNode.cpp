#include "AdvancedTemplateNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <fstream>
#include <iostream>

namespace CHTL {

// ========== AdvancedTemplateNode 实现 ==========

AdvancedTemplateNode::AdvancedTemplateNode(AdvancedTemplateType type, const String& name, const Position& pos)
    : TemplateNode(pos), advancedType(type), isCompiledTemplate(false), version(1.0) {
    setNodeName(name);
}

void AdvancedTemplateNode::addParameter(const TemplateParameter& param) {
    // 检查是否已存在同名参数
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&param](const TemplateParameter& p) { return p.name == param.name; });
    
    if (it != parameters.end()) {
        *it = param; // 替换现有参数
    } else {
        parameters.push_back(param);
    }
}

void AdvancedTemplateNode::addParameter(const String& name, const String& type, const String& defaultValue, bool required) {
    TemplateParameter param(name, type);
    param.defaultValue = defaultValue;
    param.isRequired = required;
    addParameter(param);
}

void AdvancedTemplateNode::removeParameter(const String& name) {
    parameters.erase(
        std::remove_if(parameters.begin(), parameters.end(),
                      [&name](const TemplateParameter& p) { return p.name == name; }),
        parameters.end()
    );
}

TemplateParameter* AdvancedTemplateNode::getParameter(const String& name) {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const TemplateParameter& p) { return p.name == name; });
    return it != parameters.end() ? &(*it) : nullptr;
}

const TemplateParameter* AdvancedTemplateNode::getParameter(const String& name) const {
    auto it = std::find_if(parameters.begin(), parameters.end(),
                          [&name](const TemplateParameter& p) { return p.name == name; });
    return it != parameters.end() ? &(*it) : nullptr;
}

bool AdvancedTemplateNode::hasParameter(const String& name) const {
    return getParameter(name) != nullptr;
}

StringVector AdvancedTemplateNode::getParameterNames() const {
    StringVector names;
    for (const auto& param : parameters) {
        names.push_back(param.name);
    }
    return names;
}

StringUnorderedMap AdvancedTemplateNode::getDefaultParameters() const {
    StringUnorderedMap defaults;
    for (const auto& param : parameters) {
        if (!param.defaultValue.empty()) {
            defaults[param.name] = param.defaultValue;
        }
    }
    return defaults;
}

void AdvancedTemplateNode::setBaseTemplate(const String& baseName) {
    inheritance.baseTemplate = baseName;
}

void AdvancedTemplateNode::addInterface(const String& interfaceName) {
    if (std::find(inheritance.interfaces.begin(), inheritance.interfaces.end(), interfaceName) == inheritance.interfaces.end()) {
        inheritance.interfaces.push_back(interfaceName);
    }
}

void AdvancedTemplateNode::addOverride(const String& part, const String& content) {
    inheritance.overrides[part] = content;
}

void AdvancedTemplateNode::setAbstract(bool abstract) {
    inheritance.isAbstract = abstract;
}

void AdvancedTemplateNode::setFinal(bool final) {
    inheritance.isFinal = final;
}

void AdvancedTemplateNode::addSlot(const String& name, const String& defaultContent) {
    slots[name] = defaultContent;
}

void AdvancedTemplateNode::removeSlot(const String& name) {
    slots.erase(name);
}

String AdvancedTemplateNode::getSlot(const String& name) const {
    auto it = slots.find(name);
    return it != slots.end() ? it->second : "";
}

bool AdvancedTemplateNode::hasSlot(const String& name) const {
    return slots.find(name) != slots.end();
}

StringVector AdvancedTemplateNode::getSlotNames() const {
    StringVector names;
    for (const auto& [name, content] : slots) {
        names.push_back(name);
    }
    return names;
}

void AdvancedTemplateNode::fillSlot(const String& name, const String& content) {
    if (hasSlot(name)) {
        slots[name] = content;
    }
}

void AdvancedTemplateNode::addMixin(const String& name, const String& templateName) {
    mixins[name] = templateName;
}

void AdvancedTemplateNode::removeMixin(const String& name) {
    mixins.erase(name);
}

String AdvancedTemplateNode::getMixin(const String& name) const {
    auto it = mixins.find(name);
    return it != mixins.end() ? it->second : "";
}

bool AdvancedTemplateNode::hasMixin(const String& name) const {
    return mixins.find(name) != mixins.end();
}

StringVector AdvancedTemplateNode::getMixinNames() const {
    StringVector names;
    for (const auto& [name, templateName] : mixins) {
        names.push_back(name);
    }
    return names;
}

void AdvancedTemplateNode::applyMixins() {
    // 应用混入模板到当前模板内容
    String content = getNodeValue();
    
    for (const auto& [name, templateName] : mixins) {
        // 在内容中查找混入占位符并替换
        String placeholder = "{{" + name + "}}";
        size_t pos = content.find(placeholder);
        if (pos != String::npos) {
            // 这里应该获取混入模板的内容并替换
            String mixinContent = "/* Mixin: " + templateName + " */";
            content.replace(pos, placeholder.length(), mixinContent);
        }
    }
    
    setNodeValue(content);
}

void AdvancedTemplateNode::addDependency(const String& templateName) {
    if (std::find(dependencies.begin(), dependencies.end(), templateName) == dependencies.end()) {
        dependencies.push_back(templateName);
    }
}

void AdvancedTemplateNode::removeDependency(const String& templateName) {
    dependencies.erase(
        std::remove(dependencies.begin(), dependencies.end(), templateName),
        dependencies.end()
    );
}

bool AdvancedTemplateNode::hasDependency(const String& templateName) const {
    return std::find(dependencies.begin(), dependencies.end(), templateName) != dependencies.end();
}

void AdvancedTemplateNode::resolveDependencies(const std::unordered_map<String, std::shared_ptr<AdvancedTemplateNode>>& templates) {
    // 解析和加载依赖的模板
    for (const String& depName : dependencies) {
        auto it = templates.find(depName);
        if (it != templates.end()) {
            // 处理依赖关系
            auto depTemplate = it->second;
            // 可以将依赖模板的内容合并到当前模板
        }
    }
}

void AdvancedTemplateNode::addCondition(const String& condition) {
    conditions.push_back(condition);
}

void AdvancedTemplateNode::removeCondition(const String& condition) {
    conditions.erase(
        std::remove(conditions.begin(), conditions.end(), condition),
        conditions.end()
    );
}

bool AdvancedTemplateNode::hasCondition(const String& condition) const {
    return std::find(conditions.begin(), conditions.end(), condition) != conditions.end();
}

bool AdvancedTemplateNode::evaluateConditions(const StringUnorderedMap& context) const {
    for (const String& condition : conditions) {
        if (!evaluateCondition(condition, context)) {
            return false;
        }
    }
    return true;
}

bool AdvancedTemplateNode::compile(const TemplateCompileOptions& options) {
    compileOptions = options;
    
    try {
        switch (advancedType) {
            case AdvancedTemplateType::STYLE_BASIC:
            case AdvancedTemplateType::STYLE_RESPONSIVE:
            case AdvancedTemplateType::STYLE_THEMED:
            case AdvancedTemplateType::STYLE_ANIMATED:
                compiledContent = compileStyleTemplate();
                break;
                
            case AdvancedTemplateType::ELEMENT_BASIC:
            case AdvancedTemplateType::ELEMENT_COMPONENT:
            case AdvancedTemplateType::ELEMENT_LAYOUT:
            case AdvancedTemplateType::ELEMENT_INTERACTIVE:
                compiledContent = compileElementTemplate();
                break;
                
            case AdvancedTemplateType::VAR_SIMPLE:
            case AdvancedTemplateType::VAR_COMPUTED:
            case AdvancedTemplateType::VAR_CONTEXTUAL:
            case AdvancedTemplateType::VAR_REACTIVE:
                compiledContent = compileVarTemplate();
                break;
                
            case AdvancedTemplateType::MIXIN:
                compiledContent = compileMixinTemplate();
                break;
                
            default:
                compiledContent = getNodeValue();
                break;
        }
        
        isCompiledTemplate = true;
        
        if (options.enableOptimization) {
            optimize();
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "模板编译错误: " << e.what() << std::endl;
        return false;
    }
}

void AdvancedTemplateNode::clearCompiled() {
    isCompiledTemplate = false;
    compiledContent.clear();
}

String AdvancedTemplateNode::instantiate(const StringUnorderedMap& parameters) const {
    StringUnorderedMap allParams = getDefaultParameters();
    
    // 合并提供的参数
    for (const auto& [key, value] : parameters) {
        allParams[key] = value;
    }
    
    // 验证参数
    if (!validateParameters(allParams)) {
        return "";
    }
    
    String content = isCompiledTemplate ? compiledContent : getNodeValue();
    
    switch (advancedType) {
        case AdvancedTemplateType::STYLE_BASIC:
        case AdvancedTemplateType::STYLE_RESPONSIVE:
        case AdvancedTemplateType::STYLE_THEMED:
        case AdvancedTemplateType::STYLE_ANIMATED:
            return instantiateStyle(allParams);
            
        case AdvancedTemplateType::ELEMENT_BASIC:
        case AdvancedTemplateType::ELEMENT_COMPONENT:
        case AdvancedTemplateType::ELEMENT_LAYOUT:
        case AdvancedTemplateType::ELEMENT_INTERACTIVE:
            return instantiateElement(allParams);
            
        case AdvancedTemplateType::VAR_SIMPLE:
        case AdvancedTemplateType::VAR_COMPUTED:
        case AdvancedTemplateType::VAR_CONTEXTUAL:
        case AdvancedTemplateType::VAR_REACTIVE:
            return instantiateVar(allParams);
            
        default:
            return expandTemplate(content, allParams);
    }
}

String AdvancedTemplateNode::instantiateWithSlots(const StringUnorderedMap& parameters, const StringUnorderedMap& slots) const {
    String content = instantiate(parameters);
    return processSlots(content, slots);
}

std::unique_ptr<BaseNode> AdvancedTemplateNode::createInstance(const StringUnorderedMap& parameters) const {
    // 创建模板实例节点
    auto instance = std::make_unique<AdvancedTemplateNode>(advancedType, getNodeName(), position);
    instance->setNodeValue(instantiate(parameters));
    return instance;
}

bool AdvancedTemplateNode::validate() const {
    return getValidationErrors().empty();
}

StringVector AdvancedTemplateNode::getValidationErrors() const {
    StringVector errors;
    
    // 验证模板名称
    if (getNodeName().empty()) {
        errors.push_back("模板名称不能为空");
    }
    
    // 验证模板内容
    if (getNodeValue().empty() && !isAbstract()) {
        errors.push_back("非抽象模板必须有内容");
    }
    
    // 验证参数
    if (!validateParameters()) {
        errors.push_back("模板参数验证失败");
    }
    
    // 验证继承关系
    if (!validateInheritance()) {
        errors.push_back("模板继承关系验证失败");
    }
    
    // 根据类型进行特定验证
    switch (advancedType) {
        case AdvancedTemplateType::STYLE_BASIC:
        case AdvancedTemplateType::STYLE_RESPONSIVE:
        case AdvancedTemplateType::STYLE_THEMED:
        case AdvancedTemplateType::STYLE_ANIMATED:
            if (!validateStyleTemplate()) {
                errors.push_back("样式模板验证失败");
            }
            break;
            
        case AdvancedTemplateType::ELEMENT_BASIC:
        case AdvancedTemplateType::ELEMENT_COMPONENT:
        case AdvancedTemplateType::ELEMENT_LAYOUT:
        case AdvancedTemplateType::ELEMENT_INTERACTIVE:
            if (!validateElementTemplate()) {
                errors.push_back("元素模板验证失败");
            }
            break;
            
        case AdvancedTemplateType::VAR_SIMPLE:
        case AdvancedTemplateType::VAR_COMPUTED:
        case AdvancedTemplateType::VAR_CONTEXTUAL:
        case AdvancedTemplateType::VAR_REACTIVE:
            if (!validateVarTemplate()) {
                errors.push_back("变量模板验证失败");
            }
            break;
            
        default:
            break;
    }
    
    return errors;
}

bool AdvancedTemplateNode::validateParameters(const StringUnorderedMap& params) const {
    for (const auto& param : parameters) {
        if (param.isRequired) {
            auto it = params.find(param.name);
            if (it == params.end() || it->second.empty()) {
                return false;
            }
        }
        
        // 验证参数约束
        auto it = params.find(param.name);
        if (it != params.end()) {
            const String& value = it->second;
            
            // 检查允许的值
            if (!param.allowedValues.empty()) {
                if (std::find(param.allowedValues.begin(), param.allowedValues.end(), value) == param.allowedValues.end()) {
                    return false;
                }
            }
            
            // 检查约束条件
            for (const String& constraint : param.constraints) {
                if (!evaluateCondition(constraint, params)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

StringVector AdvancedTemplateNode::getParameterValidationErrors(const StringUnorderedMap& params) const {
    StringVector errors;
    
    for (const auto& param : parameters) {
        if (param.isRequired) {
            auto it = params.find(param.name);
            if (it == params.end() || it->second.empty()) {
                errors.push_back("缺少必需参数: " + param.name);
            }
        }
        
        auto it = params.find(param.name);
        if (it != params.end()) {
            const String& value = it->second;
            
            if (!param.allowedValues.empty()) {
                if (std::find(param.allowedValues.begin(), param.allowedValues.end(), value) == param.allowedValues.end()) {
                    errors.push_back("参数值不在允许范围内: " + param.name + " = " + value);
                }
            }
            
            for (const String& constraint : param.constraints) {
                if (!evaluateCondition(constraint, params)) {
                    errors.push_back("参数约束验证失败: " + param.name + " (" + constraint + ")");
                }
            }
        }
    }
    
    return errors;
}

void AdvancedTemplateNode::optimize() {
    switch (advancedType) {
        case AdvancedTemplateType::STYLE_BASIC:
        case AdvancedTemplateType::STYLE_RESPONSIVE:
        case AdvancedTemplateType::STYLE_THEMED:
        case AdvancedTemplateType::STYLE_ANIMATED:
            optimizeStyleTemplate();
            break;
            
        case AdvancedTemplateType::ELEMENT_BASIC:
        case AdvancedTemplateType::ELEMENT_COMPONENT:
        case AdvancedTemplateType::ELEMENT_LAYOUT:
        case AdvancedTemplateType::ELEMENT_INTERACTIVE:
            optimizeElementTemplate();
            break;
            
        case AdvancedTemplateType::VAR_SIMPLE:
        case AdvancedTemplateType::VAR_COMPUTED:
        case AdvancedTemplateType::VAR_CONTEXTUAL:
        case AdvancedTemplateType::VAR_REACTIVE:
            optimizeVarTemplate();
            break;
            
        default:
            break;
    }
    
    removeUnusedParameters();
    simplifyConditions();
}

void AdvancedTemplateNode::inlineSmallTemplates() {
    // 内联小的模板片段
    if (compiledContent.length() < 100) {
        // 对于小模板，可以进行内联优化
        compileOptions.enableInlining = true;
    }
}

void AdvancedTemplateNode::cacheCommonPatterns() {
    // 缓存常用的模式
    if (compileOptions.enableCaching) {
        // 实现缓存逻辑
    }
}

void AdvancedTemplateNode::removeUnusedParameters() {
    String content = getNodeValue();
    
    parameters.erase(
        std::remove_if(parameters.begin(), parameters.end(),
                      [&content](const TemplateParameter& param) {
                          // 检查参数是否在内容中被使用
                          return content.find("{{" + param.name + "}}") == String::npos &&
                                 content.find("${" + param.name + "}") == String::npos;
                      }),
        parameters.end()
    );
}

void AdvancedTemplateNode::simplifyConditions() {
    // 简化条件表达式
    for (String& condition : conditions) {
        // 移除多余的空白
        condition = std::regex_replace(condition, std::regex(R"(\s+)"), " ");
        condition.erase(0, condition.find_first_not_of(" \t"));
        condition.erase(condition.find_last_not_of(" \t") + 1);
    }
}

// 响应式功能
void AdvancedTemplateNode::addBreakpoint(const String& name, const String& condition, const String& content) {
    breakpoints[name] = "@media " + condition + " { " + content + " }";
}

void AdvancedTemplateNode::removeBreakpoint(const String& name) {
    breakpoints.erase(name);
}

String AdvancedTemplateNode::getResponsiveCSS() const {
    std::ostringstream oss;
    for (const auto& [name, css] : breakpoints) {
        oss << css << "\n";
    }
    return oss.str();
}

bool AdvancedTemplateNode::isResponsive() const {
    return !breakpoints.empty() || 
           advancedType == AdvancedTemplateType::STYLE_RESPONSIVE;
}

// 主题化功能
void AdvancedTemplateNode::addTheme(const String& themeName, const StringUnorderedMap& variables) {
    themes[themeName] = variables;
}

void AdvancedTemplateNode::removeTheme(const String& themeName) {
    themes.erase(themeName);
}

String AdvancedTemplateNode::getThemedContent(const String& themeName) const {
    auto it = themes.find(themeName);
    if (it == themes.end()) {
        return getNodeValue();
    }
    
    return expandVariables(getNodeValue(), it->second);
}

StringVector AdvancedTemplateNode::getAvailableThemes() const {
    StringVector themeNames;
    for (const auto& [name, vars] : themes) {
        themeNames.push_back(name);
    }
    return themeNames;
}

// 动画功能
void AdvancedTemplateNode::addAnimation(const String& name, const String& keyframes) {
    animations[name] = "@keyframes " + name + " { " + keyframes + " }";
}

void AdvancedTemplateNode::removeAnimation(const String& name) {
    animations.erase(name);
}

String AdvancedTemplateNode::getAnimationCSS() const {
    std::ostringstream oss;
    for (const auto& [name, css] : animations) {
        oss << css << "\n";
    }
    return oss.str();
}

bool AdvancedTemplateNode::hasAnimations() const {
    return !animations.empty() || 
           advancedType == AdvancedTemplateType::STYLE_ANIMATED;
}

// 组件化功能
void AdvancedTemplateNode::addProp(const String& name, const String& type, const String& defaultValue) {
    props[name] = type + ":" + defaultValue;
}

void AdvancedTemplateNode::removeProp(const String& name) {
    props.erase(name);
}

StringUnorderedMap AdvancedTemplateNode::getProps() const {
    return props;
}

String AdvancedTemplateNode::generatePropTypes() const {
    std::ostringstream oss;
    oss << "PropTypes: {\n";
    for (const auto& [name, typeAndDefault] : props) {
        size_t colonPos = typeAndDefault.find(':');
        String type = typeAndDefault.substr(0, colonPos);
        String defaultVal = colonPos != String::npos ? typeAndDefault.substr(colonPos + 1) : "";
        
        oss << "  " << name << ": " << type;
        if (!defaultVal.empty()) {
            oss << " = " << defaultVal;
        }
        oss << ",\n";
    }
    oss << "}";
    return oss.str();
}

// 布局功能
void AdvancedTemplateNode::setLayoutType(const String& type) {
    layoutType = type;
}

void AdvancedTemplateNode::addLayoutProperty(const String& property, const String& value) {
    layoutProperties[property] = value;
}

String AdvancedTemplateNode::getLayoutCSS() const {
    std::ostringstream oss;
    
    if (!layoutType.empty()) {
        if (layoutType == "flex") {
            oss << "display: flex;\n";
        } else if (layoutType == "grid") {
            oss << "display: grid;\n";
        } else if (layoutType == "absolute") {
            oss << "position: absolute;\n";
        }
    }
    
    for (const auto& [prop, value] : layoutProperties) {
        oss << prop << ": " << value << ";\n";
    }
    
    return oss.str();
}

// 交互功能
void AdvancedTemplateNode::addEvent(const String& eventName, const String& handler) {
    events[eventName] = handler;
}

void AdvancedTemplateNode::removeEvent(const String& eventName) {
    events.erase(eventName);
}

StringUnorderedMap AdvancedTemplateNode::getEvents() const {
    return events;
}

String AdvancedTemplateNode::generateEventHandlers() const {
    std::ostringstream oss;
    for (const auto& [event, handler] : events) {
        oss << "element.addEventListener('" << event << "', " << handler << ");\n";
    }
    return oss.str();
}

// 计算变量功能
void AdvancedTemplateNode::addComputedVar(const String& name, const String& expression) {
    computedVars[name] = expression;
}

void AdvancedTemplateNode::removeComputedVar(const String& name) {
    computedVars.erase(name);
}

String AdvancedTemplateNode::getComputedValue(const String& name, const StringUnorderedMap& context) const {
    auto it = computedVars.find(name);
    if (it == computedVars.end()) {
        return "";
    }
    
    // 简化的表达式计算
    return expandVariables(it->second, context);
}

// 上下文变量功能
void AdvancedTemplateNode::addContextVar(const String& name, const String& source) {
    contextVars[name] = source;
}

void AdvancedTemplateNode::removeContextVar(const String& name) {
    contextVars.erase(name);
}

String AdvancedTemplateNode::getContextValue(const String& name, const StringUnorderedMap& context) const {
    auto it = contextVars.find(name);
    if (it == contextVars.end()) {
        return "";
    }
    
    String source = it->second;
    auto contextIt = context.find(source);
    return contextIt != context.end() ? contextIt->second : "";
}

String AdvancedTemplateNode::getValue() const {
    if (isCompiledTemplate) {
        return compiledContent;
    }
    return getNodeValue();
}

std::unique_ptr<BaseNode> AdvancedTemplateNode::clone() const {
    auto cloned = std::make_unique<AdvancedTemplateNode>(advancedType, getNodeName(), position);
    
    // 深拷贝所有属性
    cloned->parameters = parameters;
    cloned->inheritance = inheritance;
    cloned->compileOptions = compileOptions;
    cloned->slots = slots;
    cloned->mixins = mixins;
    cloned->dependencies = dependencies;
    cloned->conditions = conditions;
    cloned->isCompiledTemplate = isCompiledTemplate;
    cloned->compiledContent = compiledContent;
    cloned->version = version;
    cloned->author = author;
    cloned->license = license;
    
    // 深拷贝特化属性
    cloned->breakpoints = breakpoints;
    cloned->themes = themes;
    cloned->animations = animations;
    cloned->props = props;
    cloned->layoutType = layoutType;
    cloned->layoutProperties = layoutProperties;
    cloned->events = events;
    cloned->computedVars = computedVars;
    cloned->contextVars = contextVars;
    
    cloned->setNodeValue(getNodeValue());
    
    return cloned;
}

String AdvancedTemplateNode::toAdvancedString() const {
    std::ostringstream oss;
    
    oss << "[Template] " << templateTypeToString(advancedType) << " " << getNodeName();
    
    if (!parameters.empty()) {
        oss << "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << parameters[i].name << ": " << parameters[i].type;
            if (!parameters[i].defaultValue.empty()) {
                oss << " = " << parameters[i].defaultValue;
            }
        }
        oss << ")";
    }
    
    if (!inheritance.baseTemplate.empty()) {
        oss << " extends " << inheritance.baseTemplate;
    }
    
    oss << " {\n";
    oss << getNodeValue() << "\n";
    oss << "}";
    
    return oss.str();
}

String AdvancedTemplateNode::toJSON() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"name\": \"" << getNodeName() << "\",\n";
    json << "  \"type\": \"" << templateTypeToString(advancedType) << "\",\n";
    json << "  \"version\": " << version << ",\n";
    json << "  \"author\": \"" << author << "\",\n";
    json << "  \"content\": \"" << getNodeValue() << "\",\n";
    json << "  \"parameters\": [";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) json << ", ";
        json << "{\n";
        json << "    \"name\": \"" << parameters[i].name << "\",\n";
        json << "    \"type\": \"" << parameters[i].type << "\",\n";
        json << "    \"required\": " << (parameters[i].isRequired ? "true" : "false") << "\n";
        json << "  }";
    }
    
    json << "],\n";
    json << "  \"compiled\": " << (isCompiledTemplate ? "true" : "false") << "\n";
    json << "}";
    
    return json.str();
}

// 静态工厂方法
std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createStyleTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::STYLE_BASIC, name, pos);
}

std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createElementTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::ELEMENT_BASIC, name, pos);
}

std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createVarTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::VAR_SIMPLE, name, pos);
}

std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createResponsiveTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::STYLE_RESPONSIVE, name, pos);
}

std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createComponentTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::ELEMENT_COMPONENT, name, pos);
}

std::unique_ptr<AdvancedTemplateNode> AdvancedTemplateNode::createMixinTemplate(const String& name, const Position& pos) {
    return std::make_unique<AdvancedTemplateNode>(AdvancedTemplateType::MIXIN, name, pos);
}

AdvancedTemplateType AdvancedTemplateNode::parseTemplateType(const String& typeString) {
    static const std::unordered_map<String, AdvancedTemplateType> typeMap = {
        {"@Style", AdvancedTemplateType::STYLE_BASIC},
        {"@StyleResponsive", AdvancedTemplateType::STYLE_RESPONSIVE},
        {"@StyleThemed", AdvancedTemplateType::STYLE_THEMED},
        {"@StyleAnimated", AdvancedTemplateType::STYLE_ANIMATED},
        {"@Element", AdvancedTemplateType::ELEMENT_BASIC},
        {"@Component", AdvancedTemplateType::ELEMENT_COMPONENT},
        {"@Layout", AdvancedTemplateType::ELEMENT_LAYOUT},
        {"@Interactive", AdvancedTemplateType::ELEMENT_INTERACTIVE},
        {"@Var", AdvancedTemplateType::VAR_SIMPLE},
        {"@ComputedVar", AdvancedTemplateType::VAR_COMPUTED},
        {"@ContextVar", AdvancedTemplateType::VAR_CONTEXTUAL},
        {"@ReactiveVar", AdvancedTemplateType::VAR_REACTIVE},
        {"@Mixin", AdvancedTemplateType::MIXIN},
        {"@Function", AdvancedTemplateType::FUNCTION},
        {"@Macro", AdvancedTemplateType::MACRO},
        {"@Partial", AdvancedTemplateType::PARTIAL}
    };
    
    auto it = typeMap.find(typeString);
    return it != typeMap.end() ? it->second : AdvancedTemplateType::STYLE_BASIC;
}

String AdvancedTemplateNode::templateTypeToString(AdvancedTemplateType type) {
    switch (type) {
        case AdvancedTemplateType::STYLE_BASIC: return "@Style";
        case AdvancedTemplateType::STYLE_RESPONSIVE: return "@StyleResponsive";
        case AdvancedTemplateType::STYLE_THEMED: return "@StyleThemed";
        case AdvancedTemplateType::STYLE_ANIMATED: return "@StyleAnimated";
        case AdvancedTemplateType::ELEMENT_BASIC: return "@Element";
        case AdvancedTemplateType::ELEMENT_COMPONENT: return "@Component";
        case AdvancedTemplateType::ELEMENT_LAYOUT: return "@Layout";
        case AdvancedTemplateType::ELEMENT_INTERACTIVE: return "@Interactive";
        case AdvancedTemplateType::VAR_SIMPLE: return "@Var";
        case AdvancedTemplateType::VAR_COMPUTED: return "@ComputedVar";
        case AdvancedTemplateType::VAR_CONTEXTUAL: return "@ContextVar";
        case AdvancedTemplateType::VAR_REACTIVE: return "@ReactiveVar";
        case AdvancedTemplateType::MIXIN: return "@Mixin";
        case AdvancedTemplateType::FUNCTION: return "@Function";
        case AdvancedTemplateType::MACRO: return "@Macro";
        case AdvancedTemplateType::PARTIAL: return "@Partial";
        default: return "@Unknown";
    }
}

bool AdvancedTemplateNode::isCompatibleType(AdvancedTemplateType type1, AdvancedTemplateType type2) {
    // 检查类型兼容性
    if (type1 == type2) return true;
    
    // 样式类型之间兼容
    if ((type1 >= AdvancedTemplateType::STYLE_BASIC && type1 <= AdvancedTemplateType::STYLE_ANIMATED) &&
        (type2 >= AdvancedTemplateType::STYLE_BASIC && type2 <= AdvancedTemplateType::STYLE_ANIMATED)) {
        return true;
    }
    
    // 元素类型之间兼容
    if ((type1 >= AdvancedTemplateType::ELEMENT_BASIC && type1 <= AdvancedTemplateType::ELEMENT_INTERACTIVE) &&
        (type2 >= AdvancedTemplateType::ELEMENT_BASIC && type2 <= AdvancedTemplateType::ELEMENT_INTERACTIVE)) {
        return true;
    }
    
    // 变量类型之间兼容
    if ((type1 >= AdvancedTemplateType::VAR_SIMPLE && type1 <= AdvancedTemplateType::VAR_REACTIVE) &&
        (type2 >= AdvancedTemplateType::VAR_SIMPLE && type2 <= AdvancedTemplateType::VAR_REACTIVE)) {
        return true;
    }
    
    return false;
}

// 内部编译方法
String AdvancedTemplateNode::compileStyleTemplate() const {
    std::ostringstream oss;
    
    // 基础样式内容
    oss << getNodeValue();
    
    // 添加响应式CSS
    if (isResponsive()) {
        oss << "\n" << getResponsiveCSS();
    }
    
    // 添加动画CSS
    if (hasAnimations()) {
        oss << "\n" << getAnimationCSS();
    }
    
    return oss.str();
}

String AdvancedTemplateNode::compileElementTemplate() const {
    std::ostringstream oss;
    
    // 基础元素内容
    oss << getNodeValue();
    
    // 添加布局CSS
    if (!layoutType.empty()) {
        oss << "\nstyle {\n" << getLayoutCSS() << "}";
    }
    
    // 添加事件处理器
    if (!events.empty()) {
        oss << "\nscript {\n" << generateEventHandlers() << "}";
    }
    
    return oss.str();
}

String AdvancedTemplateNode::compileVarTemplate() const {
    String content = getNodeValue();
    
    // 处理计算变量
    for (const auto& [name, expr] : computedVars) {
        String placeholder = "{{" + name + "}}";
        size_t pos = content.find(placeholder);
        if (pos != String::npos) {
            content.replace(pos, placeholder.length(), expr);
        }
    }
    
    return content;
}

String AdvancedTemplateNode::compileMixinTemplate() const {
    // 混入模板编译
    return "/* Mixin: " + getNodeName() + " */\n" + getNodeValue();
}

// 内部实例化方法
String AdvancedTemplateNode::instantiateStyle(const StringUnorderedMap& params) const {
    String content = expandTemplate(getNodeValue(), params);
    
    // 处理主题化
    if (!themes.empty()) {
        auto themeIt = params.find("theme");
        if (themeIt != params.end()) {
            content = getThemedContent(themeIt->second);
        }
    }
    
    return content;
}

String AdvancedTemplateNode::instantiateElement(const StringUnorderedMap& params) const {
    String content = expandTemplate(getNodeValue(), params);
    
    // 处理组件属性
    if (!props.empty()) {
        std::ostringstream propsSS;
        for (const auto& [name, typeAndDefault] : props) {
            auto paramIt = params.find(name);
            if (paramIt != params.end()) {
                propsSS << name << "=\"" << paramIt->second << "\" ";
            }
        }
        
        String propsStr = propsSS.str();
        if (!propsStr.empty()) {
            // 在第一个元素标签中插入属性
            size_t tagEnd = content.find('>');
            if (tagEnd != String::npos) {
                content.insert(tagEnd, " " + propsStr);
            }
        }
    }
    
    return content;
}

String AdvancedTemplateNode::instantiateVar(const StringUnorderedMap& params) const {
    String content = expandTemplate(getNodeValue(), params);
    
    // 处理计算变量
    for (const auto& [name, expr] : computedVars) {
        String computedValue = getComputedValue(name, params);
        String placeholder = "{{" + name + "}}";
        size_t pos = 0;
        while ((pos = content.find(placeholder, pos)) != String::npos) {
            content.replace(pos, placeholder.length(), computedValue);
            pos += computedValue.length();
        }
    }
    
    return content;
}

// 内部验证方法
bool AdvancedTemplateNode::validateStyleTemplate() const {
    String content = getNodeValue();
    
    // 检查基本的CSS语法
    int braceCount = 0;
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

bool AdvancedTemplateNode::validateElementTemplate() const {
    String content = getNodeValue();
    
    // 检查基本的HTML语法
    int braceCount = 0;
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

bool AdvancedTemplateNode::validateVarTemplate() const {
    String content = getNodeValue();
    
    // 检查变量定义语法
    return content.find(':') != String::npos; // 至少包含一个变量定义
}

bool AdvancedTemplateNode::validateParameters() const {
    for (const auto& param : parameters) {
        if (param.name.empty()) {
            return false;
        }
        
        // 检查参数名的有效性
        if (!std::regex_match(param.name, std::regex(R"([a-zA-Z_][a-zA-Z0-9_]*)"))) {
            return false;
        }
    }
    
    return true;
}

bool AdvancedTemplateNode::validateInheritance() const {
    if (inheritance.isFinal && !inheritance.baseTemplate.empty()) {
        return false; // 最终模板不能有基类
    }
    
    return true;
}

// 内部优化方法
void AdvancedTemplateNode::optimizeStyleTemplate() {
    // 优化CSS内容
    String content = getNodeValue();
    
    // 移除多余的空白
    content = std::regex_replace(content, std::regex(R"(\s+)"), " ");
    
    // 移除注释
    content = std::regex_replace(content, std::regex(R"(/\*.*?\*/)"), "");
    
    setNodeValue(content);
}

void AdvancedTemplateNode::optimizeElementTemplate() {
    // 优化HTML内容
    String content = getNodeValue();
    
    // 移除多余的空白
    content = std::regex_replace(content, std::regex(R"(\s+)"), " ");
    
    setNodeValue(content);
}

void AdvancedTemplateNode::optimizeVarTemplate() {
    // 优化变量定义
    String content = getNodeValue();
    
    // 移除重复的变量定义
    std::unordered_set<String> seen;
    std::ostringstream oss;
    std::istringstream iss(content);
    String line;
    
    while (std::getline(iss, line)) {
        size_t colonPos = line.find(':');
        if (colonPos != String::npos) {
            String varName = line.substr(0, colonPos);
            varName.erase(0, varName.find_first_not_of(" \t"));
            varName.erase(varName.find_last_not_of(" \t") + 1);
            
            if (seen.find(varName) == seen.end()) {
                seen.insert(varName);
                oss << line << "\n";
            }
        } else {
            oss << line << "\n";
        }
    }
    
    setNodeValue(oss.str());
}

// 工具方法实现
String AdvancedTemplateNode::expandTemplate(const String& content, const StringUnorderedMap& params) const {
    String result = content;
    
    // 展开参数
    for (const auto& [key, value] : params) {
        String placeholder = "{{" + key + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

String AdvancedTemplateNode::processConditions(const String& content, const StringUnorderedMap& context) const {
    String result = content;
    
    // 处理条件语句 {{#if condition}}...{{/if}}
    std::regex ifRegex(R"(\{\{#if\s+([^}]+)\}\}(.*?)\{\{/if\}\})");
    std::smatch match;
    
    while (std::regex_search(result, match, ifRegex)) {
        String condition = match[1].str();
        String ifContent = match[2].str();
        
        String replacement = evaluateCondition(condition, context) ? ifContent : "";
        result = std::regex_replace(result, ifRegex, replacement, std::regex_constants::format_first_only);
    }
    
    return result;
}

String AdvancedTemplateNode::processMixins(const String& content) const {
    String result = content;
    
    // 处理混入引用
    for (const auto& [name, templateName] : mixins) {
        String placeholder = "{{@" + name + "}}";
        String mixinContent = "/* Mixin: " + templateName + " */";
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), mixinContent);
            pos += mixinContent.length();
        }
    }
    
    return result;
}

String AdvancedTemplateNode::processSlots(const String& content, const StringUnorderedMap& slotContents) const {
    String result = content;
    
    // 处理插槽
    for (const auto& [slotName, slotContent] : slotContents) {
        String placeholder = "{{slot:" + slotName + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), slotContent);
            pos += slotContent.length();
        }
    }
    
    return result;
}

bool AdvancedTemplateNode::evaluateCondition(const String& condition, const StringUnorderedMap& context) const {
    // 简化的条件评估
    if (condition == "true") return true;
    if (condition == "false") return false;
    
    // 检查变量存在性
    auto it = context.find(condition);
    if (it != context.end()) {
        String value = it->second;
        return !value.empty() && value != "false" && value != "0";
    }
    
    // 简单的比较操作
    std::regex compareRegex(R"((\w+)\s*(==|!=|>|<|>=|<=)\s*(.+))");
    std::smatch match;
    if (std::regex_match(condition, match, compareRegex)) {
        String left = match[1].str();
        String op = match[2].str();
        String right = match[3].str();
        
        auto leftIt = context.find(left);
        if (leftIt != context.end()) {
            String leftValue = leftIt->second;
            
            if (op == "==") return leftValue == right;
            if (op == "!=") return leftValue != right;
            // 其他比较操作可以继续实现
        }
    }
    
    return false;
}

String AdvancedTemplateNode::expandVariables(const String& content, const StringUnorderedMap& variables) const {
    String result = content;
    
    for (const auto& [key, value] : variables) {
        String placeholder = "${" + key + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != String::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

// ========== AdvancedTemplateRegistry 实现 ==========

AdvancedTemplateRegistry& AdvancedTemplateRegistry::getInstance() {
    static AdvancedTemplateRegistry instance;
    return instance;
}

void AdvancedTemplateRegistry::registerTemplate(const String& name, std::shared_ptr<AdvancedTemplateNode> template_) {
    templates[name] = template_;
}

void AdvancedTemplateRegistry::unregisterTemplate(const String& name) {
    templates.erase(name);
}

std::shared_ptr<AdvancedTemplateNode> AdvancedTemplateRegistry::getTemplate(const String& name) const {
    auto it = templates.find(name);
    return it != templates.end() ? it->second : nullptr;
}

bool AdvancedTemplateRegistry::hasTemplate(const String& name) const {
    return templates.find(name) != templates.end();
}

StringVector AdvancedTemplateRegistry::getAllTemplateNames() const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        names.push_back(name);
    }
    return names;
}

StringVector AdvancedTemplateRegistry::getTemplatesByType(AdvancedTemplateType type) const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        if (template_->advancedType == type) {
            names.push_back(name);
        }
    }
    return names;
}

StringVector AdvancedTemplateRegistry::getStyleTemplates() const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        if (template_->advancedType >= AdvancedTemplateType::STYLE_BASIC && 
            template_->advancedType <= AdvancedTemplateType::STYLE_ANIMATED) {
            names.push_back(name);
        }
    }
    return names;
}

StringVector AdvancedTemplateRegistry::getElementTemplates() const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        if (template_->advancedType >= AdvancedTemplateType::ELEMENT_BASIC && 
            template_->advancedType <= AdvancedTemplateType::ELEMENT_INTERACTIVE) {
            names.push_back(name);
        }
    }
    return names;
}

StringVector AdvancedTemplateRegistry::getVarTemplates() const {
    StringVector names;
    for (const auto& [name, template_] : templates) {
        if (template_->advancedType >= AdvancedTemplateType::VAR_SIMPLE && 
            template_->advancedType <= AdvancedTemplateType::VAR_REACTIVE) {
            names.push_back(name);
        }
    }
    return names;
}

size_t AdvancedTemplateRegistry::getTemplateCount() const {
    return templates.size();
}

std::unordered_map<String, size_t> AdvancedTemplateRegistry::getTypeStatistics() const {
    std::unordered_map<String, size_t> stats;
    
    for (const auto& [name, template_] : templates) {
        String typeName = AdvancedTemplateNode::templateTypeToString(template_->advancedType);
        stats[typeName]++;
    }
    
    return stats;
}

void AdvancedTemplateRegistry::registerBuiltinTemplates() {
    initializeBuiltinTemplates();
}

void AdvancedTemplateRegistry::initializeBuiltinTemplates() {
    // 创建内置的基础模板
    
    // 基础样式模板
    auto basicStyle = AdvancedTemplateNode::createStyleTemplate("BasicText");
    basicStyle->setNodeValue("color: {{textColor:black}}; font-size: {{fontSize:16px}};");
    basicStyle->addParameter("textColor", "Color", "black");
    basicStyle->addParameter("fontSize", "Size", "16px");
    registerTemplate("BasicText", basicStyle);
    
    // 基础元素模板
    auto basicElement = AdvancedTemplateNode::createElementTemplate("BasicBox");
    basicElement->setNodeValue("div { style { width: {{width:100px}}; height: {{height:100px}}; } }");
    basicElement->addParameter("width", "Size", "100px");
    basicElement->addParameter("height", "Size", "100px");
    registerTemplate("BasicBox", basicElement);
    
    // 基础变量模板
    auto basicVar = AdvancedTemplateNode::createVarTemplate("BasicColors");
    basicVar->setNodeValue("primary: {{primary:#007bff}}; secondary: {{secondary:#6c757d}};");
    basicVar->addParameter("primary", "Color", "#007bff");
    basicVar->addParameter("secondary", "Color", "#6c757d");
    registerTemplate("BasicColors", basicVar);
}

// ========== AdvancedTemplateBuilder 实现 ==========

AdvancedTemplateBuilder::AdvancedTemplateBuilder(const String& templateName) {
    template_ = std::make_shared<AdvancedTemplateNode>(AdvancedTemplateType::STYLE_BASIC, templateName);
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::type(AdvancedTemplateType templateType) {
    template_->advancedType = templateType;
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::description(const String& desc) {
    // 可以添加一个描述字段到AdvancedTemplateNode
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::version(double ver) {
    template_->version = ver;
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::author(const String& authorName) {
    template_->author = authorName;
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::license(const String& licenseName) {
    template_->license = licenseName;
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::content(const String& templateContent) {
    template_->setNodeValue(templateContent);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::parameter(const String& name, const String& type, const String& defaultValue, bool required) {
    template_->addParameter(name, type, defaultValue, required);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::requiredParameter(const String& name, const String& type) {
    template_->addParameter(name, type, "", true);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::optionalParameter(const String& name, const String& defaultValue, const String& type) {
    template_->addParameter(name, type, defaultValue, false);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::inherits(const String& baseName) {
    template_->setBaseTemplate(baseName);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::slot(const String& name, const String& defaultContent) {
    template_->addSlot(name, defaultContent);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::mixin(const String& name, const String& templateName) {
    template_->addMixin(name, templateName);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::dependency(const String& templateName) {
    template_->addDependency(templateName);
    return *this;
}

AdvancedTemplateBuilder& AdvancedTemplateBuilder::enableOptimization(bool enable) {
    template_->compileOptions.enableOptimization = enable;
    return *this;
}

std::shared_ptr<AdvancedTemplateNode> AdvancedTemplateBuilder::build() {
    if (validateBuild()) {
        return template_;
    }
    return nullptr;
}

String AdvancedTemplateBuilder::buildToString() {
    auto built = build();
    return built ? built->toAdvancedString() : "";
}

bool AdvancedTemplateBuilder::registerToGlobal() {
    auto built = build();
    if (built) {
        AdvancedTemplateRegistry::getInstance().registerTemplate(built->getNodeName(), built);
        return true;
    }
    return false;
}

bool AdvancedTemplateBuilder::validateBuild() const {
    return buildErrors.empty() && template_ && !template_->getNodeName().empty();
}

void AdvancedTemplateBuilder::addBuildError(const String& error) {
    buildErrors.push_back(error);
}

} // namespace CHTL