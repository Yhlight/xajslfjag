#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace AST {

// 前向声明
class Visitor;

// AST节点类型
enum class NodeType {
    // 根节点
    ROOT,
    
    // 基本节点
    TEXT_NODE,
    COMMENT_NODE,
    
    // HTML元素
    HTML_ELEMENT,
    
    // 属性节点
    ATTRIBUTE,
    
    // 样式相关
    STYLE_BLOCK,         // style { ... }
    STYLE_PROPERTY,      // color: red;
    STYLE_SELECTOR,      // .class, #id, &:hover等
    
    // 脚本相关
    SCRIPT_BLOCK,        // script { ... }
    
    // 模板系统
    TEMPLATE_STYLE,      // [Template] @Style
    TEMPLATE_ELEMENT,    // [Template] @Element
    TEMPLATE_VAR,        // [Template] @Var
    
    // 自定义系统
    CUSTOM_STYLE,        // [Custom] @Style
    CUSTOM_ELEMENT,      // [Custom] @Element
    CUSTOM_VAR,          // [Custom] @Var
    
    // 原始嵌入
    ORIGIN_BLOCK,        // [Origin] @Html/@Style/@JavaScript等
    
    // 配置
    CONFIGURATION,       // [Configuration]
    CONFIG_ITEM,         // 配置项
    
    // 命名空间
    NAMESPACE,           // [Namespace]
    
    // 导入
    IMPORT,              // [Import]
    
    // 信息和导出
    INFO,                // [Info]
    EXPORT,              // [Export]
    
    // 变量和函数
    VARIABLE,            // 变量定义
    FUNCTION_CALL,       // 函数调用
    
    // 特殊操作
    INHERIT,             // inherit
    DELETE_OP,           // delete
    INSERT_OP,           // insert
    
    // 约束
    EXCEPT,              // except
    
    // Use声明
    USE_DECLARATION,     // use
    
    // 字面量
    STRING_LITERAL,
    NUMBER_LITERAL,
    UNQUOTED_LITERAL,
    
    // 错误节点
    ERROR_NODE,
    
    UNKNOWN
};

// 位置信息
struct Position {
    std::string file_path;
    size_t line;
    size_t column;
    size_t start_pos;
    size_t end_pos;
    
    Position() : line(0), column(0), start_pos(0), end_pos(0) {}
    Position(const std::string& file, size_t l, size_t c, size_t start = 0, size_t end = 0)
        : file_path(file), line(l), column(c), start_pos(start), end_pos(end) {}
    
