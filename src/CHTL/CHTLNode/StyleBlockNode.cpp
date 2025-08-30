#include "StyleBlockNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {

// ========== StyleSelector 实现 ==========

StyleSelector::StyleSelector(StyleSelectorType t, const String& sel) : type(t), selector(sel) {
}

void StyleSelector::addClassName(const String& className) {
    if (std::find(classes.begin(), classes.end(), className) == classes.end()) {
        classes.push_back(className);
    }
}

void StyleSelector::addId(const String& idName) {
    if (std::find(ids.begin(), ids.end(), idName) == ids.end()) {
        ids.push_back(idName);
    }
}

void StyleSelector::setPseudoClass(const String& pseudo) {
    pseudoClass = pseudo;
}

void StyleSelector::setPseudoElement(const String& pseudo) {
    pseudoElement = pseudo;
}

void StyleSelector::setCombinator(const String& comb) {
    combinator = comb;
}

void StyleSelector::setParent(std::shared_ptr<StyleSelector> parentSelector) {
    parent = parentSelector;
}

void StyleSelector::addChild(std::shared_ptr<StyleSelector> childSelector) {
    children.push_back(childSelector);
    childSelector->setParent(shared_from_this());
}

std::shared_ptr<StyleSelector> StyleSelector::parseSelector(const String& selectorString) {
    auto selector = std::make_shared<StyleSelector>();
    
    // 简化的选择器解析
    String trimmed = selectorString;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty()) {
        return selector;
    }
    
    // 检测自引用
    if (trimmed[0] == '&') {
        selector->type = StyleSelectorType::SELF_REFERENCE;
        selector->selector = trimmed;
        return selector;
    }
    
    // 检测类选择器
    if (trimmed[0] == '.') {
        selector->type = StyleSelectorType::CLASS;
        selector->addClassName(trimmed.substr(1));
        selector->selector = trimmed;
        return selector;
    }
    
    // 检测ID选择器
    if (trimmed[0] == '#') {
        selector->type = StyleSelectorType::ID;
        selector->addId(trimmed.substr(1));
        selector->selector = trimmed;
        return selector;
    }
    
    // 检测通用选择器
    if (trimmed == "*") {
        selector->type = StyleSelectorType::UNIVERSAL;
        selector->selector = trimmed;
        return selector;
    }
    
    // 默认为元素选择器
    selector->type = StyleSelectorType::ELEMENT;
    selector->element = trimmed;
    selector->selector = trimmed;
    
    return selector;
}

StyleSelectorType StyleSelector::getSelectorType(const String& selectorString) {
    if (selectorString.empty()) return StyleSelectorType::ELEMENT;
    
    if (selectorString[0] == '&') return StyleSelectorType::SELF_REFERENCE;
    if (selectorString[0] == '.') return StyleSelectorType::CLASS;
    if (selectorString[0] == '#') return StyleSelectorType::ID;
    if (selectorString == "*") return StyleSelectorType::UNIVERSAL;
    if (selectorString[0] == '[') return StyleSelectorType::ATTRIBUTE;
    if (selectorString[0] == ':') {
        if (selectorString.find("::") == 0) return StyleSelectorType::PSEUDO_ELEMENT;
        return StyleSelectorType::PSEUDO_CLASS;
    }
    
    return StyleSelectorType::ELEMENT;
}

String StyleSelector::toString() const {
    return selector;
}

String StyleSelector::toCSS() const {
    std::ostringstream oss;
    
    // 基础选择器
    oss << selector;
    
    // 添加伪类
    if (!pseudoClass.empty()) {
        oss << ":" << pseudoClass;
    }
    
    // 添加伪元素
    if (!pseudoElement.empty()) {
        oss << "::" << pseudoElement;
    }
    
    return oss.str();
}

String StyleSelector::getSpecificityString() const {
    int spec = getSpecificity();
    int a = (spec / 10000) % 10;
    int b = (spec / 100) % 100;
    int c = spec % 100;
    
    return std::to_string(a) + "," + std::to_string(b) + "," + std::to_string(c);
}

int StyleSelector::getSpecificity() const {
    return calculateSpecificity();
}

bool StyleSelector::matches(const String& element, const StringVector& classes, const String& id) const {
    switch (type) {
        case StyleSelectorType::ELEMENT:
            return this->element == element;
        case StyleSelectorType::CLASS:
            return std::find(classes.begin(), classes.end(), this->classes[0]) != classes.end();
        case StyleSelectorType::ID:
            return this->ids[0] == id;
        case StyleSelectorType::UNIVERSAL:
            return true;
        default:
            return false;
    }
}

