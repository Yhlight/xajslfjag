#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

enum class NodeType {
    DOCUMENT,
    ELEMENT,
    TEXT,
    ATTRIBUTE,
    STYLE,
    SCRIPT,
    TEMPLATE,
    CUSTOM,
    ORIGIN,
    IMPORT,
    NAMESPACE,
    CONFIGURATION,
    COMMENT,
    VARIABLE,
    OPERATION,
    
    // CHTL JS节点类型
    CHTL_JS_DOCUMENT,
    MODULE_IMPORT,
    ENHANCED_SELECTOR,
    EVENT_BINDING,
    ANIMATE,
    LISTEN,
    VIRTUAL_OBJECT,
    DELEGATE,
    EVENT_LISTENER
};

class BaseNode {
public:
    BaseNode(NodeType type = NodeType::ELEMENT);
    virtual ~BaseNode();
    
    // 节点类型
    NodeType getType() const { return m_type; }
    void setType(NodeType type) { m_type = type; }
    
    // 节点值
    std::string getValue() const { return m_value; }
    void setValue(const std::string& value) { m_value = value; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const { return m_attributes; }
    
    // 子节点管理
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return m_children; }
    size_t getChildCount() const { return m_children.size(); }
    
    // 父节点
    void setParent(std::shared_ptr<BaseNode> parent) { m_parent = parent; }
    std::shared_ptr<BaseNode> getParent() const { return m_parent.lock(); }
    
    // 查找方法
    std::shared_ptr<BaseNode> findChild(NodeType type) const;
    std::shared_ptr<BaseNode> findChild(const std::string& value) const;
    std::vector<std::shared_ptr<BaseNode>> findChildren(NodeType type) const;
    
    // 工具方法
    std::string toString() const;
    std::shared_ptr<BaseNode> clone() const;
    
private:
    NodeType m_type;
    std::string m_value;
    std::unordered_map<std::string, std::string> m_attributes;
    std::vector<std::shared_ptr<BaseNode>> m_children;
    std::weak_ptr<BaseNode> m_parent;
};

}