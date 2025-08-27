#include "CHTLJSNode.h"
#include <sstream>
#include <algorithm>

namespace CHTLJS {

// CHTLJSNode基类实现
NodePtr CHTLJSNode::GetChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

// EnhancedSelectorNode实现
EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector)
    : CHTLJSNode(NodeType::ENHANCED_SELECTOR), selector(selector), index(-1), hasIndex(false) {
}

std::string EnhancedSelectorNode::GenerateJS() const {
    return GenerateSelectorJS();
}

NodePtr EnhancedSelectorNode::Clone() const {
    auto clone = std::make_shared<EnhancedSelectorNode>(selector);
    if (hasIndex) {
        clone->SetIndex(index);
    }
    clone->SetPosition(line, column);
    return clone;
}

bool EnhancedSelectorNode::IsClassSelector() const {
    return !selector.empty() && selector[0] == '.';
}

bool EnhancedSelectorNode::IsIdSelector() const {
    return !selector.empty() && selector[0] == '#';
}

bool EnhancedSelectorNode::IsTagSelector() const {
    return !selector.empty() && std::isalpha(selector[0]);
}

bool EnhancedSelectorNode::IsComplexSelector() const {
    return selector.find(' ') != std::string::npos;
}

std::string EnhancedSelectorNode::GenerateSelectorJS() const {
    std::stringstream ss;
    
    if (IsComplexSelector()) {
        // 复杂选择器，如 ".box button"
        ss << "document.querySelectorAll('" << selector << "')";
    } else if (IsClassSelector()) {
        // 类选择器
        if (hasIndex) {
            ss << "document.getElementsByClassName('" << selector.substr(1) << "')[" << index << "]";
        } else {
            ss << "document.querySelectorAll('" << selector << "')";
        }
    } else if (IsIdSelector()) {
        // ID选择器
        ss << "document.getElementById('" << selector.substr(1) << "')";
    } else if (IsTagSelector()) {
        // 标签选择器
        if (hasIndex) {
            ss << "document.getElementsByTagName('" << selector << "')[" << index << "]";
        } else {
            // 先检查是否是tag，然后查找类名/id
            ss << "(function() {\n";
            ss << "  var els = document.getElementsByTagName('" << selector << "');\n";
            ss << "  if (els.length > 0) return els;\n";
            ss << "  els = document.getElementsByClassName('" << selector << "');\n";
            ss << "  if (els.length > 0) return els;\n";
            ss << "  var el = document.getElementById('" << selector << "');\n";
            ss << "  return el ? [el] : [];\n";
            ss << "})()";
        }
    } else {
        // 其他情况
        ss << "document.querySelectorAll('" << selector << "')";
    }
    
    return ss.str();
}

// ArrowAccessNode实现
ArrowAccessNode::ArrowAccessNode(NodePtr object, const std::string& property)
    : CHTLJSNode(NodeType::ARROW_ACCESS), object(object), property(property) {
}

std::string ArrowAccessNode::GenerateJS() const {
    return object->GenerateJS() + "." + property;
}

NodePtr ArrowAccessNode::Clone() const {
    auto clone = std::make_shared<ArrowAccessNode>(object->Clone(), property);
    clone->SetPosition(line, column);
    return clone;
}

// VirDeclarationNode实现
VirDeclarationNode::VirDeclarationNode(const std::string& name, NodePtr value)
    : CHTLJSNode(NodeType::VIR_DECLARATION), name(name), value(value) {
}

std::string VirDeclarationNode::GenerateJS() const {
    // vir声明不生成JS代码，它是编译时的概念
    return "";
}

NodePtr VirDeclarationNode::Clone() const {
    auto clone = std::make_shared<VirDeclarationNode>(name, value->Clone());
    clone->SetPosition(line, column);
    return clone;
}

// CHTLJSFunctionNode实现
CHTLJSFunctionNode::CHTLJSFunctionNode(const std::string& name)
    : CHTLJSNode(NodeType::CHTLJS_FUNCTION), functionName(name) {
}

void CHTLJSFunctionNode::AddProperty(const std::string& key, NodePtr value) {
    properties[key] = value;
}

NodePtr CHTLJSFunctionNode::GetProperty(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return nullptr;
}

bool CHTLJSFunctionNode::HasProperty(const std::string& key) const {
    return properties.find(key) != properties.end();
}

std::string CHTLJSFunctionNode::GenerateJS() const {
    if (functionName == "listen") {
        return GenerateListenJS();
    } else if (functionName == "delegate") {
        return GenerateDelegateJS();
    } else if (functionName == "animate") {
        return GenerateAnimateJS();
    }
    
    // 默认生成对象字面量
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto& [key, value] : properties) {
        if (!first) ss << ", ";
        ss << key << ": " << value->GenerateJS();
        first = false;
    }
    ss << "}";
    return ss.str();
}

NodePtr CHTLJSFunctionNode::Clone() const {
    auto clone = std::make_shared<CHTLJSFunctionNode>(functionName);
    for (const auto& [key, value] : properties) {
        clone->AddProperty(key, value->Clone());
    }
    clone->SetPosition(line, column);
    return clone;
}

std::string CHTLJSFunctionNode::GenerateListenJS() const {
    std::stringstream ss;
    
    // 生成addEventListener调用
    for (const auto& [event, handler] : properties) {
        ss << ".addEventListener('" << event << "', " << handler->GenerateJS() << ")";
    }
    
    return ss.str();
}

std::string CHTLJSFunctionNode::GenerateDelegateJS() const {
    // TODO: 实现事件委托的JS生成
    return "";
}

std::string CHTLJSFunctionNode::GenerateAnimateJS() const {
    // TODO: 实现动画的JS生成
    return "";
}

