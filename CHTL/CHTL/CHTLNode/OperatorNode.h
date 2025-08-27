#pragma once

#include "BaseNode.h"
#include <vector>

namespace CHTL {

// 操作符类型
enum class OperatorType {
    DELETE,     // delete操作
    INSERT,     // insert操作
    USE,        // use操作
    INHERIT,    // inherit继承
    EXCEPT      // except约束
};

// 插入位置
enum class InsertPosition {
    AFTER,      // after
    BEFORE,     // before
    REPLACE,    // replace
    AT_TOP,     // at top
    AT_BOTTOM   // at bottom
};

// 操作符节点
class OperatorNode : public BaseNode {
public:
    OperatorNode(OperatorType type);
    
    OperatorType GetOperatorType() const { return operatorType; }
    
    std::string Generate() const override;
    NodePtr Clone() const override;
    void Accept(NodeVisitor* visitor) override;
    
    // DELETE操作
    void AddDeleteTarget(const std::string& target);
    std::vector<std::string> GetDeleteTargets() const { return deleteTargets; }
    
    // INSERT操作
    void SetInsertPosition(InsertPosition pos) { insertPosition = pos; }
    void SetInsertSelector(const std::string& selector) { insertSelector = selector; }
    void SetInsertContent(NodePtr content) { insertContent = content; }
    InsertPosition GetInsertPosition() const { return insertPosition; }
    std::string GetInsertSelector() const { return insertSelector; }
    NodePtr GetInsertContent() const { return insertContent; }
    
    // USE操作
    void SetUseTarget(const std::string& target) { useTarget = target; }
    std::string GetUseTarget() const { return useTarget; }
    
    // INHERIT操作
    void AddInheritTarget(const std::string& type, const std::string& name);
    std::vector<std::pair<std::string, std::string>> GetInheritTargets() const { return inheritTargets; }
    
    // EXCEPT操作
    void AddExceptItem(const std::string& item);
    void AddExceptType(const std::string& type);
    std::vector<std::string> GetExceptItems() const { return exceptItems; }
    std::vector<std::string> GetExceptTypes() const { return exceptTypes; }
    
private:
    OperatorType operatorType;
    
    // DELETE操作的目标
    std::vector<std::string> deleteTargets;
    
    // INSERT操作的信息
    InsertPosition insertPosition;
    std::string insertSelector;
    NodePtr insertContent;
    
    // USE操作的目标
    std::string useTarget;
    
    // INHERIT操作的目标
    std::vector<std::pair<std::string, std::string>> inheritTargets;
    
    // EXCEPT操作的项
    std::vector<std::string> exceptItems;
    std::vector<std::string> exceptTypes;
};

} // namespace CHTL