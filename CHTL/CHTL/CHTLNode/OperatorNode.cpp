#include "OperatorNode.h"
#include <sstream>

namespace CHTL {

void OperatorNode::addTarget(const std::string& target) {
    targets.push_back(target);
}

void OperatorNode::accept(NodeVisitor* visitor) {
    if (auto* operatorVisitor = dynamic_cast<OperatorNodeVisitor*>(visitor)) {
        operatorVisitor->visitOperatorNode(this);
    }
}

std::shared_ptr<BaseNode> OperatorNode::clone() const {
    auto cloned = std::make_shared<OperatorNode>(opType, line, column);
    
    // 复制属性
    cloned->targets = targets;
    cloned->useTarget = useTarget;
    cloned->insertPos = insertPos;
    cloned->insertSelector = insertSelector;
    
    // 递归克隆子节点
    for (const auto& child : children) {
        cloned->addChild(child->clone());
    }
    
    return cloned;
}

std::string OperatorNode::toString() const {
    std::stringstream ss;
    
    switch (opType) {
        case OperatorType::USE:
            ss << "use " << useTarget << ";";
            break;
            
        case OperatorType::DELETE:
            ss << "delete ";
            for (size_t i = 0; i < targets.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << targets[i];
            }
            ss << ";";
            break;
            
        case OperatorType::INSERT:
            ss << "insert ";
            switch (insertPos) {
                case InsertPosition::AFTER:
                    ss << "after ";
                    break;
                case InsertPosition::BEFORE:
                    ss << "before ";
                    break;
                case InsertPosition::REPLACE:
                    ss << "replace ";
                    break;
                case InsertPosition::AT_TOP:
                    ss << "at top ";
                    break;
                case InsertPosition::AT_BOTTOM:
                    ss << "at bottom ";
                    break;
            }
            if (!insertSelector.empty()) {
                ss << insertSelector << " ";
            }
            ss << "{\n";
            
            // 输出子节点
            for (const auto& child : children) {
                ss << "    " << child->toString() << "\n";
            }
            
            ss << "}";
            break;
            
        case OperatorType::EXCEPT:
            ss << "except ";
            for (size_t i = 0; i < targets.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << targets[i];
            }
            ss << ";";
            break;
    }
    
    return ss.str();
}

} // namespace CHTL