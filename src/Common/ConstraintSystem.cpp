#include "ConstraintSystem.h"
#include <regex>
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace Common {

// 预定义约束常量实现
namespace PredefinedConstraints {
    const std::string HTML_ELEMENT_WHITELIST = "html_element_whitelist";
    const std::string TEMPLATE_TYPE_CONSTRAINT = "template_type_constraint";
    const std::string CUSTOM_TYPE_CONSTRAINT = "custom_type_constraint";
    const std::string IMPORT_TYPE_CONSTRAINT = "import_type_constraint";
    const std::string NAMESPACE_NAMING_CONSTRAINT = "namespace_naming_constraint";
    const std::string SELECTOR_SYNTAX_CONSTRAINT = "selector_syntax_constraint";
    const std::string ATTRIBUTE_VALIDATION_CONSTRAINT = "attribute_validation_constraint";
}

// ConstraintContext实现
void ConstraintContext::setContext(const std::string& type, const std::string& element) {
    current_context_type = type;
    current_element = element;
}

void ConstraintContext::addContextData(const std::string& key, const std::string& value) {
    context_data[key] = value;
}

std::string ConstraintContext::getContextData(const std::string& key, const std::string& default_val) const {
    auto it = context_data.find(key);
    return (it != context_data.end()) ? it->second : default_val;
}

bool ConstraintContext::hasConstraint(const std::string& constraint_name) const {
    return std::find(active_constraints.begin(), active_constraints.end(), constraint_name) 
           != active_constraints.end();
}

// ConstraintSystem实现
ConstraintSystem::ConstraintSystem() {
    initializeBuiltinConstraints();
}

void ConstraintSystem::initializeBuiltinConstraints() {
    setupHTMLElementConstraints();
    setupTemplateConstraints();
    setupCustomElementConstraints();
    setupImportConstraints();
    setupSyntaxConstraints();
}

void ConstraintSystem::setupHTMLElementConstraints() {
    // HTML元素白名单约束
    ConstraintCondition html_constraint;
    html_constraint.name = PredefinedConstraints::HTML_ELEMENT_WHITELIST;
    html_constraint.type = ConstraintType::HTML_ELEMENT_CONSTRAINT;
    html_constraint.scope = ConstraintScope::GLOBAL;
    html_constraint.level = ConstraintLevel::ERROR;
    
    // 标准HTML元素列表
    html_constraint.targets = {
        "html", "head", "title", "meta", "link", "style", "script", "noscript",
        "body", "header", "nav", "main", "section", "article", "aside", "footer",
        "h1", "h2", "h3", "h4", "h5", "h6", "p", "hr", "pre", "blockquote",
        "div", "span", "a", "strong", "em", "small", "s", "cite", "q", "dfn",
        "abbr", "data", "time", "code", "var", "samp", "kbd", "sub", "sup",
        "i", "b", "u", "mark", "ruby", "rt", "rp", "bdi", "bdo", "br", "wbr",
        "ins", "del", "img", "iframe", "embed", "object", "param", "video",
        "audio", "source", "track", "canvas", "map", "area", "svg", "math",
        "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot",
        "tr", "td", "th", "form", "fieldset", "legend", "label", "input",
        "button", "select", "datalist", "optgroup", "option", "textarea",
        "output", "progress", "meter", "details", "summary", "dialog",
        "ul", "ol", "li", "dl", "dt", "dd", "figure", "figcaption"
    };
    
    html_constraint.validator = [](const std::string& element, const std::unordered_map<std::string, std::string>&) {
        return ConstraintUtils::isValidHTMLElement(element);
    };
    
    addConstraint(html_constraint);
    
    // 属性验证约束
    ConstraintCondition attr_constraint;
    attr_constraint.name = PredefinedConstraints::ATTRIBUTE_VALIDATION_CONSTRAINT;
    attr_constraint.type = ConstraintType::HTML_ELEMENT_CONSTRAINT;
    attr_constraint.scope = ConstraintScope::GLOBAL;
    attr_constraint.level = ConstraintLevel::WARNING;
    
    attr_constraint.validator = [](const std::string& attr_info, const std::unordered_map<std::string, std::string>& params) {
        auto element_it = params.find("element");
        auto attr_it = params.find("attribute");
        if (element_it != params.end() && attr_it != params.end()) {
            return ConstraintUtils::isValidHTMLAttribute(element_it->second, attr_it->second);
        }
        return true;
    };
    
    addConstraint(attr_constraint);
}

void ConstraintSystem::setupTemplateConstraints() {
    // 模板类型约束
    ConstraintCondition template_constraint;
    template_constraint.name = PredefinedConstraints::TEMPLATE_TYPE_CONSTRAINT;
    template_constraint.type = ConstraintType::TYPE_CONSTRAINT;
    template_constraint.scope = ConstraintScope::GLOBAL;
    template_constraint.level = ConstraintLevel::ERROR;
    
    template_constraint.targets = {"@Style", "@Element", "@Var"};
    
    template_constraint.validator = [](const std::string& type, const std::unordered_map<std::string, std::string>&) {
        return ConstraintUtils::isValidTemplateType(type);
    };
    
    addConstraint(template_constraint);
}

void ConstraintSystem::setupCustomElementConstraints() {
    // 自定义类型约束
    ConstraintCondition custom_constraint;
    custom_constraint.name = PredefinedConstraints::CUSTOM_TYPE_CONSTRAINT;
    custom_constraint.type = ConstraintType::TYPE_CONSTRAINT;
    custom_constraint.scope = ConstraintScope::GLOBAL;
    custom_constraint.level = ConstraintLevel::ERROR;
    
    custom_constraint.targets = {"@Style", "@Element", "@Var"};
    
    custom_constraint.validator = [](const std::string& type, const std::unordered_map<std::string, std::string>&) {
        return ConstraintUtils::isValidCustomType(type);
    };
    
    addConstraint(custom_constraint);
}

void ConstraintSystem::setupImportConstraints() {
    // 导入类型约束
    ConstraintCondition import_constraint;
    import_constraint.name = PredefinedConstraints::IMPORT_TYPE_CONSTRAINT;
    import_constraint.type = ConstraintType::TYPE_CONSTRAINT;
    import_constraint.scope = ConstraintScope::GLOBAL;
    import_constraint.level = ConstraintLevel::ERROR;
    
    import_constraint.targets = {
        "@Html", "@Style", "@JavaScript", "@Chtl", "@CJmod", "@Config",
        "[Custom]", "[Template]", "[Origin]", "[Configuration]"
    };
    
    import_constraint.validator = [](const std::string& type, const std::unordered_map<std::string, std::string>&) {
        static std::vector<std::string> valid_types = {
            "@Html", "@Style", "@JavaScript", "@Chtl", "@CJmod", "@Config",
            "[Custom]", "[Template]", "[Origin]", "[Configuration]"
        };
        return std::find(valid_types.begin(), valid_types.end(), type) != valid_types.end();
    };
    
    addConstraint(import_constraint);
}

void ConstraintSystem::setupSyntaxConstraints() {
    // 选择器语法约束
    ConstraintCondition selector_constraint;
    selector_constraint.name = PredefinedConstraints::SELECTOR_SYNTAX_CONSTRAINT;
    selector_constraint.type = ConstraintType::SYNTAX_CONSTRAINT;
    selector_constraint.scope = ConstraintScope::LOCAL;
    selector_constraint.level = ConstraintLevel::WARNING;
    
    selector_constraint.validator = [](const std::string& selector, const std::unordered_map<std::string, std::string>&) {
        return ConstraintUtils::isValidSelector(selector);
    };
    
    addConstraint(selector_constraint);
    
    // 命名空间命名约束
    ConstraintCondition namespace_constraint;
    namespace_constraint.name = PredefinedConstraints::NAMESPACE_NAMING_CONSTRAINT;
    namespace_constraint.type = ConstraintType::NAMESPACE_CONSTRAINT;
    namespace_constraint.scope = ConstraintScope::GLOBAL;
    namespace_constraint.level = ConstraintLevel::ERROR;
    
    namespace_constraint.validator = [](const std::string& name, const std::unordered_map<std::string, std::string>&) {
        return ConstraintUtils::isValidIdentifier(name);
    };
    
    addConstraint(namespace_constraint);
}

void ConstraintSystem::addConstraint(const ConstraintCondition& constraint) {
    constraints[constraint.name] = constraint;
}

void ConstraintSystem::removeConstraint(const std::string& name) {
    constraints.erase(name);
}

bool ConstraintSystem::hasConstraint(const std::string& name) const {
    return constraints.find(name) != constraints.end();
}

const ConstraintCondition* ConstraintSystem::getConstraint(const std::string& name) const {
    auto it = constraints.find(name);
    return (it != constraints.end()) ? &it->second : nullptr;
}

void ConstraintSystem::addException(const std::string& constraint_name, const std::string& exception) {
    auto it = constraints.find(constraint_name);
    if (it != constraints.end()) {
        it->second.exceptions.push_back(exception);
    }
}

void ConstraintSystem::removeException(const std::string& constraint_name, const std::string& exception) {
    auto it = constraints.find(constraint_name);
    if (it != constraints.end()) {
        auto& exceptions = it->second.exceptions;
        exceptions.erase(std::remove(exceptions.begin(), exceptions.end(), exception), exceptions.end());
    }
}

bool ConstraintSystem::isException(const std::string& constraint_name, const std::string& target) const {
    auto it = constraints.find(constraint_name);
    if (it != constraints.end()) {
        const auto& exceptions = it->second.exceptions;
        return std::find(exceptions.begin(), exceptions.end(), target) != exceptions.end();
    }
    return false;
}

void ConstraintSystem::addNamespaceConstraint(const std::string& namespace_name, const std::string& constraint_name) {
    namespace_constraints[namespace_name].push_back(constraint_name);
}

void ConstraintSystem::removeNamespaceConstraint(const std::string& namespace_name, const std::string& constraint_name) {
    auto it = namespace_constraints.find(namespace_name);
    if (it != namespace_constraints.end()) {
        auto& constraints_vec = it->second;
        constraints_vec.erase(std::remove(constraints_vec.begin(), constraints_vec.end(), constraint_name), 
                             constraints_vec.end());
    }
}

std::vector<std::string> ConstraintSystem::getNamespaceConstraints(const std::string& namespace_name) const {
    auto it = namespace_constraints.find(namespace_name);
    return (it != namespace_constraints.end()) ? it->second : std::vector<std::string>{};
}

void ConstraintSystem::addGlobalConstraint(const std::string& constraint_name) {
    global_constraints["global"].push_back(constraint_name);
}

void ConstraintSystem::removeGlobalConstraint(const std::string& constraint_name) {
    auto& constraints_vec = global_constraints["global"];
    constraints_vec.erase(std::remove(constraints_vec.begin(), constraints_vec.end(), constraint_name), 
                         constraints_vec.end());
}

std::vector<std::string> ConstraintSystem::getGlobalConstraints() const {
    auto it = global_constraints.find("global");
    return (it != global_constraints.end()) ? it->second : std::vector<std::string>{};
}

ConstraintValidationResult ConstraintSystem::validateElement(const std::string& element_name, 
                                                           const std::string& context_type,
                                                           const ConstraintContext& context) const {
    ConstraintValidationResult result;
    
    // 检查HTML元素约束
    auto html_constraint = getConstraint(PredefinedConstraints::HTML_ELEMENT_WHITELIST);
    if (html_constraint && !isException(html_constraint->name, element_name)) {
        std::unordered_map<std::string, std::string> params;
        params["element"] = element_name;
        params["context"] = context_type;
        
        if (!html_constraint->validator(element_name, params)) {
            ConstraintViolation violation(
                "Invalid HTML element: " + element_name,
                element_name,
                html_constraint->level
            );
            violation.suggested_fix = ConstraintUtils::suggestValidElement(element_name);
            result.addViolation(violation);
        }
    }
    
    // 检查命名空间约束
    if (!context.current_namespace.empty()) {
        auto ns_constraints = getNamespaceConstraints(context.current_namespace);
        for (const auto& constraint_name : ns_constraints) {
            auto constraint = getConstraint(constraint_name);
            if (constraint && !isException(constraint_name, element_name)) {
                std::unordered_map<std::string, std::string> params;
                params["element"] = element_name;
                params["namespace"] = context.current_namespace;
                
                if (!constraint->validator(element_name, params)) {
                    ConstraintViolation violation(
                        "Element violates namespace constraint: " + constraint_name,
                        element_name,
                        constraint->level
                    );
                    result.addViolation(violation);
                }
            }
        }
    }
    
    return result;
}

ConstraintValidationResult ConstraintSystem::validateAttribute(const std::string& element_name,
                                                             const std::string& attribute_name,
                                                             const std::string& attribute_value,
                                                             const ConstraintContext& context) const {
    ConstraintValidationResult result;
    
    auto attr_constraint = getConstraint(PredefinedConstraints::ATTRIBUTE_VALIDATION_CONSTRAINT);
    if (attr_constraint && !isException(attr_constraint->name, attribute_name)) {
        std::unordered_map<std::string, std::string> params;
        params["element"] = element_name;
        params["attribute"] = attribute_name;
        params["value"] = attribute_value;
        
        if (!attr_constraint->validator(attribute_name, params)) {
            ConstraintViolation violation(
                "Invalid attribute '" + attribute_name + "' for element '" + element_name + "'",
                attribute_name,
                attr_constraint->level
            );
            violation.suggested_fix = ConstraintUtils::suggestValidAttribute(element_name, attribute_name);
            result.addViolation(violation);
        }
    }
    
    return result;
}

ConstraintValidationResult ConstraintSystem::validateSyntax(const std::string& syntax_element,
                                                          const std::string& usage_context,
                                                          const ConstraintContext& context) const {
    ConstraintValidationResult result;
    
    auto selector_constraint = getConstraint(PredefinedConstraints::SELECTOR_SYNTAX_CONSTRAINT);
    if (selector_constraint && usage_context == "selector" && 
        !isException(selector_constraint->name, syntax_element)) {
        
        std::unordered_map<std::string, std::string> params;
        params["selector"] = syntax_element;
        params["context"] = usage_context;
        
        if (!selector_constraint->validator(syntax_element, params)) {
            ConstraintViolation violation(
                "Invalid selector syntax: " + syntax_element,
                syntax_element,
                selector_constraint->level
            );
            result.addViolation(violation);
        }
    }
    
    return result;
}

ConstraintValidationResult ConstraintSystem::validateType(const std::string& type_name,
                                                        const std::string& usage_context,
                                                        const ConstraintContext& context) const {
    ConstraintValidationResult result;
    
    // 根据使用上下文选择约束
    std::string constraint_name;
    if (usage_context == "template") {
        constraint_name = PredefinedConstraints::TEMPLATE_TYPE_CONSTRAINT;
    } else if (usage_context == "custom") {
        constraint_name = PredefinedConstraints::CUSTOM_TYPE_CONSTRAINT;
    } else if (usage_context == "import") {
        constraint_name = PredefinedConstraints::IMPORT_TYPE_CONSTRAINT;
    }
    
    if (!constraint_name.empty()) {
        auto constraint = getConstraint(constraint_name);
        if (constraint && !isException(constraint_name, type_name)) {
            std::unordered_map<std::string, std::string> params;
            params["type"] = type_name;
            params["context"] = usage_context;
            
            if (!constraint->validator(type_name, params)) {
                ConstraintViolation violation(
                    "Invalid type '" + type_name + "' in " + usage_context + " context",
                    type_name,
                    constraint->level
                );
                violation.suggested_fix = ConstraintUtils::suggestValidType(type_name);
                result.addViolation(violation);
            }
        }
    }
    
    return result;
}

ConstraintValidationResult ConstraintSystem::validateAll(const std::vector<std::string>& elements,
                                                       const ConstraintContext& context) const {
    ConstraintValidationResult result;
    
    for (const auto& element : elements) {
        auto element_result = validateElement(element, context.current_context_type, context);
        for (const auto& violation : element_result.violations) {
            result.addViolation(violation);
        }
        result.warnings.insert(result.warnings.end(), 
                              element_result.warnings.begin(), 
                              element_result.warnings.end());
    }
    
    return result;
}

void ConstraintSystem::parseExceptStatement(const std::string& statement) {
    // 解析 except 语句，格式：except element1, element2, ...
    std::regex except_regex(R"(except\s+(.+))");
    std::smatch match;
    
    if (std::regex_match(statement, match, except_regex)) {
        std::string exceptions_str = match[1].str();
        auto exceptions = ConstraintUtils::extractExceptions(exceptions_str);
        
        // 将例外添加到当前激活的约束中
        // 这里需要根据上下文确定是哪个约束
        for (const auto& exception : exceptions) {
            // 默认添加到HTML元素约束
            addException(PredefinedConstraints::HTML_ELEMENT_WHITELIST, exception);
        }
    }
}

void ConstraintSystem::parseConstraintDefinition(const std::string& definition) {
    // 解析约束定义，这里可以实现自定义约束的解析逻辑
    // 格式示例：constraint custom_constraint for @Element except CustomBox, CustomCard
    
    std::regex constraint_regex(R"(constraint\s+(\w+)\s+for\s+(.+?)(?:\s+except\s+(.+))?$)");
    std::smatch match;
    
    if (std::regex_match(definition, match, constraint_regex)) {
        std::string constraint_name = match[1].str();
        std::string target = match[2].str();
        std::string exceptions_str = match[3].str();
        
        ConstraintCondition constraint;
        constraint.name = constraint_name;
        constraint.type = ConstraintType::CUSTOM_OBJECT_CONSTRAINT;
        constraint.scope = ConstraintScope::LOCAL;
        constraint.level = ConstraintLevel::WARNING;
        constraint.targets.push_back(target);
        
        if (!exceptions_str.empty()) {
            constraint.exceptions = ConstraintUtils::extractExceptions(exceptions_str);
        }
        
        addConstraint(constraint);
    }
}

std::vector<ConstraintViolation> ConstraintSystem::getViolationHistory() const {
    return violation_history;
}

void ConstraintSystem::clearViolationHistory() {
    violation_history.clear();
}

std::vector<std::string> ConstraintSystem::getSuggestions(const std::string& target, 
                                                        const std::string& context) const {
    std::vector<std::string> suggestions;
    
    if (context == "element") {
        std::string suggestion = ConstraintUtils::suggestValidElement(target);
        if (!suggestion.empty()) {
            suggestions.push_back(suggestion);
        }
    } else if (context == "type") {
        std::string suggestion = ConstraintUtils::suggestValidType(target);
        if (!suggestion.empty()) {
            suggestions.push_back(suggestion);
        }
    }
    
    return suggestions;
}

void ConstraintSystem::enableConstraint(const std::string& name) {
    auto it = constraints.find(name);
    if (it != constraints.end()) {
        it->second.parameters["enabled"] = "true";
    }
}

void ConstraintSystem::disableConstraint(const std::string& name) {
    auto it = constraints.find(name);
    if (it != constraints.end()) {
        it->second.parameters["enabled"] = "false";
    }
}

bool ConstraintSystem::isConstraintEnabled(const std::string& name) const {
    auto it = constraints.find(name);
    if (it != constraints.end()) {
        auto param_it = it->second.parameters.find("enabled");
        return (param_it == it->second.parameters.end()) || (param_it->second != "false");
    }
    return false;
}

std::string ConstraintSystem::generateConstraintReport() const {
    std::ostringstream report;
    report << "CHTL Constraint System Report\n";
    report << "=============================\n\n";
    
    report << "Total Constraints: " << constraints.size() << "\n";
    report << "Active Constraints:\n";
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        report << "  - " << constraint.name << " (";
        report << (isConstraintEnabled(constraint.name) ? "enabled" : "disabled");
        report << ", " << static_cast<int>(constraint.level) << ")\n";
        
        if (!constraint.exceptions.empty()) {
            report << "    Exceptions: ";
            for (size_t i = 0; i < constraint.exceptions.size(); ++i) {
                if (i > 0) report << ", ";
                report << constraint.exceptions[i];
            }
            report << "\n";
        }
    }
    
    return report.str();
}

