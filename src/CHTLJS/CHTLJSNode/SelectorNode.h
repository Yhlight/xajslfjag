#ifndef CHTLJS_SELECTOR_NODE_H
#define CHTLJS_SELECTOR_NODE_H

#include "BaseNode.h"
#include <optional>

namespace CHTLJS {

// 增强选择器节点（{{selector}}）
class EnhancedSelectorNode : public ASTNode {
public:
    enum class SelectorType {
        CLASS,      // .className
        ID,         // #id
        TAG,        // tagName
        COMPOUND,   // 复合选择器
        REFERENCE   // & 引用选择器
    };
    
    EnhancedSelectorNode(const std::string& selector, SelectorType type,
                        const TokenLocation& location)
        : ASTNode(NodeType::ENHANCED_SELECTOR, location),
          selector_(selector), selectorType_(type) {}
    
    const std::string& getSelector() const { return selector_; }
    SelectorType getSelectorType() const { return selectorType_; }
    
    // 设置索引（如 {{button[0]}}）
    void setIndex(size_t index) {
        index_ = index;
    }
    
    std::optional<size_t> getIndex() const {
        return index_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::string selector_;
    SelectorType selectorType_;
    std::optional<size_t> index_;
};

// 扩展访问者接口
class SelectorVisitor : public Visitor {
public:
    virtual void visitEnhancedSelectorNode(EnhancedSelectorNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_SELECTOR_NODE_H