#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

// 前向声明
class CHTLScriptNode : public CHTLBaseNode {
public:
    CHTLScriptNode() : CHTLBaseNode(CHTLNodeType::SCRIPT) {}
    virtual ~CHTLScriptNode() = default;
    
    void accept(CHTLNodeVisitor* visitor) override {
        if (visitor) {
            visitor->visit(this);
        }
    }
    
    std::shared_ptr<CHTLBaseNode> clone() const override {
        return std::make_shared<CHTLScriptNode>();
    }
};

} // namespace CHTL