std::string ConstraintSystem::generateViolationSummary() const {
    std::ostringstream summary;
    summary << "Constraint Violations Summary\n";
    summary << "============================\n\n";
    
    int error_count = 0, warning_count = 0, info_count = 0;
    
    for (const auto& violation : violation_history) {
        switch (violation.level) {
            case ConstraintLevel::ERROR: error_count++; break;
            case ConstraintLevel::WARNING: warning_count++; break;
            case ConstraintLevel::INFO: info_count++; break;
        }
    }
    
    summary << "Errors: " << error_count << "\n";
    summary << "Warnings: " << warning_count << "\n";
    summary << "Info: " << info_count << "\n\n";
    
    if (!violation_history.empty()) {
        summary << "Recent Violations:\n";
        for (const auto& violation : violation_history) {
            summary << "  - " << violation.message << " (" << violation.target << ")\n";
        }
    }
    
    return summary.str();
}

// ConstraintUtils实现
bool ConstraintUtils::isValidHTMLElement(const std::string& element_name) {
    static std::unordered_set<std::string> valid_elements = {
        "html", "head", "title", "meta", "link", "style", "script", "noscript",
        "body", "header", "nav", "main", "section", "article", "aside", "footer",
        "h1", "h2", "h3", "h4", "h5", "h6", "p", "hr", "pre", "blockquote",
        "div", "span", "a", "strong", "em", "small", "s", "cite", "q", "dfn",
        "abbr", "data", "time", "code", "var", "samp", "kbd", "sub", "sup",
        "i", "b", "u", "mark", "ruby", "rt", "rp", "bdi", "bdo", "br", "wbr",
        "ins", "del", "img", "iframe", "embed", "object", "param", "video",
        "audio", "source", "track", "canvas", "map", "area", "svg", "math",
        "table", "caption", "colgroup", "col", "tbody", "thead", "tfoot",
        "tr", "td", "th", "form", "fieldset", "legend", "label", "input",
        "button", "select", "datalist", "optgroup", "option", "textarea",
        "output", "progress", "meter", "details", "summary", "dialog",
        "ul", "ol", "li", "dl", "dt", "dd", "figure", "figcaption"
    };
    
    return valid_elements.find(element_name) != valid_elements.end();
}