    std::string toString() const {
        return file_path + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// AST节点基类
class ASTNode {
public:
    NodeType type;
    Position position;
    std::string value;  // 节点的文本值
    std::unordered_map<std::string, std::string> attributes; // 属性映射
    std::vector<std::shared_ptr<ASTNode>> children; // 子节点
    std::weak_ptr<ASTNode> parent; // 父节点（弱引用避免循环引用）
    
    explicit ASTNode(NodeType t) : type(t) {}
    ASTNode(NodeType t, const std::string& v) : type(t), value(v) {}
    ASTNode(NodeType t, const std::string& v, const Position& pos) 
        : type(t), value(v), position(pos) {}
    
    virtual ~ASTNode() = default;
    
    // 访问者模式
    virtual void accept(Visitor& visitor) = 0;
    
    // 子节点操作
    void addChild(std::shared_ptr<ASTNode> child) {
        if (child) {
            child->parent = shared_from_this();
            children.push_back(child);
        }
    }
    
    void removeChild(std::shared_ptr<ASTNode> child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            (*it)->parent.reset();
            children.erase(it);
        }
    }
    
    void insertChild(size_t index, std::shared_ptr<ASTNode> child) {
        if (child && index <= children.size()) {
            child->parent = shared_from_this();
            children.insert(children.begin() + index, child);
        }
    }
    
    // 属性操作
    void setAttribute(const std::string& name, const std::string& value) {
        attributes[name] = value;
    }
    
    std::string getAttribute(const std::string& name, const std::string& default_value = "") const {
        auto it = attributes.find(name);
        return it != attributes.end() ? it->second : default_value;
    }
    
    bool hasAttribute(const std::string& name) const {
        return attributes.find(name) != attributes.end();
    }
    
    // 查找操作
    std::vector<std::shared_ptr<ASTNode>> findChildren(NodeType node_type) const {
        std::vector<std::shared_ptr<ASTNode>> result;
        for (const auto& child : children) {
            if (child->type == node_type) {
                result.push_back(child);
            }
        }
        return result;
    }
    
    std::shared_ptr<ASTNode> findFirstChild(NodeType node_type) const {
        for (const auto& child : children) {
            if (child->type == node_type) {
                return child;
            }
        }
        return nullptr;
    }
    
    std::shared_ptr<ASTNode> getParent() const {
        return parent.lock();
    }
    
    // 路径操作
    std::string getPath() const {
        std::vector<std::string> path_parts;
        auto current = shared_from_this();
        while (current && current->type != NodeType::ROOT) {
            if (!current->value.empty()) {
                path_parts.push_back(current->value);
            }
            current = current->getParent();
        }
        
        std::string path;
        for (auto it = path_parts.rbegin(); it != path_parts.rend(); ++it) {
            if (!path.empty()) path += ".";
            path += *it;
        }
        return path;
    }
    
    // 克隆
    virtual std::shared_ptr<ASTNode> clone() const = 0;
    
    // 序列化
    virtual std::string toString() const;
    virtual std::string toDebugString() const;
    
    // 验证
    virtual bool validate() const { return true; }
    
    // 类型检查
    bool isType(NodeType t) const { return type == t; }
    bool isAnyType(const std::vector<NodeType>& types) const {
        return std::find(types.begin(), types.end(), type) != types.end();
    }
    
    // 获取节点类型名称
    std::string getTypeName() const;
    
protected:
    // 使能shared_from_this
    virtual std::shared_ptr<ASTNode> shared_from_this() = 0;
    virtual std::shared_ptr<const ASTNode> shared_from_this() const = 0;
};

// 实际的AST节点基类（支持shared_from_this）
class ASTNodeImpl : public ASTNode, public std::enable_shared_from_this<ASTNodeImpl> {
public:
    explicit ASTNodeImpl(NodeType t) : ASTNode(t) {}
    ASTNodeImpl(NodeType t, const std::string& v) : ASTNode(t, v) {}
    ASTNodeImpl(NodeType t, const std::string& v, const Position& pos) 
        : ASTNode(t, v, pos) {}
    
protected:
    std::shared_ptr<ASTNode> shared_from_this() override {
        return std::enable_shared_from_this<ASTNodeImpl>::shared_from_this();
    }
    
    std::shared_ptr<const ASTNode> shared_from_this() const override {
        return std::enable_shared_from_this<ASTNodeImpl>::shared_from_this();
    }
};

// 工具函数
class ASTUtils {
public:
    // 获取节点类型名称
    static std::string getNodeTypeName(NodeType type);
    
    // 创建节点
    template<typename T, typename... Args>
    static std::shared_ptr<T> createNode(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    
    // 深度优先遍历
    static void traverseDepthFirst(std::shared_ptr<ASTNode> node, 
                                   std::function<void(std::shared_ptr<ASTNode>)> callback);
    
    // 广度优先遍历
    static void traverseBreadthFirst(std::shared_ptr<ASTNode> node,
                                     std::function<void(std::shared_ptr<ASTNode>)> callback);
    
    // 查找节点
    static std::vector<std::shared_ptr<ASTNode>> findNodesOfType(
        std::shared_ptr<ASTNode> root, NodeType type);
    
    // 打印AST树
    static std::string printTree(std::shared_ptr<ASTNode> root, int indent = 0);
    
    // 验证AST
    static bool validateTree(std::shared_ptr<ASTNode> root);
};

} // namespace AST
} // namespace CHTL