#ifndef CHTL_NAMESPACE_NODE_H
#define CHTL_NAMESPACE_NODE_H

#include "BaseNode.h"
#include <vector>

namespace CHTL {

// 命名空间节点类
class NamespaceNode : public BaseNode {
private:
    std::string name;
    std::vector<std::string> nestedPath;  // 嵌套命名空间路径
    std::vector<std::string> exceptList;  // 约束列表
    
public:
    NamespaceNode(const std::string& n, int line = 0, int col = 0)
        : BaseNode(NodeType::NAMESPACE, line, col), name(n) {}
    
    // 名称访问
    const std::string& getName() const { return name; }
    void setName(const std::string& n) { name = n; }
    
    // 嵌套路径管理
    void addToPath(const std::string& ns);
    const std::vector<std::string>& getNestedPath() const { return nestedPath; }
    std::string getFullPath() const;  // 获取完整的命名空间路径（如space.room2）
    
    // 约束管理
    void addExcept(const std::string& item);
    const std::vector<std::string>& getExceptList() const { return exceptList; }
    bool hasExceptions() const { return !exceptList.empty(); }
    bool isExcepted(const std::string& item) const;
    
    // 重写虚函数
    void accept(NodeVisitor* visitor) override;
    std::shared_ptr<BaseNode> clone() const override;
    std::string toString() const override;
};

// 扩展访问者接口
class NamespaceNodeVisitor : public NodeVisitor {
public:
    virtual void visitNamespaceNode(NamespaceNode* node) = 0;
};

} // namespace CHTL

#endif // CHTL_NAMESPACE_NODE_H