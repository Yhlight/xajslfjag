#include "ProgramNode.h"
#include "CommentNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"
#include "TemplateNode.h"
#include "OperatorNode.h"
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

} // namespace CHTL