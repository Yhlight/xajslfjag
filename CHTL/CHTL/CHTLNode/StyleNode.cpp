#include "StyleNode.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

void StyleNode::addRule(const CSSRule& rule) {
    rules.push_back(rule);
}

void StyleNode::addInlineProperty(const std::string& property, const std::string& value) {
    // 查找或创建内联样式规则
    for (auto& rule : rules) {
        if (rule.type == CSSRuleType::INLINE) {
            rule.properties[property] = value;
            return;
        }
    }
    
    // 如果没有内联样式规则，创建一个
    CSSRule inlineRule(CSSRuleType::INLINE, "");
    inlineRule.properties[property] = value;
    rules.push_back(inlineRule);
}

void StyleNode::addClassRule(const std::string& className) {
    CSSRule classRule(CSSRuleType::CLASS, className);
    rules.push_back(classRule);
}

void StyleNode::addIdRule(const std::string& idName) {
    CSSRule idRule(CSSRuleType::ID, idName);
    rules.push_back(idRule);
}

void StyleNode::addPseudoClassRule(const std::string& selector) {
    CSSRule pseudoRule(CSSRuleType::PSEUDO_CLASS, selector);
    rules.push_back(pseudoRule);
}

void StyleNode::addPseudoElementRule(const std::string& selector) {
    CSSRule pseudoRule(CSSRuleType::PSEUDO_ELEMENT, selector);
    rules.push_back(pseudoRule);
}

void StyleNode::addContextRule(const std::string& selector) {
    CSSRule contextRule(CSSRuleType::CONTEXT, selector);
    rules.push_back(contextRule);
}

std::vector<CSSRule> StyleNode::getRulesByType(CSSRuleType type) const {
    std::vector<CSSRule> filtered;
    for (const auto& rule : rules) {
        if (rule.type == type) {
            filtered.push_back(rule);
        }
    }
    return filtered;
}

void StyleNode::importStyle(const std::string& styleName) {
    importedStyles.push_back(styleName);
}

std::string StyleNode::getAutoClassName() const {
    // 返回第一个类选择器的名称（如果存在）
    for (const auto& rule : rules) {
        if (rule.type == CSSRuleType::CLASS && !rule.selector.empty()) {
            // 移除点号前缀
            if (rule.selector[0] == '.') {
                return rule.selector.substr(1);
            }
            return rule.selector;
        }
    }
    return "";
}

std::string StyleNode::getAutoIdName() const {
    // 返回第一个ID选择器的名称（如果存在）
    for (const auto& rule : rules) {
        if (rule.type == CSSRuleType::ID && !rule.selector.empty()) {
            // 移除井号前缀
            if (rule.selector[0] == '#') {
                return rule.selector.substr(1);
            }
            return rule.selector;
        }
    }
    return "";
}

void StyleNode::accept(NodeVisitor* visitor) {
    if (auto* styleVisitor = dynamic_cast<StyleNodeVisitor*>(visitor)) {
        styleVisitor->visitStyleNode(this);
    }
}

std::shared_ptr<BaseNode> StyleNode::clone() const {
    auto cloned = std::make_shared<StyleNode>(isGlobal, line, column);
    
    // 复制规则
    cloned->rules = rules;
    cloned->importedStyles = importedStyles;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    
    if (isGlobal) {
        ss << "<style>\n";
    } else {
        ss << "style {\n";
    }
    
    // 输出导入的样式组
    for (const auto& imported : importedStyles) {
        ss << "    @Style " << imported << ";\n";
    }
    
    // 输出CSS规则
    for (const auto& rule : rules) {
        if (rule.type == CSSRuleType::INLINE) {
            // 内联样式直接输出属性
            for (const auto& [prop, value] : rule.properties) {
                ss << "    " << prop << ": " << value << ";\n";
            }
        } else {
            // 其他类型的规则输出选择器和属性
            ss << "    " << rule.selector << " {\n";
            for (const auto& [prop, value] : rule.properties) {
                ss << "        " << prop << ": " << value << ";\n";
            }
            ss << "    }\n";
        }
    }
    
    // 输出子节点
    for (const auto& child : children) {
        ss << "    " << child->toString() << "\n";
    }
    
    if (isGlobal) {
        ss << "</style>";
    } else {
        ss << "}";
    }
    
    return ss.str();
}

} // namespace CHTL