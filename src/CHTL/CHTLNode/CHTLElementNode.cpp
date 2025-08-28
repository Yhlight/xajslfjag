#include "CHTLElementNode.h"
#include "CHTLTextNode.h"
#include "CHTLStyleNode.h"
#include "CHTLScriptNode.h"
#include <sstream>
#include <algorithm>
#include <set>

namespace CHTL {

CHTLElementNode::CHTLElementNode(const std::string& tagName) 
    : CHTLBaseNode(CHTLNodeType::ELEMENT), tagName(tagName) {
}

bool CHTLElementNode::isSelfClosing() const {
    // 检查是否为HTML自闭合标签
    static const std::set<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "keygen", "link", "meta", "param", "source", "track", "wbr"
    };
    
    return selfClosing || selfClosingTags.find(tagName) != selfClosingTags.end();
}

void CHTLElementNode::setId(const std::string& id) {
    setAttribute("id", id);
}

std::string CHTLElementNode::getId() const {
    return getAttribute("id");
}

void CHTLElementNode::addClass(const std::string& className) {
    if (className.empty()) return;
    
    auto classes = parseClassAttribute();
    
    // 检查类名是否已存在
    if (std::find(classes.begin(), classes.end(), className) == classes.end()) {
        classes.push_back(className);
        setClasses(classes);
    }
}

void CHTLElementNode::removeClass(const std::string& className) {
    auto classes = parseClassAttribute();
    auto it = std::find(classes.begin(), classes.end(), className);
    
    if (it != classes.end()) {
        classes.erase(it);
        setClasses(classes);
    }
}

bool CHTLElementNode::hasClass(const std::string& className) const {
    auto classes = parseClassAttribute();
    return std::find(classes.begin(), classes.end(), className) != classes.end();
}

std::vector<std::string> CHTLElementNode::getClasses() const {
    return parseClassAttribute();
}

void CHTLElementNode::setClasses(const std::vector<std::string>& classes) {
    if (classes.empty()) {
        removeAttribute("class");
    } else {
        std::ostringstream oss;
        for (size_t i = 0; i < classes.size(); ++i) {
            if (i > 0) oss << " ";
            oss << classes[i];
        }
        setAttribute("class", oss.str());
    }
}

std::vector<std::string> CHTLElementNode::parseClassAttribute() const {
    std::vector<std::string> classes;
    std::string classStr = getAttribute("class");
    
    if (!classStr.empty()) {
        std::istringstream iss(classStr);
        std::string className;
        
        while (iss >> className) {
            classes.push_back(className);
        }
    }
    
    return classes;
}

void CHTLElementNode::setInlineStyle(const std::string& property, const std::string& value) {
    inlineStyles[property] = value;
    updateStyleAttribute();
}

void CHTLElementNode::removeInlineStyle(const std::string& property) {
    inlineStyles.erase(property);
    updateStyleAttribute();
}

std::string CHTLElementNode::getInlineStyle(const std::string& property) const {
    auto it = inlineStyles.find(property);
    return it != inlineStyles.end() ? it->second : "";
}

std::string CHTLElementNode::getInlineStyleString() const {
    std::ostringstream oss;
    for (const auto& style : inlineStyles) {
        if (oss.tellp() > 0) oss << "; ";
        oss << style.first << ": " << style.second;
    }
    return oss.str();
}

void CHTLElementNode::updateStyleAttribute() {
    std::string styleStr = getInlineStyleString();
    if (styleStr.empty()) {
        removeAttribute("style");
    } else {
        setAttribute("style", styleStr);
    }
}

std::shared_ptr<CHTLStyleNode> CHTLElementNode::getStyleNode() const {
    for (const auto& child : children) {
        if (child->getNodeType() == CHTLNodeType::STYLE) {
            return std::dynamic_pointer_cast<CHTLStyleNode>(child);
        }
    }
    return nullptr;
}

