#ifndef CHTLJS_MODULE_NODE_H
#define CHTLJS_MODULE_NODE_H

#include "BaseNode.h"
#include <vector>

namespace CHTLJS {

// 模块节点（module {}）
class ModuleNode : public ASTNode {
public:
    ModuleNode(const TokenLocation& location)
        : ASTNode(NodeType::MODULE_BLOCK, location) {}
    
    // 添加加载项
    void addLoadItem(const std::string& path) {
        loadItems_.push_back(path);
    }
    
    const std::vector<std::string>& getLoadItems() const {
        return loadItems_;
    }
    
    void accept(Visitor* visitor) override;
    std::string toString() const override;
    
private:
    std::vector<std::string> loadItems_;
};

// 扩展访问者接口
class ModuleVisitor : public Visitor {
public:
    virtual void visitModuleNode(ModuleNode* node) = 0;
};

} // namespace CHTLJS

#endif // CHTLJS_MODULE_NODE_H