// EventBindingNode实现
EventBindingNode::EventBindingNode(NodePtr target, const std::string& event, NodePtr handler)
    : CHTLJSNode(NodeType::EVENT_BINDING), target(target), event(event), handler(handler) {
}

std::string EventBindingNode::GenerateJS() const {
    std::stringstream ss;
    ss << target->GenerateJS() << ".addEventListener('" << event << "', " 
       << handler->GenerateJS() << ")";
    return ss.str();
}

NodePtr EventBindingNode::Clone() const {
    auto clone = std::make_shared<EventBindingNode>(
        target->Clone(), event, handler->Clone()
    );
    clone->SetPosition(line, column);
    return clone;
}

// LiteralNode实现
LiteralNode::LiteralNode(const std::string& value, LiteralType type)
    : CHTLJSNode(NodeType::LITERAL), value(value), literalType(type) {
}

std::string LiteralNode::GenerateJS() const {
    switch (literalType) {
        case LiteralType::STRING:
            return "'" + value + "'";
        case LiteralType::NUMBER:
        case LiteralType::BOOLEAN:
            return value;
        case LiteralType::NULL_TYPE:
            return "null";
        case LiteralType::UNDEFINED:
            return "undefined";
        default:
            return value;
    }
}

NodePtr LiteralNode::Clone() const {
    auto clone = std::make_shared<LiteralNode>(value, literalType);
    clone->SetPosition(line, column);
    return clone;
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& name)
    : CHTLJSNode(NodeType::IDENTIFIER), name(name) {
}

std::string IdentifierNode::GenerateJS() const {
    return name;
}

NodePtr IdentifierNode::Clone() const {
    auto clone = std::make_shared<IdentifierNode>(name);
    clone->SetPosition(line, column);
    return clone;
}

// BinaryOpNode实现
BinaryOpNode::BinaryOpNode(const std::string& op, NodePtr left, NodePtr right)
    : CHTLJSNode(NodeType::BINARY_OP), op(op), left(left), right(right) {
}

std::string BinaryOpNode::GenerateJS() const {
    // 处理 ** 操作符
    if (op == "**") {
        return "Math.pow(" + left->GenerateJS() + ", " + right->GenerateJS() + ")";
    }
    
    return left->GenerateJS() + " " + op + " " + right->GenerateJS();
}

NodePtr BinaryOpNode::Clone() const {
    auto clone = std::make_shared<BinaryOpNode>(op, left->Clone(), right->Clone());
    clone->SetPosition(line, column);
    return clone;
}

// FunctionCallNode实现
FunctionCallNode::FunctionCallNode(NodePtr function)
    : CHTLJSNode(NodeType::FUNCTION_CALL), function(function) {
}

std::string FunctionCallNode::GenerateJS() const {
    std::stringstream ss;
    ss << function->GenerateJS() << "(";
    
    bool first = true;
    for (const auto& arg : arguments) {
        if (!first) ss << ", ";
        ss << arg->GenerateJS();
        first = false;
    }
    
    ss << ")";
    return ss.str();
}

NodePtr FunctionCallNode::Clone() const {
    auto clone = std::make_shared<FunctionCallNode>(function->Clone());
    for (const auto& arg : arguments) {
        clone->AddArgument(arg->Clone());
    }
    clone->SetPosition(line, column);
    return clone;
}

// ObjectLiteralNode实现
ObjectLiteralNode::ObjectLiteralNode()
    : CHTLJSNode(NodeType::OBJECT_LITERAL) {
}

void ObjectLiteralNode::AddProperty(const std::string& key, NodePtr value) {
    properties[key] = value;
    propertyOrder.push_back(key);
}

std::string ObjectLiteralNode::GenerateJS() const {
    std::stringstream ss;
    ss << "{";
    
    bool first = true;
    for (const auto& key : propertyOrder) {
        if (!first) ss << ", ";
        
        auto it = properties.find(key);
        if (it != properties.end()) {
            ss << key << ": " << it->second->GenerateJS();
        }
        
        first = false;
    }
    
    ss << "}";
    return ss.str();
}

NodePtr ObjectLiteralNode::Clone() const {
    auto clone = std::make_shared<ObjectLiteralNode>();
    for (const auto& key : propertyOrder) {
        auto it = properties.find(key);
        if (it != properties.end()) {
            clone->AddProperty(key, it->second->Clone());
        }
    }
    clone->SetPosition(line, column);
    return clone;
}

// ProgramNode实现
ProgramNode::ProgramNode()
    : CHTLJSNode(NodeType::PROGRAM) {
}

std::string ProgramNode::GenerateJS() const {
    std::stringstream ss;
    for (const auto& child : children) {
        ss << child->GenerateJS();
        if (child->GetType() == NodeType::STATEMENT) {
            ss << ";\n";
        }
    }
    return ss.str();
}

NodePtr ProgramNode::Clone() const {
    auto clone = std::make_shared<ProgramNode>();
    for (const auto& child : children) {
        clone->AddChild(child->Clone());
    }
    clone->SetPosition(line, column);
    return clone;
}

// StatementNode实现
StatementNode::StatementNode()
    : CHTLJSNode(NodeType::STATEMENT) {
}

std::string StatementNode::GenerateJS() const {
    std::stringstream ss;
    for (const auto& child : children) {
        ss << child->GenerateJS();
    }
    return ss.str();
}

NodePtr StatementNode::Clone() const {
    auto clone = std::make_shared<StatementNode>();
    for (const auto& child : children) {
        clone->AddChild(child->Clone());
    }
    clone->SetPosition(line, column);
    return clone;
}

} // namespace CHTLJS