bool ConstraintUtils::isValidHTMLAttribute(const std::string& element_name, const std::string& attribute_name) {
    // 全局属性
    static std::unordered_set<std::string> global_attributes = {
        "id", "class", "style", "title", "lang", "dir", "hidden", "tabindex",
        "accesskey", "contenteditable", "draggable", "dropzone", "spellcheck",
        "translate", "data-*", "aria-*"
    };
    
    if (global_attributes.find(attribute_name) != global_attributes.end()) {
        return true;
    }
    
    // data-* 和 aria-* 属性
    if (attribute_name.substr(0, 5) == "data-" || attribute_name.substr(0, 5) == "aria-") {
        return true;
    }
    
    // 元素特定属性的详细验证可以在这里实现
    // 为了简化，这里返回true
    return true;
}

std::vector<std::string> ConstraintUtils::getValidAttributesForElement(const std::string& element_name) {
    // 返回指定元素的有效属性列表
    // 这里可以实现详细的元素-属性映射
    std::vector<std::string> attributes = {"id", "class", "style", "title"};
    
    if (element_name == "a") {
        attributes.insert(attributes.end(), {"href", "target", "rel", "download"});
    } else if (element_name == "img") {
        attributes.insert(attributes.end(), {"src", "alt", "width", "height"});
    } else if (element_name == "input") {
        attributes.insert(attributes.end(), {"type", "name", "value", "placeholder"});
    }
    
    return attributes;
}

