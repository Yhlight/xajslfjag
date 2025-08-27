#include "StyleNode.h"
#include <sstream>
#include <regex>

namespace CHTL {

StyleNode::StyleNode(bool isLocal)
    : BaseNode(NodeType::STYLE), isLocal(isLocal) {
}

std::string StyleNode::Generate() const {
    auto [inlineStyle, globalStyle] = SeparateStyles();
    
    if (isLocal) {
        // 局部样式块只返回内联样式部分
        // 全局样式部分会被提取到全局样式块中
        return inlineStyle;
    } else {
        // 全局样式块
        std::stringstream ss;
        ss << "<style>\n";
        ss << globalStyle;
        ss << "</style>\n";
        return ss.str();
    }
}

NodePtr StyleNode::Clone() const {
    auto clone = std::make_shared<StyleNode>(isLocal);
    clone->SetPosition(line, column);
    clone->rules = rules;
    return clone;
}

void StyleNode::Accept(NodeVisitor* visitor) {
    visitor->VisitStyleNode(this);
}

void StyleNode::AddRule(const StyleRule& rule) {
    rules.push_back(rule);
}

void StyleNode::AddInlineProperty(const std::string& name, const std::string& value) {
    StyleRule inlineRule;
    inlineRule.isInlineStyle = true;
    inlineRule.properties[name] = value;
    rules.push_back(inlineRule);
}

std::unordered_map<std::string, std::string> StyleNode::GetInlineProperties() const {
    std::unordered_map<std::string, std::string> result;
    
    for (const auto& rule : rules) {
        if (rule.isInlineStyle) {
            for (const auto& [name, value] : rule.properties) {
                result[name] = value;
            }
        }
    }
    
    return result;
}

std::string StyleNode::GetFirstClassSelector() const {
    for (const auto& rule : rules) {
        if (!rule.isInlineStyle && IsClassSelector(rule.selector)) {
            return ExtractClassName(rule.selector);
        }
    }
    return "";
}

std::string StyleNode::GetFirstIdSelector() const {
    for (const auto& rule : rules) {
        if (!rule.isInlineStyle && IsIdSelector(rule.selector)) {
            return ExtractIdName(rule.selector);
        }
    }
    return "";
}

bool StyleNode::HasClassSelector(const std::string& className) const {
    for (const auto& rule : rules) {
        if (!rule.isInlineStyle && IsClassSelector(rule.selector)) {
            if (ExtractClassName(rule.selector) == className) {
                return true;
            }
        }
    }
    return false;
}

bool StyleNode::HasIdSelector(const std::string& id) const {
    for (const auto& rule : rules) {
        if (!rule.isInlineStyle && IsIdSelector(rule.selector)) {
            if (ExtractIdName(rule.selector) == id) {
                return true;
            }
        }
    }
    return false;
}

void StyleNode::ProcessAmpersandSelector(const std::string& parentSelector) {
    for (auto& rule : rules) {
        if (IsAmpersandSelector(rule.selector)) {
            // 替换 & 为父选择器
            size_t pos = rule.selector.find('&');
            if (pos != std::string::npos) {
                rule.selector = parentSelector + rule.selector.substr(pos + 1);
            }
        }
    }
}

std::pair<std::string, std::string> StyleNode::SeparateStyles() const {
    std::stringstream inlineStream;
    std::stringstream globalStream;
    
    for (const auto& rule : rules) {
        if (rule.isInlineStyle) {
            // 内联样式
            for (const auto& [name, value] : rule.properties) {
                if (!inlineStream.str().empty()) {
                    inlineStream << " ";
                }
                inlineStream << name << ": " << value << ";";
            }
        } else {
            // 全局样式
            globalStream << rule.selector << " {\n";
            for (const auto& [name, value] : rule.properties) {
                globalStream << "    " << name << ": " << value << ";\n";
            }
            globalStream << "}\n\n";
        }
    }
    
    return {inlineStream.str(), globalStream.str()};
}

bool StyleNode::IsClassSelector(const std::string& selector) const {
    return !selector.empty() && selector[0] == '.';
}

bool StyleNode::IsIdSelector(const std::string& selector) const {
    return !selector.empty() && selector[0] == '#';
}

bool StyleNode::IsAmpersandSelector(const std::string& selector) const {
    return selector.find('&') != std::string::npos;
}

std::string StyleNode::ExtractClassName(const std::string& selector) const {
    if (!IsClassSelector(selector)) return "";
    
    // 提取类名（从.开始到第一个非标识符字符）
    std::regex classRegex(R"(\.([a-zA-Z_][\w-]*))");
    std::smatch match;
    
    if (std::regex_search(selector, match, classRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string StyleNode::ExtractIdName(const std::string& selector) const {
    if (!IsIdSelector(selector)) return "";
    
    // 提取ID名（从#开始到第一个非标识符字符）
    std::regex idRegex(R"(#([a-zA-Z_][\w-]*))");
    std::smatch match;
    
    if (std::regex_search(selector, match, idRegex)) {
        return match[1].str();
    }
    
    return "";
}

} // namespace CHTL