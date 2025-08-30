#include "BaseNode.h"
#include <sstream>

namespace CHTLJS {

void BaseNode::addChild(std::shared_ptr<BaseNode> child) {
    if (child) {
        children.push_back(child);
    }
}

std::shared_ptr<BaseNode> BaseNode::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

size_t BaseNode::getChildCount() const {
    return children.size();
}

std::string BaseNode::getNodeTypeName() const {
    switch (nodeType) {
        case CHTLJSNodeType::ROOT_NODE: return "ROOT_NODE";
        case CHTLJSNodeType::SCRIPT_NODE: return "SCRIPT_NODE";
        case CHTLJSNodeType::MODULE_NODE: return "MODULE_NODE";
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE: return "ENHANCED_SELECTOR_NODE";
        case CHTLJSNodeType::EVENT_LISTENER_NODE: return "EVENT_LISTENER_NODE";
        case CHTLJSNodeType::EVENT_DELEGATE_NODE: return "EVENT_DELEGATE_NODE";
        case CHTLJSNodeType::ANIMATION_NODE: return "ANIMATION_NODE";
        case CHTLJSNodeType::VIRTUAL_OBJECT_NODE: return "VIRTUAL_OBJECT_NODE";
        case CHTLJSNodeType::EVENT_BINDING_NODE: return "EVENT_BINDING_NODE";
        case CHTLJSNodeType::CHTLJS_FUNCTION_NODE: return "CHTLJS_FUNCTION_NODE";
        case CHTLJSNodeType::FUNCTION_PARAM_NODE: return "FUNCTION_PARAM_NODE";
        case CHTLJSNodeType::JS_EXPRESSION_NODE: return "JS_EXPRESSION_NODE";
        case CHTLJSNodeType::JS_STATEMENT_NODE: return "JS_STATEMENT_NODE";
        default: return "UNKNOWN_NODE";
    }
}

bool BaseNode::isCHTLJSSpecificNode() const {
    return nodeType == CHTLJSNodeType::ENHANCED_SELECTOR_NODE ||
           nodeType == CHTLJSNodeType::EVENT_LISTENER_NODE ||
           nodeType == CHTLJSNodeType::EVENT_DELEGATE_NODE ||
           nodeType == CHTLJSNodeType::ANIMATION_NODE ||
           nodeType == CHTLJSNodeType::VIRTUAL_OBJECT_NODE ||
           nodeType == CHTLJSNodeType::EVENT_BINDING_NODE ||
           nodeType == CHTLJSNodeType::CHTLJS_FUNCTION_NODE;
}

bool BaseNode::isEventNode() const {
    return nodeType == CHTLJSNodeType::EVENT_LISTENER_NODE ||
           nodeType == CHTLJSNodeType::EVENT_DELEGATE_NODE ||
           nodeType == CHTLJSNodeType::EVENT_BINDING_NODE;
}

std::string BaseNode::generateJS() const {
    // 基础实现，子类可以重写
    std::ostringstream js;
    
    switch (nodeType) {
        case CHTLJSNodeType::ENHANCED_SELECTOR_NODE:
            // 转换增强选择器
            if (content.find('.') == 0) {
                js << "document.querySelector('" << content << "')";
            } else if (content.find('#') == 0) {
                std::string id = content.substr(1);
                js << "document.getElementById('" << id << "')";
            } else {
                js << "document.querySelectorAll('" << content << "')";
            }
            break;
            
        case CHTLJSNodeType::EVENT_LISTENER_NODE:
            // 生成事件监听器代码
            js << "element.addEventListener('" << content << "', function() {});";
            break;
            
        case CHTLJSNodeType::EVENT_BINDING_NODE:
            // 生成事件绑定代码
            js << "element.addEventListener('" << content << "', function() {});";
            break;
            
        case CHTLJSNodeType::VIRTUAL_OBJECT_NODE:
            // vir是编译期语法糖，不生成代码
            break;
            
        default:
            js << content;
            break;
    }
    
    return js.str();
}

bool BaseNode::validate() const {
    // 基础验证
    if (nodeType == CHTLJSNodeType::UNKNOWN_NODE) {
        return false;
    }
    
    // 验证子节点
    for (const auto& child : children) {
        if (child && !child->validate()) {
            return false;
        }
    }
    
    return true;
}

std::string BaseNode::toString() const {
    std::ostringstream oss;
    oss << getNodeTypeName() << "(\"" << content << "\") at " << position.toString();
    if (!children.empty()) {
        oss << " [" << children.size() << " children]";
    }
    return oss.str();
}

std::shared_ptr<BaseNode> BaseNode::clone() const {
    auto cloned = std::make_shared<BaseNode>(nodeType, content, position);
    
    for (const auto& child : children) {
        if (child) {
            cloned->addChild(child->clone());
        }
    }
    
    return cloned;
}

void BaseNode::reportError(const std::string& message) const {
    ErrorReporter reporter;
    reporter.error(ErrorType::SYNTAX_ERROR, message, "", position.line, position.column);
}

NodePtr NodeFactory::createNode(CHTLJSNodeType type, const std::string& content) {
    return std::make_shared<BaseNode>(type, content);
}

NodePtr NodeFactory::createNodeFromToken(const Token& token) {
    CHTLJSNodeType nodeType = CHTLJSNodeType::UNKNOWN_NODE;
    
    switch (token.type) {
        case TokenType::SCRIPT:
            nodeType = CHTLJSNodeType::SCRIPT_NODE;
            break;
        case TokenType::MODULE:
            nodeType = CHTLJSNodeType::MODULE_NODE;
            break;
        case TokenType::LISTEN:
            nodeType = CHTLJSNodeType::EVENT_LISTENER_NODE;
            break;
        case TokenType::DELEGATE:
            nodeType = CHTLJSNodeType::EVENT_DELEGATE_NODE;
            break;
        case TokenType::ANIMATE:
            nodeType = CHTLJSNodeType::ANIMATION_NODE;
            break;
        case TokenType::VIR:
            nodeType = CHTLJSNodeType::VIRTUAL_OBJECT_NODE;
            break;
        case TokenType::EVENT_BINDING:
            nodeType = CHTLJSNodeType::EVENT_BINDING_NODE;
            break;
        default:
            nodeType = CHTLJSNodeType::UNKNOWN_NODE;
            break;
    }
    
    return std::make_shared<BaseNode>(nodeType, token.value, token.position);
}

} // namespace CHTLJS