bool ConstraintUtils::isValidCustomType(const std::string& type_name) {
    static std::unordered_set<std::string> valid_types = {"@Style", "@Element", "@Var"};
    return valid_types.find(type_name) != valid_types.end();
}

bool ConstraintUtils::isValidTemplateType(const std::string& type_name) {
    return isValidCustomType(type_name); // 模板和自定义使用相同的类型
}

bool ConstraintUtils::isValidOriginType(const std::string& type_name) {
    static std::unordered_set<std::string> valid_types = {
        "@Html", "@Style", "@JavaScript", "@Vue", "@React", "@Angular"
    };
    return valid_types.find(type_name) != valid_types.end();
}

bool ConstraintUtils::isValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return false;
    
    // 标识符必须以字母或下划线开头，后跟字母、数字或下划线
    std::regex identifier_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(identifier, identifier_regex);
}

bool ConstraintUtils::isValidSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 基本CSS选择器验证
    std::regex selector_regex(R"([.#]?[a-zA-Z][a-zA-Z0-9_-]*|&|[a-zA-Z]+)");
    return std::regex_match(selector, selector_regex);
}

bool ConstraintUtils::isValidCSSProperty(const std::string& property) {
    // CSS属性验证的简化版本
    std::regex property_regex(R"([a-z-]+)");
    return std::regex_match(property, property_regex);
}