bool StyleSelector::isDescendantOf(const StyleSelector& ancestor) const {
    auto current = parent;
    while (current) {
        if (current->selector == ancestor.selector) {
            return true;
        }
        current = current->parent;
    }
    return false;
}

void StyleSelector::optimize() {
    // 去重类名和ID
    std::sort(classes.begin(), classes.end());
    classes.erase(std::unique(classes.begin(), classes.end()), classes.end());
    
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
}

bool StyleSelector::canBeMerged(const StyleSelector& other) const {
    return type == other.type && element == other.element;
}

std::shared_ptr<StyleSelector> StyleSelector::merge(const StyleSelector& other) const {
    if (!canBeMerged(other)) {
        return nullptr;
    }
    
    auto merged = std::make_shared<StyleSelector>(type, selector);
    merged->element = element;
    merged->classes = classes;
    merged->ids = ids;
    
    // 合并类名
    for (const String& className : other.classes) {
        merged->addClassName(className);
    }
    
    // 合并ID
    for (const String& idName : other.ids) {
        merged->addId(idName);
    }
    
    return merged;
}

int StyleSelector::calculateSpecificity() const {
    int specificity = 0;
    
    // ID选择器: 100分
    specificity += ids.size() * 100;
    
    // 类选择器、属性选择器、伪类: 10分
    specificity += classes.size() * 10;
    if (!pseudoClass.empty()) specificity += 10;
    
    // 元素选择器、伪元素: 1分
    if (!element.empty()) specificity += 1;
    if (!pseudoElement.empty()) specificity += 1;
    
    return specificity;
}

// ========== StyleRule 实现 ==========

StyleRule::StyleRule(std::shared_ptr<StyleSelector> sel) 
    : selector(sel), isNested(false), specificity(0), sourceOrder(0) {
    if (selector) {
        updateSpecificity();
    }
}

void StyleRule::addProperty(const String& name, const String& value, bool important) {
    StyleProperty prop(name, value);
    prop.isImportant = important;
    properties.push_back(prop);
}

void StyleRule::addProperty(const StyleProperty& property) {
    properties.push_back(property);
}

void StyleRule::removeProperty(const String& name) {
    properties.erase(
        std::remove_if(properties.begin(), properties.end(),
                      [&name](const StyleProperty& prop) { return prop.name == name; }),
        properties.end()
    );
}

StyleProperty* StyleRule::getProperty(const String& name) {
    for (auto& prop : properties) {
        if (prop.name == name) {
            return &prop;
        }
    }
    return nullptr;
}

const StyleProperty* StyleRule::getProperty(const String& name) const {
    for (const auto& prop : properties) {
        if (prop.name == name) {
            return &prop;
        }
    }
    return nullptr;
}

bool StyleRule::hasProperty(const String& name) const {
    return getProperty(name) != nullptr;
}

void StyleRule::addMediaQuery(const String& query) {
    mediaQueries.push_back(query);
}

void StyleRule::setMediaQueries(const StringVector& queries) {
    mediaQueries = queries;
}

String StyleRule::toString() const {
    std::ostringstream oss;
    
    if (selector) {
        oss << selector->toCSS();
    }
    
    oss << " {\n";
    
    for (const auto& prop : properties) {
        oss << "  " << prop.name << ": " << prop.value;
        if (prop.isImportant) {
            oss << " !important";
        }
        oss << ";\n";
    }
    
    oss << "}";
    
    return oss.str();
}

String StyleRule::toCSS(int indentLevel) const {
    std::ostringstream oss;
    String indent(indentLevel * 2, ' ');
    
    if (selector) {
        oss << indent << selector->toCSS() << " {\n";
    }
    
    for (const auto& prop : properties) {
        oss << indent << "  " << prop.name << ": " << prop.value;
        if (prop.isImportant) {
            oss << " !important";
        }
        oss << ";\n";
    }
    
    if (selector) {
        oss << indent << "}\n";
    }
    
    return oss.str();
}

String StyleRule::toCSSWithMediaQuery() const {
    String css = toCSS();
    
    if (!mediaQueries.empty()) {
        std::ostringstream oss;
        for (const String& query : mediaQueries) {
            oss << "@media " << query << " {\n";
            oss << css;
            oss << "}\n";
        }
        return oss.str();
    }
    
    return css;
}

