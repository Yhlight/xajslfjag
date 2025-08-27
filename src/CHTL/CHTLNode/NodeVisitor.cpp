#include "NodeVisitor.h"
#include "BaseNode.h"
#include "StyleNode.h"
#include "ScriptNode.h"

namespace CHTL {

// TraversalVisitor实现

void TraversalVisitor::VisitElement(ElementNode& node) {
    TraverseChildren(node);
}

void TraversalVisitor::VisitText(TextNode& node) {
    // 文本节点没有子节点，不需要遍历
}

void TraversalVisitor::VisitComment(CommentNode& node) {
    // 注释节点没有子节点，不需要遍历
}

void TraversalVisitor::VisitStyleBlock(StyleBlockNode& node) {
    TraverseChildren(node);
}

void TraversalVisitor::VisitScriptBlock(ScriptBlockNode& node) {
    TraverseChildren(node);
}

void TraversalVisitor::TraverseChildren(BaseNode& node) {
    for (const auto& child : node.GetChildren()) {
        if (child) {
            child->Accept(*this);
        }
    }
}

} // namespace CHTL