bool ConstraintUtils::matchesPattern(const std::string& target, const std::string& pattern) {
    try {
        std::regex regex_pattern(pattern);
        return std::regex_match(target, regex_pattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::vector<std::string> ConstraintUtils::extractExceptions(const std::string& except_statement) {
    std::vector<std::string> exceptions;
    std::istringstream iss(except_statement);
    std::string exception;
    
    while (std::getline(iss, exception, ',')) {
        // 移除前后空格
        exception.erase(0, exception.find_first_not_of(" \t"));
        exception.erase(exception.find_last_not_of(" \t") + 1);
        
        if (!exception.empty()) {
            exceptions.push_back(exception);
        }
    }
    
    return exceptions;
}

std::string ConstraintUtils::suggestValidElement(const std::string& invalid_element) {
    // 简单的建议生成逻辑
    if (invalid_element.length() < 3) return "";
    
    static std::vector<std::string> common_elements = {
        "div", "span", "p", "a", "img", "ul", "li", "h1", "h2", "h3",
        "button", "input", "form", "table", "tr", "td", "section", "article"
    };
    
    // 查找最相似的元素
    for (const auto& element : common_elements) {
        if (element.substr(0, 2) == invalid_element.substr(0, 2)) {
            return "Did you mean '" + element + "'?";
        }
    }
    
    return "Consider using a standard HTML element";
}

std::string ConstraintUtils::suggestValidAttribute(const std::string& element, const std::string& invalid_attr) {
    auto valid_attrs = getValidAttributesForElement(element);
    
    // 查找最相似的属性
    for (const auto& attr : valid_attrs) {
        if (attr.substr(0, 2) == invalid_attr.substr(0, 2)) {
            return "Did you mean '" + attr + "'?";
        }
    }
    
    return "Consider using: id, class, style, or title";
}

std::string ConstraintUtils::suggestValidType(const std::string& invalid_type) {
    static std::vector<std::string> valid_types = {
        "@Style", "@Element", "@Var", "@Html", "@JavaScript"
    };
    
    for (const auto& type : valid_types) {
        if (type.substr(0, 2) == invalid_type.substr(0, 2)) {
            return "Did you mean '" + type + "'?";
        }
    }
    
    return "Use @Style, @Element, or @Var";
}

} // namespace Common
} // namespace CHTL