bool StyleRule::canBeMerged(const StyleRule& other) const {
    if (!selector || !other.selector) {
        return false;
    }
    
    return selector->canBeMerged(*other.selector) && 
           mediaQueries == other.mediaQueries &&
           namespace_ == other.namespace_;
}

std::shared_ptr<StyleRule> StyleRule::merge(const StyleRule& other) const {
    if (!canBeMerged(other)) {
        return nullptr;
    }
    
    auto merged = std::make_shared<StyleRule>(selector);
    merged->properties = properties;
    merged->mediaQueries = mediaQueries;
    merged->namespace_ = namespace_;
    merged->isNested = isNested;
    
    // 合并属性，other的属性覆盖当前的
    for (const auto& prop : other.properties) {
        merged->removeProperty(prop.name);
        merged->addProperty(prop);
    }
    
    merged->updateSpecificity();
    return merged;
}

void StyleRule::optimize() {
    deduplicateProperties();
    updateSpecificity();
}

void StyleRule::deduplicateProperties() {
    // 保留最后一个同名属性
    std::unordered_map<String, size_t> lastOccurrence;
    
    for (size_t i = 0; i < properties.size(); ++i) {
        lastOccurrence[properties[i].name] = i;
    }
    
    std::vector<StyleProperty> uniqueProperties;
    for (size_t i = 0; i < properties.size(); ++i) {
        if (lastOccurrence[properties[i].name] == i) {
            uniqueProperties.push_back(properties[i]);
        }
    }
    
    properties = uniqueProperties;
}

bool StyleRule::isValid() const {
    if (!selector) {
        return false;
    }
    
    for (const auto& prop : properties) {
        if (prop.name.empty() || prop.value.empty()) {
            return false;
        }
    }
    
    return true;
}

StringVector StyleRule::getValidationErrors() const {
    StringVector errors;
    
    if (!selector) {
        errors.push_back("选择器不能为空");
    }
    
    for (const auto& prop : properties) {
        if (prop.name.empty()) {
            errors.push_back("属性名不能为空");
        }
        if (prop.value.empty()) {
            errors.push_back("属性值不能为空: " + prop.name);
        }
        if (!isValidProperty(prop.name, prop.value)) {
            errors.push_back("无效的CSS属性: " + prop.name + ": " + prop.value);
        }
    }
    
    return errors;
}

void StyleRule::updateSpecificity() {
    calculateSpecificity();
}

void StyleRule::calculateSpecificity() {
    if (selector) {
        specificity = selector->getSpecificity();
    }
}

bool StyleRule::isValidProperty(const String& name, const String& value) const {
    // 基本的CSS属性验证
    static const StringUnorderedSet validProperties = {
        "color", "background-color", "font-size", "font-family", "font-weight",
        "margin", "padding", "border", "width", "height", "display", "position",
        "top", "right", "bottom", "left", "float", "clear", "overflow",
        "text-align", "vertical-align", "line-height", "text-decoration",
        "opacity", "visibility", "z-index", "cursor", "box-shadow", "border-radius"
    };
    
    return !name.empty() && !value.empty() && 
           (validProperties.find(name) != validProperties.end() || 
            name.find("-") != String::npos); // 允许vendor前缀
}

// ========== StyleBlockNode 实现 ==========

StyleBlockNode::StyleBlockNode(const Position& pos) 
    : BaseNode(NodeType::STYLE, "", pos), isInline(false), autoAddClass(true), autoAddId(false) {
}

void StyleBlockNode::addRule(std::shared_ptr<StyleRule> rule) {
    if (rule && rule->isValid()) {
        rule->sourceOrder = rules.size();
        rules.push_back(rule);
    }
}

void StyleBlockNode::removeRule(size_t index) {
    if (index < rules.size()) {
        rules.erase(rules.begin() + index);
    }
}

void StyleBlockNode::removeRule(const String& selectorString) {
    rules.erase(
        std::remove_if(rules.begin(), rules.end(),
                      [&selectorString](const std::shared_ptr<StyleRule>& rule) {
                          return rule->selector && rule->selector->toString() == selectorString;
                      }),
        rules.end()
    );
}

std::shared_ptr<StyleRule> StyleBlockNode::getRule(size_t index) const {
    return index < rules.size() ? rules[index] : nullptr;
}

