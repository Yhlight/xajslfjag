#include "BaseNode.h"
#include <sstream>

namespace CHTLJS {

// ProgramNode 实现
void ProgramNode::addStatement(std::shared_ptr<BaseNode> stmt) {
    if (stmt) {
        statements.push_back(stmt);
    }
}

void ProgramNode::accept(NodeVisitor* visitor) {
    visitor->visitProgramNode(this);
}

std::shared_ptr<BaseNode> ProgramNode::clone() const {
    auto cloned = std::make_shared<ProgramNode>(line, column);
    
    // 递归克隆所有语句
    for (const auto& stmt : statements) {
        cloned->addStatement(stmt->clone());
    }
    
    return cloned;
}

std::string ProgramNode::toJavaScript() const {
    std::stringstream ss;
    
    for (size_t i = 0; i < statements.size(); ++i) {
        if (i > 0) ss << "\n";
        ss << statements[i]->toJavaScript();
    }
    
    return ss.str();
}

} // namespace CHTLJS