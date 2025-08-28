#pragma once

#include "CHTLBaseNode.h"

namespace CHTL {

// 前向声明
class CHTLStyleNode : public CHTLBaseNode {
public:
    CHTLStyleNode() : CHTLBaseNode(CHTLNodeType::STYLE) {}
    virtual ~CHTLStyleNode() = default;
    
    void accept(CHTLNodeVisitor* visitor) override {
        if (visitor) {
            visitor->visit(this);
        }
    }
    
    std::shared_ptr<CHTLBaseNode> clone() const override {
        return std::make_shared<CHTLStyleNode>();
    }
};

} // namespace CHTL