std::shared_ptr<StyleRule> StyleBlockNode::getRule(const String& selectorString) const {
    for (const auto& rule : rules) {
        if (rule->selector && rule->selector->toString() == selectorString) {
            return rule;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<StyleRule>> StyleBlockNode::getRulesBySelector(const String& selectorPattern) const {
    std::vector<std::shared_ptr<StyleRule>> matching;
    std::regex pattern(selectorPattern);
    
    for (const auto& rule : rules) {
        if (rule->selector && std::regex_match(rule->selector->toString(), pattern)) {
            matching.push_back(rule);
        }
    }
    
    return matching;
}

void StyleBlockNode::addStyle(const String& selector, const String& property, const String& value) {
    auto styleSelector = StyleSelector::parseSelector(selector);
    auto rule = std::make_shared<StyleRule>(styleSelector);
    rule->addProperty(property, value);
    addRule(rule);
}

void StyleBlockNode::addClassStyle(const String& className, const String& property, const String& value) {
    addStyle("." + className, property, value);
}

void StyleBlockNode::addIdStyle(const String& idName, const String& property, const String& value) {
    addStyle("#" + idName, property, value);
}

void StyleBlockNode::addElementStyle(const String& element, const String& property, const String& value) {
    addStyle(element, property, value);
}

void StyleBlockNode::addPseudoStyle(const String& selector, const String& pseudo, const String& property, const String& value) {
    addStyle(selector + ":" + pseudo, property, value);
}

void StyleBlockNode::addSelfReferenceStyle(const String& pseudoClass, const String& property, const String& value) {
    String selector = "&";
    if (!pseudoClass.empty()) {
        selector += ":" + pseudoClass;
    }
    addStyle(selector, property, value);
}

void StyleBlockNode::addSelfReferenceStyle(const String& property, const String& value) {
    addSelfReferenceStyle("", property, value);
}

std::shared_ptr<StyleRule> StyleBlockNode::createSelfReferenceRule(const String& pseudoClass) {
    String selector = "&";
    if (!pseudoClass.empty()) {
        selector += ":" + pseudoClass;
    }
    
    auto styleSelector = StyleSelector::parseSelector(selector);
    return std::make_shared<StyleRule>(styleSelector);
}

String StyleBlockNode::resolveSelfReference(const String& selector, const String& parentSelector) const {
    if (selector.find('&') == String::npos) {
        return selector;
    }
    
    String resolved = selector;
    size_t pos = 0;
    while ((pos = resolved.find('&', pos)) != String::npos) {
        resolved.replace(pos, 1, parentSelector);
        pos += parentSelector.length();
    }
    
    return resolved;
}

void StyleBlockNode::processNestedStyles() {
    // 处理嵌套样式，展开到平级规则
    std::vector<std::shared_ptr<StyleRule>> expandedRules;
    
    for (const auto& rule : rules) {
        if (rule->isNested) {
            auto expanded = expandRule(rule);
            expandedRules.insert(expandedRules.end(), expanded.begin(), expanded.end());
        } else {
            expandedRules.push_back(rule);
        }
    }
    
    rules = expandedRules;
}

void StyleBlockNode::flattenNestedRules() {
    processNestedStyles();
}

void StyleBlockNode::expandSelfReferences(const String& parentSelector) {
    for (auto& rule : rules) {
        if (rule->selector && rule->selector->type == StyleSelectorType::SELF_REFERENCE) {
            String newSelector = resolveSelfReference(rule->selector->selector, parentSelector);
            rule->selector = StyleSelector::parseSelector(newSelector);
            rule->updateSpecificity();
        }
    }
}

std::vector<std::shared_ptr<StyleRule>> StyleBlockNode::expandRule(std::shared_ptr<StyleRule> rule, const String& parentSelector) const {
    std::vector<std::shared_ptr<StyleRule>> expanded;
    
    if (!rule->selector) {
        return expanded;
    }
    
    if (rule->selector->type == StyleSelectorType::SELF_REFERENCE) {
        String newSelector = resolveSelfReference(rule->selector->selector, parentSelector);
        auto newRule = std::make_shared<StyleRule>(StyleSelector::parseSelector(newSelector));
        newRule->properties = rule->properties;
        newRule->mediaQueries = rule->mediaQueries;
        newRule->namespace_ = rule->namespace_;
        newRule->updateSpecificity();
        expanded.push_back(newRule);
    } else {
        expanded.push_back(rule);
    }
    
    return expanded;
}

void StyleBlockNode::addVariable(const String& name, const String& value) {
    variables[name] = value;
}

void StyleBlockNode::removeVariable(const String& name) {
    variables.erase(name);
}

String StyleBlockNode::getVariable(const String& name) const {
    auto it = variables.find(name);
    return it != variables.end() ? it->second : "";
}

bool StyleBlockNode::hasVariable(const String& name) const {
    return variables.find(name) != variables.end();
}

void StyleBlockNode::expandVariables() {
    for (auto& rule : rules) {
        for (auto& prop : rule->properties) {
            prop.value = expandVariableReferences(prop.value);
        }
    }
}

String StyleBlockNode::expandVariableReferences(const String& value) const {
    String result = value;
    
    // 展开CSS变量引用 var(--variable-name)
    std::regex varRegex(R"(var\(--([^)]+)\))");
    std::smatch match;
    
    while (std::regex_search(result, match, varRegex)) {
        String varName = match[1].str();
        String varValue = getVariable(varName);
        
        if (!varValue.empty()) {
            result = std::regex_replace(result, std::regex("var\\(--" + varName + "\\)"), varValue);
        } else {
            break; // 避免无限循环
        }
    }
    
    return result;
}

void StyleBlockNode::addImport(const String& url, const String& mediaQuery) {
    String importRule = "@import url('" + url + "')";
    if (!mediaQuery.empty()) {
        importRule += " " + mediaQuery;
    }
    importRule += ";";
    
    imports.push_back(importRule);
}

void StyleBlockNode::addFontFace(const String& fontFamily, const String& src, const StringUnorderedMap& properties) {
    std::ostringstream oss;
    oss << "@font-face {\n";
    oss << "  font-family: '" << fontFamily << "';\n";
    oss << "  src: " << src << ";\n";
    
    for (const auto& [prop, value] : properties) {
        oss << "  " << prop << ": " << value << ";\n";
    }
    
    oss << "}";
    fontFaces.push_back(oss.str());
}

void StyleBlockNode::addKeyframes(const String& name, const String& keyframeRules) {
    std::ostringstream oss;
    oss << "@keyframes " << name << " {\n";
    oss << keyframeRules;
    oss << "\n}";
    
    keyframes.push_back(oss.str());
}

void StyleBlockNode::applyAutoClassNames(const String& baseSelector) {
    if (!autoAddClass) return;
    
    // 为没有类名的元素自动添加类名
    for (auto& rule : rules) {
        if (rule->selector && 
            rule->selector->type == StyleSelectorType::ELEMENT && 
            rule->selector->classes.empty()) {
            
            String className = generateUniqueClassName();
            rule->selector->addClassName(className);
            rule->selector->type = StyleSelectorType::CLASS;
            rule->selector->selector = "." + className;
            rule->updateSpecificity();
        }
    }
}

void StyleBlockNode::applyAutoIds(const String& baseSelector) {
    if (!autoAddId) return;
    
    // 为特定的规则自动添加ID
    for (auto& rule : rules) {
        if (rule->selector && 
            rule->selector->type == StyleSelectorType::ELEMENT && 
            rule->selector->ids.empty()) {
            
            String idName = generateUniqueId();
            rule->selector->addId(idName);
            rule->selector->type = StyleSelectorType::ID;
            rule->selector->selector = "#" + idName;
            rule->updateSpecificity();
        }
    }
}

void StyleBlockNode::applyScopeSelector() {
    if (scopeSelector.empty()) return;
    
    for (auto& rule : rules) {
        if (rule->selector) {
            String newSelector = scopeSelector + " " + rule->selector->selector;
            rule->selector = StyleSelector::parseSelector(newSelector);
            rule->updateSpecificity();
        }
    }
}

void StyleBlockNode::optimize() {
    removeEmptyRules();
    mergeCompatibleRules();
    deduplicateRules();
    sortRulesBySpecificity();
    expandVariables();
}

void StyleBlockNode::mergeCompatibleRules() {
    std::vector<std::shared_ptr<StyleRule>> optimizedRules;
    
    for (const auto& rule : rules) {
        bool merged = false;
        
        for (auto& optimizedRule : optimizedRules) {
            if (optimizedRule->canBeMerged(*rule)) {
                auto mergedRule = optimizedRule->merge(*rule);
                if (mergedRule) {
                    optimizedRule = mergedRule;
                    merged = true;
                    break;
                }
            }
        }
        
        if (!merged) {
            optimizedRules.push_back(rule);
        }
    }
    
    rules = optimizedRules;
}

void StyleBlockNode::removeEmptyRules() {
    rules.erase(
        std::remove_if(rules.begin(), rules.end(),
                      [](const std::shared_ptr<StyleRule>& rule) {
                          return !rule || rule->properties.empty();
                      }),
        rules.end()
    );
}

void StyleBlockNode::sortRulesBySpecificity() {
    std::stable_sort(rules.begin(), rules.end(),
                    [](const std::shared_ptr<StyleRule>& a, const std::shared_ptr<StyleRule>& b) {
                        if (a->specificity != b->specificity) {
                            return a->specificity < b->specificity;
                        }
                        return a->sourceOrder < b->sourceOrder;
                    });
}

void StyleBlockNode::deduplicateRules() {
    for (auto& rule : rules) {
        rule->optimize();
    }
}

void StyleBlockNode::minifyPropertyValues() {
    for (auto& rule : rules) {
        for (auto& prop : rule->properties) {
            // 简化的值压缩
            if (prop.value == "0px" || prop.value == "0em" || prop.value == "0rem") {
                prop.value = "0";
            }
        }
    }
}

String StyleBlockNode::toCSS(int indentLevel) const {
    std::ostringstream oss;
    String indent(indentLevel * 2, ' ');
    
    // 添加导入
    for (const String& import : imports) {
        oss << indent << import << "\n";
    }
    
    // 添加字体定义
    for (const String& fontFace : fontFaces) {
        oss << indent << fontFace << "\n";
    }
    
    // 添加关键帧动画
    for (const String& keyframe : keyframes) {
        oss << indent << keyframe << "\n";
    }
    
    // 添加CSS变量
    if (!variables.empty()) {
        oss << indent << ":root {\n";
        for (const auto& [name, value] : variables) {
            oss << indent << "  --" << name << ": " << value << ";\n";
        }
        oss << indent << "}\n";
    }
    
    // 添加样式规则
    for (const auto& rule : rules) {
        oss << rule->toCSS(indentLevel);
        if (!rule->mediaQueries.empty()) {
            // 处理媒体查询
            for (const String& query : rule->mediaQueries) {
                oss << indent << "@media " << query << " {\n";
                oss << rule->toCSS(indentLevel + 1);
                oss << indent << "}\n";
            }
        }
    }
    
    return oss.str();
}

String StyleBlockNode::toInlineCSS() const {
    std::ostringstream oss;
    
    for (const auto& rule : rules) {
        for (const auto& prop : rule->properties) {
            if (oss.tellp() > 0) oss << " ";
            oss << prop.name << ": " << prop.value << ";";
        }
    }
    
    return oss.str();
}

String StyleBlockNode::toCSSWithMediaQueries() const {
    return toCSS();
}

String StyleBlockNode::generateScopedCSS(const String& scopePrefix) const {
    std::ostringstream oss;
    
    for (const auto& rule : rules) {
        if (rule->selector) {
            String scopedSelector = scopePrefix + " " + rule->selector->selector;
            oss << scopedSelector << " {\n";
            
            for (const auto& prop : rule->properties) {
                oss << "  " << prop.name << ": " << prop.value;
                if (prop.isImportant) {
                    oss << " !important";
                }
                oss << ";\n";
            }
            
            oss << "}\n";
        }
    }
    
    return oss.str();
}

bool StyleBlockNode::validateStyles() const {
    for (const auto& rule : rules) {
        if (!rule->isValid()) {
            return false;
        }
    }
    return true;
}

StringVector StyleBlockNode::getStyleValidationErrors() const {
    StringVector allErrors;
    
    for (size_t i = 0; i < rules.size(); ++i) {
        auto errors = rules[i]->getValidationErrors();
        for (const String& error : errors) {
            allErrors.push_back("规则 " + std::to_string(i) + ": " + error);
        }
    }
    
    return allErrors;
}

bool StyleBlockNode::hasConflictingRules() const {
    return !getConflictingRulePairs().empty();
}

std::vector<std::pair<size_t, size_t>> StyleBlockNode::getConflictingRulePairs() const {
    std::vector<std::pair<size_t, size_t>> conflicts;
    
    for (size_t i = 0; i < rules.size(); ++i) {
        for (size_t j = i + 1; j < rules.size(); ++j) {
            if (rules[i]->selector && rules[j]->selector &&
                rules[i]->selector->selector == rules[j]->selector->selector) {
                // 检查是否有相同的属性
                for (const auto& prop1 : rules[i]->properties) {
                    for (const auto& prop2 : rules[j]->properties) {
                        if (prop1.name == prop2.name && prop1.value != prop2.value) {
                            conflicts.push_back({i, j});
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return conflicts;
}

std::unordered_map<String, size_t> StyleBlockNode::getPropertyUsageStats() const {
    std::unordered_map<String, size_t> stats;
    
    for (const auto& rule : rules) {
        for (const auto& prop : rule->properties) {
            stats[prop.name]++;
        }
    }
    
    return stats;
}

std::unordered_map<String, size_t> StyleBlockNode::getSelectorUsageStats() const {
    std::unordered_map<String, size_t> stats;
    
    for (const auto& rule : rules) {
        if (rule->selector) {
            stats[rule->selector->selector]++;
        }
    }
    
    return stats;
}

size_t StyleBlockNode::getTotalDeclarations() const {
    size_t total = 0;
    for (const auto& rule : rules) {
        total += rule->properties.size();
    }
    return total;
}

double StyleBlockNode::getAverageSpecificity() const {
    if (rules.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& rule : rules) {
        total += rule->specificity;
    }
    
    return total / rules.size();
}

String StyleBlockNode::getValue() const {
    return toCSS();
}

std::unique_ptr<BaseNode> StyleBlockNode::clone() const {
    auto cloned = std::make_unique<StyleBlockNode>(position);
    
    // 深拷贝规则
    for (const auto& rule : rules) {
        auto clonedRule = std::make_shared<StyleRule>(rule->selector);
        clonedRule->properties = rule->properties;
        clonedRule->mediaQueries = rule->mediaQueries;
        clonedRule->namespace_ = rule->namespace_;
        clonedRule->isNested = rule->isNested;
        clonedRule->specificity = rule->specificity;
        clonedRule->sourceOrder = rule->sourceOrder;
        cloned->addRule(clonedRule);
    }
    
    cloned->blockType = blockType;
    cloned->isInline = isInline;
    cloned->autoAddClass = autoAddClass;
    cloned->autoAddId = autoAddId;
    cloned->scopeSelector = scopeSelector;
    cloned->variables = variables;
    cloned->imports = imports;
    cloned->fontFaces = fontFaces;
    cloned->keyframes = keyframes;
    
    return cloned;
}

std::unique_ptr<StyleBlockNode> StyleBlockNode::createInlineStyle() {
    auto styleBlock = std::make_unique<StyleBlockNode>();
    styleBlock->isInline = true;
    styleBlock->blockType = "inline";
    return styleBlock;
}

std::unique_ptr<StyleBlockNode> StyleBlockNode::createExternalStyle() {
    auto styleBlock = std::make_unique<StyleBlockNode>();
    styleBlock->isInline = false;
    styleBlock->blockType = "external";
    return styleBlock;
}

std::unique_ptr<StyleBlockNode> StyleBlockNode::createScopedStyle(const String& scope) {
    auto styleBlock = std::make_unique<StyleBlockNode>();
    styleBlock->setScopeSelector(scope);
    return styleBlock;
}

std::shared_ptr<StyleRule> StyleBlockNode::parseStyleRule(const String& ruleString) {
    // 简化的CSS规则解析
    size_t bracePos = ruleString.find('{');
    if (bracePos == String::npos) {
        return nullptr;
    }
    
    String selectorStr = ruleString.substr(0, bracePos);
    selectorStr.erase(0, selectorStr.find_first_not_of(" \t"));
    selectorStr.erase(selectorStr.find_last_not_of(" \t") + 1);
    
    String declarationsStr = ruleString.substr(bracePos + 1);
    size_t closeBracePos = declarationsStr.rfind('}');
    if (closeBracePos != String::npos) {
        declarationsStr = declarationsStr.substr(0, closeBracePos);
    }
    
    auto selector = StyleSelector::parseSelector(selectorStr);
    auto rule = std::make_shared<StyleRule>(selector);
    
    // 解析属性声明
    std::istringstream iss(declarationsStr);
    String declaration;
    while (std::getline(iss, declaration, ';')) {
        declaration.erase(0, declaration.find_first_not_of(" \t\n"));
        declaration.erase(declaration.find_last_not_of(" \t\n") + 1);
        
        if (!declaration.empty()) {
            auto prop = parseStyleProperty(declaration);
            if (!prop.name.empty()) {
                rule->addProperty(prop);
            }
        }
    }
    
    return rule;
}

StyleProperty StyleBlockNode::parseStyleProperty(const String& propertyString) {
    size_t colonPos = propertyString.find(':');
    if (colonPos == String::npos) {
        return StyleProperty();
    }
    
    String name = propertyString.substr(0, colonPos);
    String value = propertyString.substr(colonPos + 1);
    
    name.erase(0, name.find_first_not_of(" \t"));
    name.erase(name.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    StyleProperty prop(name, value);
    
    // 检查 !important
    if (value.find("!important") != String::npos) {
        prop.isImportant = true;
        size_t importantPos = value.find("!important");
        prop.value = value.substr(0, importantPos);
        prop.value.erase(prop.value.find_last_not_of(" \t") + 1);
    }
    
    return prop;
}

String StyleBlockNode::generateUniqueClassName() const {
    static size_t counter = 0;
    return "chtl-auto-class-" + std::to_string(++counter);
}

String StyleBlockNode::generateUniqueId() const {
    static size_t counter = 0;
    return "chtl-auto-id-" + std::to_string(++counter);
}

String StyleBlockNode::normalizeSelector(const String& selector) const {
    String normalized = selector;
    
    // 移除多余的空白
    normalized = std::regex_replace(normalized, std::regex(R"(\s+)"), " ");
    
    // 移除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    return normalized;
}

int StyleBlockNode::calculateRuleSpecificity(const StyleRule& rule) const {
    return rule.selector ? rule.selector->getSpecificity() : 0;
}

// ========== StyleProcessor 静态方法实现 ==========

String StyleProcessor::processInlineStyle(const String& styleContent, const String& elementSelector) {
    // 处理内联样式
    std::ostringstream oss;
    
    if (!elementSelector.empty()) {
        oss << elementSelector << " { " << styleContent << " }";
    } else {
        oss << styleContent;
    }
    
    return oss.str();
}

String StyleProcessor::processBlockStyle(const StyleBlockNode& styleBlock, const String& scopePrefix) {
    String css = styleBlock.toCSS();
    
    if (!scopePrefix.empty()) {
        css = styleBlock.generateScopedCSS(scopePrefix);
    }
    
    return css;
}

String StyleProcessor::expandSelfReferences(const String& css, const String& parentSelector) {
    String result = css;
    
    // 替换&引用
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != String::npos) {
        result.replace(pos, 1, parentSelector);
        pos += parentSelector.length();
    }
    
    return result;
}

String StyleProcessor::optimizeCSS(const String& css) {
    String optimized = css;
    
    // 移除注释
    optimized = removeComments(optimized);
    
    // 移除多余空白
    optimized = std::regex_replace(optimized, std::regex(R"(\s+)"), " ");
    
    // 移除重复规则
    optimized = removeDuplicateRules(optimized);
    
    return optimized;
}

String StyleProcessor::combineSelectors(const String& parent, const String& child, const String& combinator) {
    if (parent.empty()) return child;
    if (child.empty()) return parent;
    
    return parent + combinator + child;
}

String StyleProcessor::normalizeSelector(const String& selector) {
    String normalized = selector;
    
    // 移除多余空白
    normalized = std::regex_replace(normalized, std::regex(R"(\s+)"), " ");
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    return normalized;
}

String StyleProcessor::minifyCSS(const String& css) {
    String minified = css;
    
    // 移除注释
    minified = removeComments(minified);
    
    // 移除不必要的空白
    minified = std::regex_replace(minified, std::regex(R"(\s*{\s*)"), "{");
    minified = std::regex_replace(minified, std::regex(R"(\s*}\s*)"), "}");
    minified = std::regex_replace(minified, std::regex(R"(\s*:\s*)"), ":");
    minified = std::regex_replace(minified, std::regex(R"(\s*;\s*)"), ";");
    minified = std::regex_replace(minified, std::regex(R"(\s*,\s*)"), ",");
    
    // 移除最后一个分号
    minified = std::regex_replace(minified, std::regex(R"(;})"), "}");
    
    return minified;
}

String StyleProcessor::removeComments(const String& css) {
    String result = css;
    
    // 移除 /* */ 注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "", std::regex_constants::format_default);
    
    return result;
}

String StyleProcessor::removeDuplicateRules(const String& css) {
    // 简化的重复规则移除
    return css; // TODO: 实现更复杂的重复规则检测
}

bool StyleProcessor::validateCSS(const String& css) {
    // 基本的CSS验证
    if (css.empty()) return true;
    
    // 检查括号匹配
    int braceCount = 0;
    for (char c : css) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

StringVector StyleProcessor::getCSSErrors(const String& css) {
    StringVector errors;
    
    if (!validateCSS(css)) {
        errors.push_back("CSS语法错误：括号不匹配");
    }
    
    return errors;
}

bool StyleProcessor::isValidSelector(const String& selector) {
    return !selector.empty() && selector.find_first_not_of(" \t") != String::npos;
}

bool StyleProcessor::isValidProperty(const String& property, const String& value) {
    return !property.empty() && !value.empty();
}

} // namespace CHTL