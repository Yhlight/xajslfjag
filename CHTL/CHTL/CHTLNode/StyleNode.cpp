#include "StyleNode.h"
#include "NodeVisitor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>

namespace CHTL {

StyleNode::StyleNode(bool is_global)
    : BaseNode(NodeType::STYLE, "style"), is_global_(is_global) {
}

void StyleNode::addRule(const StyleRule& rule) {
    rules_.push_back(rule);
    
    // 如果是类选择器，自动添加到auto_classes_
    if (!rule.is_inline && rule.selector.size() > 0 && rule.selector[0] == '.') {
        std::string class_name = rule.selector.substr(1);
        // 移除伪类、伪元素等
        size_t colon_pos = class_name.find(':');
        if (colon_pos != std::string::npos) {
            class_name = class_name.substr(0, colon_pos);
        }
        auto_classes_.insert(class_name);
    }
    
    // 如果是ID选择器，自动添加到auto_ids_
    if (!rule.is_inline && rule.selector.size() > 0 && rule.selector[0] == '#') {
        std::string id_name = rule.selector.substr(1);
        // 移除伪类、伪元素等
        size_t colon_pos = id_name.find(':');
        if (colon_pos != std::string::npos) {
            id_name = id_name.substr(0, colon_pos);
        }
        auto_ids_.insert(id_name);
    }
}

void StyleNode::addInlineProperty(const std::string& property, const std::string& value) {
    // 查找或创建内联样式规则
    StyleRule* inline_rule = nullptr;
    for (auto& rule : rules_) {
        if (rule.is_inline) {
            inline_rule = &rule;
            break;
        }
    }
    
    if (!inline_rule) {
        rules_.emplace_back("", true);
        inline_rule = &rules_.back();
    }
    
    inline_rule->properties[property] = value;
}

void StyleNode::addSelectorRule(const std::string& selector, 
                               const std::unordered_map<std::string, std::string>& properties) {
    StyleRule rule(normalizeSelector(selector), false);
    rule.properties = properties;
    addRule(rule);
}

void StyleNode::addAutoClass(const std::string& class_name) {
    auto_classes_.insert(class_name);
}

void StyleNode::addAutoId(const std::string& id_name) {
    auto_ids_.insert(id_name);
}

std::string StyleNode::resolveAmpersandSelector(const std::string& selector) const {
    if (parent_selector_.empty()) {
        // 如果没有父选择器，使用第一个自动类或ID
        std::string parent;
        if (!auto_classes_.empty()) {
            parent = "." + *auto_classes_.begin();
        } else if (!auto_ids_.empty()) {
            parent = "#" + *auto_ids_.begin();
        } else {
            return selector; // 无法解析&
        }
        
        // 替换&为父选择器
        std::string result = selector;
        size_t pos = 0;
        while ((pos = result.find('&', pos)) != std::string::npos) {
            result.replace(pos, 1, parent);
            pos += parent.length();
        }
        return result;
    }
    
    // 使用设定的父选择器
    std::string result = selector;
    size_t pos = 0;
    while ((pos = result.find('&', pos)) != std::string::npos) {
        result.replace(pos, 1, parent_selector_);
        pos += parent_selector_.length();
    }
    return result;
}

const StyleRule* StyleNode::getInlineRule() const {
    for (const auto& rule : rules_) {
        if (rule.is_inline) {
            return &rule;
        }
    }
    return nullptr;
}

std::vector<StyleRule> StyleNode::getNonInlineRules() const {
    std::vector<StyleRule> non_inline_rules;
    for (const auto& rule : rules_) {
        if (!rule.is_inline) {
            non_inline_rules.push_back(rule);
        }
    }
    return non_inline_rules;
}

std::string StyleNode::generateInlineStyle() const {
    const StyleRule* inline_rule = getInlineRule();
    if (!inline_rule || inline_rule->properties.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    bool first = true;
    for (const auto& prop : inline_rule->properties) {
        if (!first) oss << " ";
        oss << prop.first << ": " << escapeCSSValue(prop.second) << ";";
        first = false;
    }
    
    return oss.str();
}

std::string StyleNode::generateCSS(int indent) const {
    auto non_inline_rules = getNonInlineRules();
    if (non_inline_rules.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& rule : non_inline_rules) {
        if (!first) {
            oss << std::endl << std::endl;
        }
        
        // 处理&引用
        std::string processed_selector = rule.selector;
        if (processed_selector.find('&') != std::string::npos) {
            processed_selector = resolveAmpersandSelector(processed_selector);
        }
        
        oss << getIndent(indent) << processed_selector << " {" << std::endl;
        
        for (const auto& prop : rule.properties) {
            oss << getIndent(indent + 1) << prop.first << ": " 
                << escapeCSSValue(prop.second) << ";" << std::endl;
        }
        
        oss << getIndent(indent) << "}";
        first = false;
    }
    
    return oss.str();
}

void StyleNode::merge(const StyleNode& other) {
    // 合并规则
    for (const auto& rule : other.rules_) {
        rules_.push_back(rule);
    }
    
    // 合并自动类名和ID
    auto_classes_.insert(other.auto_classes_.begin(), other.auto_classes_.end());
    auto_ids_.insert(other.auto_ids_.begin(), other.auto_ids_.end());
}

NodePtr StyleNode::clone(bool deep) const {
    auto cloned = std::make_shared<StyleNode>(is_global_);
    cloned->attributes_ = attributes_;
    cloned->start_pos_ = start_pos_;
    cloned->end_pos_ = end_pos_;
    cloned->raw_content_ = raw_content_;
    cloned->rules_ = rules_;
    cloned->auto_classes_ = auto_classes_;
    cloned->auto_ids_ = auto_ids_;
    cloned->parent_selector_ = parent_selector_;
    
    if (deep) {
        for (const auto& child : children_) {
            cloned->addChild(child->clone(true));
        }
    }
    
    return cloned;
}

void StyleNode::accept(NodeVisitor& visitor) {
    visitor.visitStyle(*this);
}

std::string StyleNode::toString(int indent) const {
    std::ostringstream oss;
    oss << getIndent(indent) << "StyleNode{global=" << (is_global_ ? "true" : "false")
        << ", rules=" << rules_.size();
    
    if (!auto_classes_.empty()) {
        oss << ", auto_classes=[";
        bool first = true;
        for (const auto& cls : auto_classes_) {
            if (!first) oss << ", ";
            oss << cls;
            first = false;
        }
        oss << "]";
    }
    
    if (!auto_ids_.empty()) {
        oss << ", auto_ids=[";
        bool first = true;
        for (const auto& id : auto_ids_) {
            if (!first) oss << ", ";
            oss << id;
            first = false;
        }
        oss << "]";
    }
    
    oss << "}";
    
    if (!rules_.empty()) {
        oss << " {" << std::endl;
        for (const auto& rule : rules_) {
            oss << getIndent(indent + 1) << "Rule{selector=\"" << rule.selector
                << "\", inline=" << (rule.is_inline ? "true" : "false")
                << ", properties=" << rule.properties.size() << "}" << std::endl;
        }
        oss << getIndent(indent) << "}";
    }
    
    return oss.str();
}

std::string StyleNode::validate() const {
    // 验证样式规则
    for (const auto& rule : rules_) {
        // 验证选择器
        if (!rule.is_inline && rule.selector.empty()) {
            return "Non-inline style rule cannot have empty selector";
        }
        
        // 验证属性
        for (const auto& prop : rule.properties) {
            if (prop.first.empty()) {
                return "CSS property name cannot be empty";
            }
            
            if (!validateCSSProperty(prop.first, prop.second)) {
                return "Invalid CSS property: " + prop.first + " = " + prop.second;
            }
        }
    }
    
    return BaseNode::validate();
}

std::string StyleNode::normalizeSelector(const std::string& selector) const {
    std::string normalized = selector;
    
    // 移除多余的空白字符
    std::regex ws_regex("\\s+");
    normalized = std::regex_replace(normalized, ws_regex, " ");
    
    // 移除前后空白
    size_t start = normalized.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = normalized.find_last_not_of(" \t\n\r");
    normalized = normalized.substr(start, end - start + 1);
    
    return normalized;
}

bool StyleNode::validateCSSProperty(const std::string& property, const std::string& value) const {
    // 简单的CSS属性验证
    if (property.empty() || value.empty()) {
        return false;
    }
    
    // 检查属性名是否包含非法字符
    for (char c : property) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    // 这里可以添加更复杂的CSS属性验证逻辑
    return true;
}

std::string StyleNode::escapeCSSValue(const std::string& value) const {
    // 简单的CSS值转义
    std::string escaped = value;
    
    // 如果值包含特殊字符，需要引号包围
    bool needs_quotes = false;
    for (char c : value) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '"' || c == '\'') {
            needs_quotes = true;
            break;
        }
    }
    
    if (needs_quotes && !value.empty() && value[0] != '"' && value[0] != '\'') {
        escaped = "\"" + escaped + "\"";
    }
    
    return escaped;
}

} // namespace CHTL