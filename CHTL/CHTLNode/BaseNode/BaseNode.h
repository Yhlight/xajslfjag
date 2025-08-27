#pragma once
#include "../../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {

// 前向声明
class CHTLContext;
class CHTLGenerator;

/**
 * CHTL AST节点类型枚举
 */
enum class NodeType {
    // 基础节点
    BASE_NODE,
    ELEMENT_NODE,
    TEXT_NODE,
    
    // 结构节点
    COMMENT_NODE,
    TEMPLATE_NODE,
    CUSTOM_NODE,
    STYLE_NODE,
    SCRIPT_NODE,
    ORIGIN_NODE,
    IMPORT_NODE,
    CONFIG_NODE,
    NAMESPACE_NODE,
    
    // 操作节点
    OPERATOR_NODE,
    DELETE_NODE,
    INSERT_NODE,
    INHERIT_NODE,
    
    // 特殊节点
    USE_NODE,
    EXCEPT_NODE,
    FROM_NODE
};

/**
 * CHTL AST基础节点类
 * 所有CHTL节点的抽象基类
 */
class BaseNode : public std::enable_shared_from_this<BaseNode> {
public:
    explicit BaseNode(NodeType type, const Position& pos = Position{});
    virtual ~BaseNode() = default;
    
    // 节点类型
    NodeType getType() const { return type_; }
    bool isType(NodeType type) const { return type_ == type; }
    
    // 位置信息
    const Position& getPosition() const { return position_; }
    void setPosition(const Position& pos) { position_ = pos; }
    
    // 父子关系
    void addChild(std::shared_ptr<BaseNode> child);
    void removeChild(std::shared_ptr<BaseNode> child);
    void insertChild(size_t index, std::shared_ptr<BaseNode> child);
    std::shared_ptr<BaseNode> getChild(size_t index) const;
    size_t getChildCount() const { return children_.size(); }
    const std::vector<std::shared_ptr<BaseNode>>& getChildren() const { return children_; }
    
    // 父节点
    std::shared_ptr<BaseNode> getParent() const { return parent_.lock(); }
    void setParent(std::shared_ptr<BaseNode> parent) { parent_ = parent; }
    
    // 属性管理
    void setAttribute(const std::string& name, const std::string& value);
    std::string getAttribute(const std::string& name) const;
    bool hasAttribute(const std::string& name) const;
    void removeAttribute(const std::string& name);
    const std::unordered_map<std::string, std::string>& getAttributes() const { return attributes_; }
    
    // 内容管理
    void setContent(const std::string& content) { content_ = content; }
    const std::string& getContent() const { return content_; }
    
    // 名称管理
    void setName(const std::string& name) { name_ = name; }
    const std::string& getName() const { return name_; }
    
    // 节点查找
    std::shared_ptr<BaseNode> findChild(NodeType type) const;
    std::vector<std::shared_ptr<BaseNode>> findChildren(NodeType type) const;
    std::shared_ptr<BaseNode> findChildByName(const std::string& name) const;
    
    // 节点遍历
    void traverse(std::function<void(std::shared_ptr<BaseNode>)> visitor);
    void traverseDepthFirst(std::function<void(std::shared_ptr<BaseNode>)> visitor);
    void traverseBreadthFirst(std::function<void(std::shared_ptr<BaseNode>)> visitor);
    
    // 抽象方法 - 子类必须实现
    virtual std::string toString() const = 0;
    virtual std::shared_ptr<BaseNode> clone() const = 0;
    virtual void accept(CHTLGenerator* generator) = 0;
    
    // 验证
    virtual bool validate() const { return true; }
    virtual std::vector<std::string> getValidationErrors() const { return {}; }
    
    // 元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
protected:
    NodeType type_;
    Position position_;
    std::vector<std::shared_ptr<BaseNode>> children_;
    std::weak_ptr<BaseNode> parent_;
    std::unordered_map<std::string, std::string> attributes_;
    std::unordered_map<std::string, std::string> metadata_;
    std::string content_;
    std::string name_;
    
    // 辅助方法
    virtual void onChildAdded(std::shared_ptr<BaseNode> /*child*/) {}
    virtual void onChildRemoved(std::shared_ptr<BaseNode> /*child*/) {}
    virtual void onAttributeChanged(const std::string& /*name*/, const std::string& /*value*/) {}
};

/**
 * 节点类型到字符串转换
 */
std::string nodeTypeToString(NodeType type);

/**
 * 节点工厂
 */
class NodeFactory {
public:
    static std::shared_ptr<BaseNode> createNode(NodeType type, const Position& pos = Position{});
    template<typename T>
    static std::shared_ptr<T> createNode(const Position& pos = Position{});
};

} // namespace CHTL