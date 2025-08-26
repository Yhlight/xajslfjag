#ifndef CHTL_NODE_H
#define CHTL_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "../CHTLLexer/Token.h"

namespace CHTL {

// 前向声明
class Visitor;

// 节点类型
enum class NodeType {
    // 基础节点
    PROGRAM,
    COMMENT,
    
    // 文本和字面量
    TEXT_NODE,
    LITERAL,
    
    // HTML元素
    ELEMENT,
    ATTRIBUTE,
    
    // 样式和脚本块
    STYLE_BLOCK,
    SCRIPT_BLOCK,
    CSS_RULE,
    
    // 模板和自定义
    TEMPLATE_DECLARATION,
    CUSTOM_DECLARATION,
    TEMPLATE_USAGE,
    CUSTOM_USAGE,
    
    // 原始嵌入
    ORIGIN_EMBED,
    
    // 配置
    CONFIGURATION,
    CONFIG_OPTION,
    NAME_GROUP,
    ORIGIN_TYPE,
    
    // 导入
    IMPORT_STATEMENT,
    
    // 命名空间
    NAMESPACE_DECLARATION,
    
    // 约束
    EXCEPT_CONSTRAINT,
    
    // 特殊语法
    USE_STATEMENT,
    DELETE_STATEMENT,
    INSERT_STATEMENT,
    INHERIT_STATEMENT,
    
    // 表达式
    SELECTOR_EXPRESSION,
    VARIABLE_REFERENCE,
    INDEX_ACCESS,
    
    // 其他
    IDENTIFIER,
    BLOCK,
    PROPERTY,
    STATEMENT_LIST,
    FROM_CLAUSE
};

// 节点基类
class Node {
protected:
    NodeType type;
    Token startToken;
    Token endToken;
    std::weak_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    
    // 位置信息
    size_t line;
    size_t column;
    size_t endLine;
    size_t endColumn;
    
    // 元数据
    std::unordered_map<std::string, std::string> metadata;
    
public:
    Node(NodeType type, const Token& token);
    virtual ~Node() = default;
    
    // 基本访问器
    NodeType getType() const { return type; }
    size_t getLine() const { return line; }
    size_t getColumn() const { return column; }
    size_t getEndLine() const { return endLine; }
    size_t getEndColumn() const { return endColumn; }
    
    // 父子关系
    void setParent(std::shared_ptr<Node> parent) { this->parent = parent; }
    std::shared_ptr<Node> getParent() const { return parent.lock(); }
    
    // 子节点管理
    void addChild(std::shared_ptr<Node> child);
    void insertChild(size_t index, std::shared_ptr<Node> child);
    void removeChild(std::shared_ptr<Node> child);
    void removeChildAt(size_t index);
    std::vector<std::shared_ptr<Node>> getChildren() const { return children; }
    std::shared_ptr<Node> getChild(size_t index) const;
    size_t getChildCount() const { return children.size(); }
    
    // 查找子节点
    std::shared_ptr<Node> findChild(NodeType type) const;
    std::vector<std::shared_ptr<Node>> findAllChildren(NodeType type) const;
    std::shared_ptr<Node> findChildByName(const std::string& name) const;
    
    // Token管理
    void setStartToken(const Token& token) { startToken = token; }
    void setEndToken(const Token& token) { endToken = token; }
    Token getStartToken() const { return startToken; }
    Token getEndToken() const { return endToken; }
    
    // 位置更新
    void updatePosition(size_t line, size_t column, size_t endLine, size_t endColumn);
    
    // 元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    
    // 访问者模式
    virtual void accept(Visitor* visitor) = 0;
    
    // 调试
    virtual std::string toString() const = 0;
    virtual void dump(int indent = 0) const;
    
    // 克隆
    virtual std::shared_ptr<Node> clone() const = 0;
};

// 访问者接口
class Visitor {
public:
    virtual ~Visitor() = default;
    
    // 通用访问方法
    virtual void visit(Node* node) = 0;
    
    // 每种节点类型的访问方法将在具体节点类中定义
};

} // namespace CHTL

#endif // CHTL_NODE_H