#ifndef CHTL_OPERATOR_NODE_H
#define CHTL_OPERATOR_NODE_H

#include "BaseNode.h"
#include <vector>

namespace CHTL {

// 操作符类型枚举
enum class OperatorType {
    USE,        // use操作符
    DELETE,     // delete操作符
    INSERT,     // insert操作符
    EXCEPT      // except约束操作符
};

// 插入位置类型
enum class InsertPosition {
    AFTER,      // after
    BEFORE,     // before
    REPLACE,    // replace
    AT_TOP,     // at top
    AT_BOTTOM   // at bottom
};

// 操作符节点类
class OperatorNode : public BaseNode {
private:
    OperatorType opType;
    std::vector<std::string> targets;  // 操作目标列表
    
    // use特有
    std::string useTarget;  // html5 或 @Config名称
    
    // insert特有
    InsertPosition insertPos;
    std::string insertSelector;  // 插入位置选择器
    
public:
    OperatorNode(OperatorType type, int line = 0, int col = 0)
        : BaseNode(NodeType::OPERATOR, line, col), opType(type), insertPos(InsertPosition::AFTER) {}
    
    // 操作符类型访问
    OperatorType getOperatorType() const { return opType; }
    void setOperatorType(OperatorType type) { opType = type; }
    
    // 目标管理
    void addTarget(const std::string& target);
    const std::vector<std::string>& getTargets() const { return targets; }
    void clearTargets() { targets.clear(); }
    
    // use操作符特有
    const std::string& getUseTarget() const { return useTarget; }
    void setUseTarget(const std::string& target) { useTarget = target; }
    bool isUseHtml5() const { return useTarget == "html5"; }
    
    // insert操作符特有
    InsertPosition getInsertPosition() const { return insertPos; }
    void setInsertPosition(InsertPosition pos) { insertPos = pos; }
    const std::string& getInsertSelector() const { return insertSelector; }
    void setInsertSelector(const std::string& sel) { insertSelector = sel; }
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class OperatorNodeVisitor : public NodeVisitor {
public:
    virtual void visitOperatorNode(OperatorNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_OPERATOR_NODE_H