std::shared_ptr<CHTLScriptNode> CHTLElementNode::getScriptNode() const {
    for (const auto& child : children) {
        if (child->getNodeType() == CHTLNodeType::SCRIPT) {
            return std::dynamic_pointer_cast<CHTLScriptNode>(child);
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<CHTLTextNode>> CHTLElementNode::getTextNodes() const {
    std::vector<std::shared_ptr<CHTLTextNode>> textNodes;
    for (const auto& child : children) {
        if (child->getNodeType() == CHTLNodeType::TEXT) {
            textNodes.push_back(std::dynamic_pointer_cast<CHTLTextNode>(child));
        }
    }
    return textNodes;
}

void CHTLElementNode::addStyleNode(std::shared_ptr<CHTLStyleNode> styleNode) {
    if (styleNode) {
        addChild(styleNode);
    }
}

void CHTLElementNode::addScriptNode(std::shared_ptr<CHTLScriptNode> scriptNode) {
    if (scriptNode) {
        addChild(scriptNode);
    }
}

void CHTLElementNode::addTextNode(std::shared_ptr<CHTLTextNode> textNode) {
    if (textNode) {
        addChild(textNode);
    }
}

bool CHTLElementNode::isValidHtmlElement() const {
    // 基本的HTML标签名验证
    if (tagName.empty()) return false;
    
    // 检查标签名是否包含有效字符
    for (char c : tagName) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    return true;
}

void CHTLElementNode::accept(CHTLNodeVisitor* visitor) {
    if (visitor) {
        visitor->visit(this);
    }
}

std::shared_ptr<CHTLBaseNode> CHTLElementNode::clone() const {
    auto cloned = std::make_shared<CHTLElementNode>(tagName);
    
    // 复制属性
    cloned->attributes = attributes;
    cloned->selfClosing = selfClosing;
    cloned->inlineStyles = inlineStyles;
    cloned->sourceLine = sourceLine;
    cloned->sourceColumn = sourceColumn;
    cloned->token = token;
    
    // 深度复制子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string CHTLElementNode::toString() const {
    std::ostringstream oss;
    oss << "ElementNode[" << tagName << "]";
    
    if (!attributes.empty()) {
        oss << " {";
        bool first = true;
        for (const auto& attr : attributes) {
            if (!first) oss << ", ";
            oss << attr.first << "=\"" << attr.second << "\"";
            first = false;
        }
        oss << "}";
    }
    
    if (sourceLine > 0) {
        oss << " @" << sourceLine << ":" << sourceColumn;
    }
    
    return oss.str();
}

std::string CHTLElementNode::toHTML(bool includeChildren, int indent) const {
    std::string indentStr = std::string(indent * 2, ' ');
    std::ostringstream oss;
    
    oss << indentStr << "<" << tagName;
    
    // 添加属性
    for (const auto& attr : attributes) {
        oss << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    if (isSelfClosing()) {
        oss << " />";
    } else {
        oss << ">";
        
        if (includeChildren && !children.empty()) {
            bool hasElementChildren = false;
            for (const auto& child : children) {
                if (child->getNodeType() == CHTLNodeType::ELEMENT) {
                    hasElementChildren = true;
                    break;
                }
            }
            
            if (hasElementChildren) {
                oss << "\n";
            }
            
            for (const auto& child : children) {
                if (auto elementChild = std::dynamic_pointer_cast<CHTLElementNode>(child)) {
                    oss << elementChild->toHTML(true, indent + 1);
                    if (hasElementChildren) {
                        oss << "\n";
                    }
                } else if (auto textChild = std::dynamic_pointer_cast<CHTLTextNode>(child)) {
                    oss << textChild->getText();
                }
            }
            
            if (hasElementChildren) {
                oss << indentStr;
            }
        }
        
        oss << "</" << tagName << ">";
    }
    
    return oss.str();
}

} // namespace CHTL