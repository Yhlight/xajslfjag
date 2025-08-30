#include "ProgramNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "TemplateNode.h"
#include "OperatorNode.h"
#include "CustomNode.h"
#include "OriginNode.h"
#include "ImportNode.h"
#include "NamespaceNode.h"
#include "ConfigNode.h"
#include "../CHTLGenerator/Generator.h"
#include <sstream>

namespace CHTL {

// ProgramNode implementation
void ProgramNode::accept(Visitor* visitor) {
    // Visitor needs to be extended to support ProgramNode
    // For now, just visit all children
    for (auto& node : topLevelNodes_) {
        if (node) {
            node->accept(visitor);
        }
    }
}

std::string ProgramNode::toString() const {
    std::stringstream ss;
    ss << "ProgramNode(" << filename_ << ", nodes=" << topLevelNodes_.size() << ")";
    return ss.str();
}

// CommentNode implementation
void CommentNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitCommentNode to Visitor interface
}

std::string CommentNode::toString() const {
    std::stringstream ss;
    ss << "CommentNode(type=";
    switch (commentType_) {
        case CommentType::SINGLE_LINE: ss << "SINGLE_LINE"; break;
        case CommentType::MULTI_LINE: ss << "MULTI_LINE"; break;
        case CommentType::GENERATOR: ss << "GENERATOR"; break;
    }
    ss << ", content=\"" << content_ << "\")";
    return ss.str();
}



// StyleNode implementation
void StyleNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitStyleNode to Visitor interface
}

std::string StyleNode::toString() const {
    std::stringstream ss;
    ss << "StyleNode(type=";
    switch (blockType_) {
        case StyleBlockType::LOCAL: ss << "LOCAL"; break;
        case StyleBlockType::GLOBAL: ss << "GLOBAL"; break;
    }
    ss << ", rules=" << rules_.size() << ")";
    return ss.str();
}

// ScriptNode implementation
void ScriptNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitScriptNode to Visitor interface
}

std::string ScriptNode::toString() const {
    std::stringstream ss;
    ss << "ScriptNode(type=";
    switch (blockType_) {
        case ScriptBlockType::LOCAL: ss << "LOCAL"; break;
        case ScriptBlockType::GLOBAL: ss << "GLOBAL"; break;
    }
    ss << ", content=\"" << content_ << "\")";
    return ss.str();
}

// TemplateNode implementation
void TemplateNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitTemplateNode to Visitor interface
}

std::string TemplateNode::toString() const {
    std::stringstream ss;
    ss << "TemplateNode(type=";
    switch (templateType_) {
        case TemplateType::STYLE: ss << "STYLE"; break;
        case TemplateType::ELEMENT: ss << "ELEMENT"; break;
        case TemplateType::VAR: ss << "VAR"; break;
    }
    ss << ", name=\"" << name_ << "\")";
    return ss.str();
}

// UseNode implementation
void UseNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitUseNode to Visitor interface
}

std::string UseNode::toString() const {
    std::stringstream ss;
    ss << "UseNode(target=\"" << target_ << "\")";
    return ss.str();
}

// SelectorNode implementation
void SelectorNode::accept(Visitor* /*visitor*/) {
    // TODO: Add visitSelectorNode to Visitor interface
}

std::string SelectorNode::toString() const {
    std::stringstream ss;
    ss << "SelectorNode(type=";
    switch (selectorType_) {
        case SelectorType::CLASS: ss << "CLASS"; break;
        case SelectorType::ID: ss << "ID"; break;
        case SelectorType::TAG: ss << "TAG"; break;
        case SelectorType::PSEUDO_CLASS: ss << "PSEUDO_CLASS"; break;
        case SelectorType::PSEUDO_ELEMENT: ss << "PSEUDO_ELEMENT"; break;
        case SelectorType::REFERENCE: ss << "REFERENCE"; break;
        case SelectorType::COMPOUND: ss << "COMPOUND"; break;
    }
    ss << ", selector=\"" << selector_ << "\")";
    return ss.str();
}

// CustomNode implementation
void CustomNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<CustomVisitor*>(visitor)) {
        v->visitCustomNode(this);
    }
}

std::string CustomNode::toString() const {
    std::string typeStr;
    switch (customType_) {
        case CustomType::STYLE: typeStr = "@Style"; break;
        case CustomType::ELEMENT: typeStr = "@Element"; break;
        case CustomType::VAR: typeStr = "@Var"; break;
    }
    return "[Custom] " + typeStr + " " + name_;
}

// OriginNode implementation
void OriginNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<OriginVisitor*>(visitor)) {
        v->visitOriginNode(this);
    }
}

std::string OriginNode::toString() const {
    std::string typeStr;
    switch (type_) {
        case OriginType::HTML: typeStr = "@Html"; break;
        case OriginType::STYLE: typeStr = "@Style"; break;
        case OriginType::JAVASCRIPT: typeStr = "@JavaScript"; break;
        case OriginType::CUSTOM: typeStr = customType_; break;
    }
    return "[Origin] " + typeStr + (name_.empty() ? "" : " " + name_);
}

// ImportNode implementation
void ImportNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<ImportVisitor*>(visitor)) {
        v->visitImportNode(this);
    }
}

std::string ImportNode::toString() const {
    return "[Import] from " + fromPath_;
}

// NamespaceNode implementation
void NamespaceNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<NamespaceVisitor*>(visitor)) {
        v->visitNamespaceNode(this);
    }
}

std::string NamespaceNode::toString() const {
    return "[Namespace] " + name_;
}

// ConfigNode implementation
void ConfigNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<ConfigVisitor*>(visitor)) {
        v->visitConfigNode(this);
    }
}

std::string ConfigNode::toString() const {
    return "[Configuration]" + (name_.empty() ? "" : " @Config " + name_);
}

// ExceptNode implementation
void ExceptNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<OperatorVisitor*>(visitor)) {
        v->visitExceptNode(this);
    }
}

std::string ExceptNode::toString() const {
    return "except " + std::to_string(constraints_.size()) + " constraints";
}

// PropertyNode implementation
void PropertyNode::accept(Visitor* visitor) {
    // TODO: Add visitPropertyNode to Visitor interface
    (void)visitor;
}

std::string PropertyNode::toString() const {
    return name_ + ": " + value_;
}

// TemplateUseNode implementation
void TemplateUseNode::accept(Visitor* visitor) {
    if (auto* v = dynamic_cast<TemplateVisitor*>(visitor)) {
        v->visitTemplateUseNode(this);
    }
}

std::string TemplateUseNode::toString() const {
    std::string typeStr;
    switch (templateType_) {
        case TemplateType::STYLE: typeStr = "@Style"; break;
        case TemplateType::ELEMENT: typeStr = "@Element"; break;
        case TemplateType::VAR: typeStr = "@Var"; break;
    }
    return typeStr + " " + name_;
}

} // namespace CHTL